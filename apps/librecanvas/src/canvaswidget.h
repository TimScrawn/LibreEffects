#pragma once

#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPoint>
#include <memory>
#include "document.h"
#include "tool.h"

class CanvasWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget *parent = nullptr);
    
    // Document operations
    bool loadImage(const QString &filePath);
    bool saveImage(const QString &filePath);
    void newImage(int width, int height);
    
    // Document access
    std::shared_ptr<LibreCanvas::Document> getDocument() { return m_document; }
    void setDocument(std::shared_ptr<LibreCanvas::Document> document);
    
    // History
    void setHistoryManager(std::shared_ptr<LibreCanvas::HistoryManager> manager);
    
    // Canvas operations
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void fitToWindow();
    void updatePixmap();
    
    // Getters
    QImage getImage() const;
    float getZoomLevel() const { return m_zoomLevel; }
    bool hasImage() const { return m_document != nullptr; }

    // Tool management
    void setTool(std::shared_ptr<LibreCanvas::Tool> tool);
    std::shared_ptr<LibreCanvas::Tool> getCurrentTool() const { return m_currentTool; }

signals:
    void imageChanged();
    void zoomChanged(float zoom);
    void documentChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    std::shared_ptr<LibreCanvas::Document> m_document;
    std::shared_ptr<LibreCanvas::Tool> m_currentTool;
    std::shared_ptr<LibreCanvas::HistoryManager> m_historyManager;
    QPixmap m_pixmap;
    float m_zoomLevel;
    QPoint m_panStart;
    QPoint m_panDelta;
    bool m_isPanning;
    
    void updatePixmap();
    QPoint imageToCanvas(const QPoint &point) const;
    QPoint canvasToImage(const QPoint &point) const;
    void drawSelection(QPainter& painter);
};

