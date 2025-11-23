#include "lassotool.h"
#include <QPainterPath>

namespace LibreCanvas {

    void LassoTool::onMousePress(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (event->button() != Qt::LeftButton) return;
        
        m_isSelecting = true;
        m_polygon.clear();
        m_polygon << imagePos;
        m_selection = QRegion();
    }

    void LassoTool::onMouseMove(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (!m_isSelecting) return;
        
        m_polygon << imagePos;
    }

    void LassoTool::onMouseRelease(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (event->button() == Qt::LeftButton && m_isSelecting) {
            m_isSelecting = false;
            
            // Close the polygon
            if (m_polygon.size() > 2) {
                m_polygon << m_polygon.first();
                
                // Convert polygon to region
                QPainterPath path;
                path.addPolygon(m_polygon);
                m_selection = QRegion(path.toFillPolygon().toPolygon());
            }
        }
    }

    void MarqueeEllipseTool::onMousePress(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (event->button() != Qt::LeftButton) return;
        m_isSelecting = true;
        m_startPos = imagePos;
        m_selection = QRegion();
    }

    void MarqueeEllipseTool::onMouseMove(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (!m_isSelecting) return;
        
        QRect rect = QRect(m_startPos, imagePos).normalized();
        QPainterPath path;
        path.addEllipse(rect);
        m_selection = QRegion(path.toFillPolygon().toPolygon());
    }

    void MarqueeEllipseTool::onMouseRelease(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos)
    {
        if (event->button() == Qt::LeftButton) {
            m_isSelecting = false;
            QRect rect = QRect(m_startPos, imagePos).normalized();
            QPainterPath path;
            path.addEllipse(rect);
            m_selection = QRegion(path.toFillPolygon().toPolygon());
        }
    }

} // namespace LibreCanvas

