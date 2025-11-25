#include "canvaswidget.h"
#include "tool.h"
#include "history.h"
#include "transformtool.h"
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFileInfo>
#include <QMessageBox>
#include <QPen>
#include <QBrush>
#include <QApplication>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent)
    , m_document(nullptr)
    , m_zoomLevel(1.0f)
    , m_isPanning(false)
{
    setMinimumSize(400, 300);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    
    // Set background pattern for transparency
    setStyleSheet("background-color: #2a2a2a;");
    
    // Set default tool (Brush)
    m_currentTool = std::make_shared<LibreCanvas::BrushTool>();
}

bool CanvasWidget::loadImage(const QString &filePath)
{
    QImage loadedImage;
    if (!loadedImage.load(filePath)) {
        QMessageBox::warning(this, "Load Error", 
            QString("Failed to load image:\n%1").arg(filePath));
        return false;
    }
    
    // Create document with loaded image
    m_document = std::make_shared<LibreCanvas::Document>(loadedImage.width(), loadedImage.height());
    if (m_historyManager) {
        m_document->setHistoryManager(m_historyManager.get());
    }
    auto layer = std::make_shared<LibreCanvas::Layer>("Layer 1", loadedImage);
    m_document->addLayer(layer);
    m_document->saveState("Load Image");
    
    m_zoomLevel = 1.0f;
    m_panDelta = QPoint(0, 0);
    updatePixmap();
    update();
    
    emit imageChanged();
    emit documentChanged();
    return true;
}

bool CanvasWidget::saveImage(const QString &filePath)
{
    if (!m_document) {
        QMessageBox::warning(this, "Save Error", "No document to save.");
        return false;
    }
    
    QString format = QFileInfo(filePath).suffix().toUpper();
    if (format.isEmpty()) {
        format = "PNG";
    }
    
    QImage rendered = m_document->render();
    if (!rendered.save(filePath, format.toLatin1().constData())) {
        QMessageBox::warning(this, "Save Error", 
            QString("Failed to save image:\n%1").arg(filePath));
        return false;
    }
    
    return true;
}

void CanvasWidget::newImage(int width, int height)
{
    m_document = std::make_shared<LibreCanvas::Document>(width, height, Qt::white);
    if (m_historyManager) {
        m_document->setHistoryManager(m_historyManager.get());
    }
    m_zoomLevel = 1.0f;
    m_panDelta = QPoint(0, 0);
    updatePixmap();
    update();
    
    emit imageChanged();
    emit documentChanged();
}

void CanvasWidget::setDocument(std::shared_ptr<LibreCanvas::Document> document)
{
    m_document = document;
    if (m_document && m_historyManager) {
        m_document->setHistoryManager(m_historyManager.get());
    }
    updatePixmap();
    update();
    emit documentChanged();
}

void CanvasWidget::setHistoryManager(std::shared_ptr<LibreCanvas::HistoryManager> manager)
{
    m_historyManager = manager;
    if (m_document && m_historyManager) {
        m_document->setHistoryManager(m_historyManager.get());
    }
}

void CanvasWidget::zoomIn()
{
    m_zoomLevel *= 1.2f;
    if (m_zoomLevel > 10.0f) m_zoomLevel = 10.0f;
    updatePixmap();
    update();
    emit zoomChanged(m_zoomLevel);
}

void CanvasWidget::zoomOut()
{
    m_zoomLevel /= 1.2f;
    if (m_zoomLevel < 0.1f) m_zoomLevel = 0.1f;
    updatePixmap();
    update();
    emit zoomChanged(m_zoomLevel);
}

void CanvasWidget::resetZoom()
{
    m_zoomLevel = 1.0f;
    m_panDelta = QPoint(0, 0);
    updatePixmap();
    update();
    emit zoomChanged(m_zoomLevel);
}

void CanvasWidget::fitToWindow()
{
    if (!m_document) return;
    
    QSize widgetSize = size();
    QSize imageSize = m_document->getSize();
    
    float scaleX = static_cast<float>(widgetSize.width()) / imageSize.width();
    float scaleY = static_cast<float>(widgetSize.height()) / imageSize.height();
    
    m_zoomLevel = qMin(scaleX, scaleY) * 0.95f; // 95% to leave some margin
    m_panDelta = QPoint(0, 0);
    updatePixmap();
    update();
    emit zoomChanged(m_zoomLevel);
}

void CanvasWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw checkerboard pattern for transparency
    QColor color1(60, 60, 60);
    QColor color2(50, 50, 50);
    int tileSize = 20;
    
    for (int y = 0; y < height(); y += tileSize) {
        for (int x = 0; x < width(); x += tileSize) {
            QColor color = ((x / tileSize) + (y / tileSize)) % 2 == 0 ? color1 : color2;
            painter.fillRect(x, y, tileSize, tileSize, color);
        }
    }
    
    // Draw document if available
    if (m_document && !m_pixmap.isNull()) {
        QSize scaledSize = m_pixmap.size();
        QPoint drawPos = (rect().center() - QRect(0, 0, scaledSize.width(), scaledSize.height()).center()) + m_panDelta;
        painter.drawPixmap(drawPos, m_pixmap);
        
        // Draw selection overlay
        drawSelection(painter);
    } else {
        // Draw placeholder text
        painter.setPen(Qt::gray);
        painter.setFont(QFont("Arial", 14));
        painter.drawText(rect(), Qt::AlignCenter, "No image loaded\n\nFile → New or File → Open");
    }
}

void CanvasWidget::drawSelection(QPainter& painter)
{
    if (!m_currentTool || !m_document) return;
    
    // Draw marquee selection
    if (m_currentTool->getType() == LibreCanvas::ToolType::MarqueeRect) {
        auto marqueeTool = std::dynamic_pointer_cast<LibreCanvas::MarqueeRectTool>(m_currentTool);
        if (marqueeTool && marqueeTool->hasSelection()) {
            QRect selection = marqueeTool->getSelection();
            QSize docSize = m_document->getSize();
            QSize scaledSize = docSize * m_zoomLevel;
            QPoint canvasCenter = rect().center() + m_panDelta;
            QPoint imageTopLeft = canvasCenter - QPoint(scaledSize.width() / 2, scaledSize.height() / 2);
            
            QRect scaledSelection(
                imageTopLeft.x() + selection.x() * m_zoomLevel,
                imageTopLeft.y() + selection.y() * m_zoomLevel,
                selection.width() * m_zoomLevel,
                selection.height() * m_zoomLevel
            );
            
            // Draw marching ants effect
            QPen pen(Qt::white, 1, Qt::DashLine);
            painter.setPen(pen);
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(scaledSelection);
        }
    }
    
    // Draw transform handles
    if (m_currentTool->getType() == LibreCanvas::ToolType::Transform) {
        auto transformTool = std::dynamic_pointer_cast<LibreCanvas::TransformTool>(m_currentTool);
        if (transformTool) {
            QRect bounds = transformTool->isTransforming() ? 
                          transformTool->getCurrentBounds() : 
                          transformTool->getOriginalBounds();
            
            if (!bounds.isEmpty()) {
                QSize docSize = m_document->getSize();
                QSize scaledSize = docSize * m_zoomLevel;
                QPoint canvasCenter = rect().center() + m_panDelta;
                QPoint imageTopLeft = canvasCenter - QPoint(scaledSize.width() / 2, scaledSize.height() / 2);
                
                QRect scaledBounds(
                    imageTopLeft.x() + bounds.x() * m_zoomLevel,
                    imageTopLeft.y() + bounds.y() * m_zoomLevel,
                    bounds.width() * m_zoomLevel,
                    bounds.height() * m_zoomLevel
                );
                
                // Draw bounding box
                QPen pen(Qt::blue, 2);
                painter.setPen(pen);
                painter.setBrush(QBrush(Qt::white));
                painter.drawRect(scaledBounds);
                
                // Draw corner handles
                const int handleSize = 8;
                QPoint corners[] = {
                    scaledBounds.topLeft(),
                    scaledBounds.topRight(),
                    scaledBounds.bottomLeft(),
                    scaledBounds.bottomRight()
                };
                for (const QPoint& corner : corners) {
                    QRect handleRect(corner.x() - handleSize/2, corner.y() - handleSize/2, handleSize, handleSize);
                    painter.drawRect(handleRect);
                }
                
                // Draw edge handles
                QPoint edges[] = {
                    QPoint(scaledBounds.center().x(), scaledBounds.top()),
                    QPoint(scaledBounds.center().x(), scaledBounds.bottom()),
                    QPoint(scaledBounds.left(), scaledBounds.center().y()),
                    QPoint(scaledBounds.right(), scaledBounds.center().y())
                };
                for (const QPoint& edge : edges) {
                    QRect handleRect(edge.x() - handleSize/2, edge.y() - handleSize/2, handleSize, handleSize);
                    painter.drawRect(handleRect);
                }
                
                // Draw rotation handle
                QPoint rotHandle = QPoint(scaledBounds.center().x(), scaledBounds.top() - 20);
                QRect rotHandleRect(rotHandle.x() - handleSize/2, rotHandle.y() - handleSize/2, handleSize, handleSize);
                painter.drawEllipse(rotHandleRect);
            }
        }
    }
}

