#include "clonestamptool.h"
#include <QPainter>
#include <QRadialGradient>

namespace LibreCanvas {

    void CloneStampTool::onMousePress(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (!doc || event->button() != Qt::LeftButton) return;
        
        auto activeLayer = doc->getActiveLayer();
        if (!activeLayer || activeLayer->isLocked()) return;
        
        if (event->modifiers() & Qt::AltModifier) {
            // Set source point and capture source image
            m_sourcePoint = imagePos;
            m_sourceImage = activeLayer->getImage().copy();
            m_sourceSet = true;
            return;
        }
        
        if (!m_sourceSet) {
            // Default to same point if source not set
            m_sourcePoint = imagePos;
            m_sourceImage = activeLayer->getImage().copy();
            m_sourceSet = true;
        }
        
        m_isCloning = true;
        m_lastDestPos = imagePos;
        
        QImage& layerImage = activeLayer->getImage();
        QPainter painter(&layerImage);
        painter.setRenderHint(QPainter::Antialiasing);
        
        cloneBrush(painter, m_sourceImage, m_sourcePoint, imagePos, m_size, m_hardness, m_opacity);
        painter.end();
        
        // Note: History is saved in canvas widget before tool operation
    }

    void CloneStampTool::onMouseMove(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (!m_isCloning || !doc || !m_sourceSet) return;
        
        auto activeLayer = doc->getActiveLayer();
        if (!activeLayer || activeLayer->isLocked()) return;
        
        QImage& layerImage = activeLayer->getImage();
        QPainter painter(&layerImage);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Calculate offset
        QPoint offset = imagePos - m_lastDestPos;
        m_sourcePoint += offset;
        
        cloneBrush(painter, m_sourceImage, m_sourcePoint, imagePos, m_size, m_hardness, m_opacity);
        
        m_lastDestPos = imagePos;
        painter.end();
    }

    void CloneStampTool::onMouseRelease(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        m_isCloning = false;
    }

    void CloneStampTool::cloneBrush(QPainter& painter, const QImage& sourceImage, const QPoint& sourcePos, const QPoint& destPos, int size, float hardness, float opacity)
    {
        
        QRect sourceRect(sourcePos.x() - size/2, sourcePos.y() - size/2, size, size);
        QRect destRect(destPos.x() - size/2, destPos.y() - size/2, size, size);
        
        // Clamp source rect to image bounds
        sourceRect = sourceRect.intersected(QRect(0, 0, sourceImage.width(), sourceImage.height()));
        destRect = destRect.intersected(QRect(0, 0, painter.device()->width(), painter.device()->height()));
        
        if (sourceRect.isEmpty() || destRect.isEmpty()) return;
        
        if (hardness >= 1.0f) {
            // Hard brush - direct copy
            painter.setOpacity(opacity);
            painter.drawImage(destRect, sourceImage, sourceRect);
        } else {
            // Soft brush with mask
            QImage mask(size, size, QImage::Format_ARGB32);
            mask.fill(Qt::transparent);
            QPainter maskPainter(&mask);
            
            QRadialGradient gradient(QPoint(size/2, size/2), size/2.0f);
            QColor white(255, 255, 255, static_cast<int>(255 * opacity));
            gradient.setColorAt(0.0, white);
            gradient.setColorAt(hardness, white);
            QColor transparent(255, 255, 255, 0);
            gradient.setColorAt(1.0, transparent);
            
            maskPainter.setBrush(gradient);
            maskPainter.setPen(Qt::NoPen);
            maskPainter.drawEllipse(0, 0, size, size);
            maskPainter.end();
            
            // Apply mask and draw
            QImage sourceCopy = sourceImage.copy(sourceRect).scaled(size, size);
            for (int y = 0; y < size && y < sourceCopy.height(); ++y) {
                for (int x = 0; x < size && x < sourceCopy.width(); ++x) {
                    if (x < mask.width() && y < mask.height()) {
                        QColor srcColor = sourceCopy.pixelColor(x, y);
                        int alpha = qGray(mask.pixel(x, y));
                        srcColor.setAlpha(srcColor.alpha() * alpha / 255);
                        sourceCopy.setPixelColor(x, y, srcColor);
                    }
                }
            }
            
            painter.drawImage(destRect.topLeft(), sourceCopy);
        }
    }

} // namespace LibreCanvas

