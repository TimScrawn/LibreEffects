#include "canvaswidget.h"
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QFileInfo>
#include <QMessageBox>

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
    auto layer = std::make_shared<LibreCanvas::Layer>("Layer 1", loadedImage);
    m_document->addLayer(layer);
    
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
    updatePixmap();
    update();
    emit documentChanged();
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
    } else {
        // Draw placeholder text
        painter.setPen(Qt::gray);
        painter.setFont(QFont("Arial", 14));
        painter.drawText(rect(), Qt::AlignCenter, "No image loaded\n\nFile → New or File → Open");
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
    }
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPanning) {
        QPoint delta = event->pos() - m_panStart;
        m_panDelta += delta;
        m_panStart = event->pos();
        update();
    }
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton || event->button() == Qt::LeftButton) {
        m_isPanning = false;
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

