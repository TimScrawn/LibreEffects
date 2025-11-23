#include "tool.h"
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QQueue>
#include <QSet>
#include <cmath>
#include <algorithm>

namespace LibreCanvas {

    // Brush Tool Implementation
    void BrushTool::onMousePress(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (!doc || event->button() != Qt::LeftButton) return;
        
        auto activeLayer = doc->getActiveLayer();
        if (!activeLayer || activeLayer->isLocked()) return;
        
        m_isDrawing = true;
        m_lastPos = imagePos;
        
        QImage& layerImage = activeLayer->getImage();
        QPainter painter(&layerImage);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        
        drawBrush(painter, imagePos, m_size, m_hardness, m_color, m_opacity);
        painter.end();
    }

    void BrushTool::onMouseMove(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (!m_isDrawing || !doc) return;
        
        auto activeLayer = doc->getActiveLayer();
        if (!activeLayer || activeLayer->isLocked()) return;
        
        QImage& layerImage = activeLayer->getImage();
        QPainter painter(&layerImage);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        
        // Draw line between last position and current
        QPoint p1 = m_lastPos;
        QPoint p2 = imagePos;
        int steps = qMax(abs(p2.x() - p1.x()), abs(p2.y() - p1.y()));
        
        for (int i = 0; i <= steps; ++i) {
            float t = static_cast<float>(i) / steps;
            QPoint pos(
                static_cast<int>(p1.x() * (1.0f - t) + p2.x() * t),
                static_cast<int>(p1.y() * (1.0f - t) + p2.y() * t)
            );
            drawBrush(painter, pos, m_size, m_hardness, m_color, m_opacity * m_flow);
        }
        
        m_lastPos = imagePos;
        painter.end();
    }

    void BrushTool::onMouseRelease(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        m_isDrawing = false;
    }

    void BrushTool::drawBrush(QPainter& painter, const QPoint& pos, int size, float hardness, const QColor& color, float opacity)
    {
        QColor brushColor = color;
        brushColor.setAlphaF(color.alphaF() * opacity);
        
        if (hardness >= 1.0f) {
            // Hard brush
            painter.setPen(Qt::NoPen);
            painter.setBrush(brushColor);
            painter.drawEllipse(pos, size / 2, size / 2);
        } else {
            // Soft brush with gradient
            QRadialGradient gradient(pos, size / 2.0f);
            float softEdge = size * (1.0f - hardness) / 2.0f;
            
            gradient.setColorAt(0.0, brushColor);
            gradient.setColorAt(hardness, brushColor);
            QColor transparent = brushColor;
            transparent.setAlphaF(0.0f);
            gradient.setColorAt(1.0, transparent);
            
            painter.setPen(Qt::NoPen);
            painter.setBrush(gradient);
            painter.drawEllipse(pos, size / 2, size / 2);
        }
    }

    // Eraser Tool Implementation
    void EraserTool::onMousePress(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (!doc || event->button() != Qt::LeftButton) return;
        
        auto activeLayer = doc->getActiveLayer();
        if (!activeLayer || activeLayer->isLocked()) return;
        
        m_isErasing = true;
        m_lastPos = imagePos;
        
        QImage& layerImage = activeLayer->getImage();
        QPainter painter(&layerImage);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        
        eraseBrush(painter, imagePos, m_size, m_hardness, m_opacity);
        painter.end();
    }

    void EraserTool::onMouseMove(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (!m_isErasing || !doc) return;
        
        auto activeLayer = doc->getActiveLayer();
        if (!activeLayer || activeLayer->isLocked()) return;
        
        QImage& layerImage = activeLayer->getImage();
        QPainter painter(&layerImage);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        
        QPoint p1 = m_lastPos;
        QPoint p2 = imagePos;
        int steps = qMax(abs(p2.x() - p1.x()), abs(p2.y() - p1.y()));
        
        for (int i = 0; i <= steps; ++i) {
            float t = static_cast<float>(i) / steps;
            QPoint pos(
                static_cast<int>(p1.x() * (1.0f - t) + p2.x() * t),
                static_cast<int>(p1.y() * (1.0f - t) + p2.y() * t)
            );
            eraseBrush(painter, pos, m_size, m_hardness, m_opacity);
        }
        
        m_lastPos = imagePos;
        painter.end();
    }

