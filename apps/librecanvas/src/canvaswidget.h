#pragma once

#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPoint>

class CanvasWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget *parent = nullptr);
    
    // Image operations
    bool loadImage(const QString &filePath);
    bool saveImage(const QString &filePath);
    void newImage(int width, int height);
    
    // Canvas operations
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void fitToWindow();
    
    // Getters
    QImage getImage() const { return m_image; }
    float getZoomLevel() const { return m_zoomLevel; }
    bool hasImage() const { return !m_image.isNull(); }

signals:
    void imageChanged();
    void zoomChanged(float zoom);

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QImage m_image;
    QPixmap m_pixmap;
    float m_zoomLevel;
    QPoint m_panStart;
    QPoint m_panDelta;
    bool m_isPanning;
    
    void updatePixmap();
    QPoint imageToCanvas(const QPoint &point) const;
    QPoint canvasToImage(const QPoint &point) const;
};