void CanvasWidget::wheelEvent(QWheelEvent *event)
{
    if (!m_document) return;
    
    // Zoom with mouse wheel
    float delta = event->angleDelta().y() / 120.0f;
    if (delta > 0) {
        zoomIn();
    } else {
        zoomOut();
    }
}

void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_isPanning = true;
        m_panStart = event->pos();
        return;
    }
    
    // Handle tool
    if (m_currentTool && m_document && event->button() == Qt::LeftButton) {
        QPoint imagePos = canvasToImage(event->pos());
        m_currentTool->onMousePress(event, m_document, imagePos);
        // Save state before tool operation
        if (m_historyManager && m_document) {
            m_historyManager->pushState(m_document, "Tool Operation");
        }
        updatePixmap();
        update();
    }
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPanning) {
        QPoint delta = event->pos() - m_panStart;
        m_panDelta += delta;
        m_panStart = event->pos();
        update();
        return;
    }
    
    // Handle tool
    if (m_currentTool && m_document && event->buttons() & Qt::LeftButton) {
        QPoint imagePos = canvasToImage(event->pos());
        m_currentTool->onMouseMove(event, m_document, imagePos);
        updatePixmap();
        update();
    }
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_isPanning = false;
        return;
    }
    
    // Handle tool
    if (m_currentTool && m_document && event->button() == Qt::LeftButton) {
        QPoint imagePos = canvasToImage(event->pos());
        m_currentTool->onMouseRelease(event, m_document, imagePos);
        updatePixmap();
        update();
        emit imageChanged();
    }
}

void CanvasWidget::keyPressEvent(QKeyEvent *event)
{
    if (m_currentTool) {
        m_currentTool->onKeyPress(event);
    }
    QWidget::keyPressEvent(event);
}

void CanvasWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (m_currentTool) {
        m_currentTool->onKeyRelease(event);
    }
    QWidget::keyReleaseEvent(event);
}

void CanvasWidget::setTool(std::shared_ptr<LibreCanvas::Tool> tool)
{
    m_currentTool = tool;
    if (tool) {
        setCursor(tool->getCursor());
    }
}

void CanvasWidget::updatePixmap()
{
    if (!m_document) {
        m_pixmap = QPixmap();
        return;
    }
    
    QImage rendered = m_document->render();
    QSize scaledSize = rendered.size() * m_zoomLevel;
    m_pixmap = QPixmap::fromImage(rendered.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    update();
}

QImage CanvasWidget::getImage() const
{
    if (!m_document) {
        return QImage();
    }
    return m_document->render();
}

QPoint CanvasWidget::imageToCanvas(const QPoint &point) const
{
    if (!m_document) return point;
    
    // Convert image coordinates to canvas coordinates
    QSize docSize = m_document->getSize();
    QSize scaledSize = docSize * m_zoomLevel;
    QPoint canvasCenter = rect().center() + m_panDelta;
    QPoint imageTopLeft = canvasCenter - QPoint(scaledSize.width() / 2, scaledSize.height() / 2);
    return point - imageTopLeft;
}

QPoint CanvasWidget::canvasToImage(const QPoint &point) const
{
    if (!m_document) return point;
    
    // Convert canvas coordinates to image coordinates
    QSize docSize = m_document->getSize();
    QSize scaledSize = docSize * m_zoomLevel;
    QPoint canvasCenter = rect().center() + m_panDelta;
    QPoint imageTopLeft = canvasCenter - QPoint(scaledSize.width() / 2, scaledSize.height() / 2);
    QPoint relativePoint = point - imageTopLeft;
    return QPoint(relativePoint.x() / m_zoomLevel, relativePoint.y() / m_zoomLevel);
}