    void EraserTool::onMouseRelease(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        m_isErasing = false;
    }

    void EraserTool::eraseBrush(QPainter& painter, const QPoint& pos, int size, float hardness, float opacity)
    {
        QColor eraseColor(0, 0, 0, static_cast<int>(255 * opacity));
        
        if (hardness >= 1.0f) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(eraseColor);
            painter.drawEllipse(pos, size / 2, size / 2);
        } else {
            QRadialGradient gradient(pos, size / 2.0f);
            gradient.setColorAt(0.0, eraseColor);
            gradient.setColorAt(hardness, eraseColor);
            QColor transparent(0, 0, 0, 0);
            gradient.setColorAt(1.0, transparent);
            
            painter.setPen(Qt::NoPen);
            painter.setBrush(gradient);
            painter.drawEllipse(pos, size / 2, size / 2);
        }
    }

    // Marquee Rect Tool Implementation
    void MarqueeRectTool::onMousePress(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (event->button() != Qt::LeftButton) return;
        m_isSelecting = true;
        m_startPos = imagePos;
        m_selection = QRect();
    }

    void MarqueeRectTool::onMouseMove(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (!m_isSelecting) return;
        m_selection = QRect(m_startPos, imagePos).normalized();
    }

    void MarqueeRectTool::onMouseRelease(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (event->button() == Qt::LeftButton) {
            m_isSelecting = false;
            m_selection = QRect(m_startPos, imagePos).normalized();
        }
    }

    // Magic Wand Tool Implementation
    void MagicWandTool::onMousePress(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (!doc || event->button() != Qt::LeftButton) return;
        
        auto activeLayer = doc->getActiveLayer();
        if (!activeLayer) return;
        
        const QImage& image = activeLayer->getImage();
        if (imagePos.x() < 0 || imagePos.x() >= image.width() ||
            imagePos.y() < 0 || imagePos.y() >= image.height()) {
            return;
        }
        
        m_selection = floodFill(image, imagePos, m_tolerance);
    }

    QRegion MagicWandTool::floodFill(const QImage& image, const QPoint& start, int tolerance)
    {
        QRegion region;
        if (start.x() < 0 || start.x() >= image.width() ||
            start.y() < 0 || start.y() >= image.height()) {
            return region;
        }
        
        QColor targetColor = image.pixelColor(start);
        QSet<QPoint> visited;
        QQueue<QPoint> queue;
        queue.enqueue(start);
        visited.insert(start);
        
        while (!queue.isEmpty()) {
            QPoint current = queue.dequeue();
            
            // Check if this pixel matches
            QColor currentColor = image.pixelColor(current);
            int diff = qAbs(targetColor.red() - currentColor.red()) +
                      qAbs(targetColor.green() - currentColor.green()) +
                      qAbs(targetColor.blue() - currentColor.blue());
            
            if (diff <= tolerance * 3) {
                region += QRect(current.x(), current.y(), 1, 1);
                
                // Check neighbors
                QPoint neighbors[] = {
                    QPoint(current.x() + 1, current.y()),
                    QPoint(current.x() - 1, current.y()),
                    QPoint(current.x(), current.y() + 1),
                    QPoint(current.x(), current.y() - 1)
                };
                
                for (const QPoint& neighbor : neighbors) {
                    if (neighbor.x() >= 0 && neighbor.x() < image.width() &&
                        neighbor.y() >= 0 && neighbor.y() < image.height() &&
                        !visited.contains(neighbor)) {
                        visited.insert(neighbor);
                        queue.enqueue(neighbor);
                    }
                }
            }
        }
        
        return region;
    }

} // namespace LibreCanvas

