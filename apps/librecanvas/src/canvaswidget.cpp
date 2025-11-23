#include "canvaswidget.h"
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QFileInfo>
#include <QMessageBox>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent)
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
    
    m_image = loadedImage;
    m_zoomLevel = 1.0f;
    m_panDelta = QPoint(0, 0);
    updatePixmap();
    update();
    
    emit imageChanged();
    return true;
}

bool CanvasWidget::saveImage(const QString &filePath)
{
    if (m_image.isNull()) {
        QMessageBox::warning(this, "Save Error", "No image to save.");
        return false;
    }
    
    QString format = QFileInfo(filePath).suffix().toUpper();
    if (format.isEmpty()) {
        format = "PNG";
    }
    
    if (!m_image.save(filePath, format.toLatin1().constData())) {
        QMessageBox::warning(this, "Save Error", 
            QString("Failed to save image:\n%1").arg(filePath));
        return false;
    }
    
    return true;
}

void CanvasWidget::newImage(int width, int height)
{
    m_image = QImage(width, height, QImage::Format_ARGB32);
    m_image.fill(Qt::white);
    m_zoomLevel = 1.0f;
    m_panDelta = QPoint(0, 0);
    updatePixmap();
    update();
    
    emit imageChanged();
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
    if (m_image.isNull()) return;
    
    QSize widgetSize = size();
    QSize imageSize = m_image.size();
    
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
    
    // Draw image if available
    if (!m_pixmap.isNull()) {
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
    if (m_image.isNull()) return;
    
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
    if (event->button() == Qt::MiddleButton || 
        (event->button() == Qt::LeftButton && event->modifiers() & Qt::Space)) {
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
    if (m_image.isNull()) {
        m_pixmap = QPixmap();
        return;
    }
    
    QSize scaledSize = m_image.size() * m_zoomLevel;
    m_pixmap = QPixmap::fromImage(m_image.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

QPoint CanvasWidget::imageToCanvas(const QPoint &point) const
{
    // Convert image coordinates to canvas coordinates
    QSize scaledSize = m_image.size() * m_zoomLevel;
    QPoint canvasCenter = rect().center() + m_panDelta;
    QPoint imageTopLeft = canvasCenter - QPoint(scaledSize.width() / 2, scaledSize.height() / 2);
    return point - imageTopLeft;
}

QPoint CanvasWidget::canvasToImage(const QPoint &point) const
{
    // Convert canvas coordinates to image coordinates
    QSize scaledSize = m_image.size() * m_zoomLevel;
    QPoint canvasCenter = rect().center() + m_panDelta;
    QPoint imageTopLeft = canvasCenter - QPoint(scaledSize.width() / 2, scaledSize.height() / 2);
    QPoint relativePoint = point - imageTopLeft;
    return QPoint(relativePoint.x() / m_zoomLevel, relativePoint.y() / m_zoomLevel);
}

