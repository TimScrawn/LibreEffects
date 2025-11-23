#pragma once

#include "tool.h"
#include <QPolygon>
#include <QRegion>

namespace LibreCanvas {

    class LassoTool : public Tool {
    public:
        LassoTool() : Tool(ToolType::Lasso) {}
        QString getName() const override { return "Lasso"; }
        QCursor getCursor() const override { return QCursor(Qt::CrossCursor); }

        void onMousePress(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;
        void onMouseMove(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;
        void onMouseRelease(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;

        QRegion getSelection() const { return m_selection; }
        bool hasSelection() const { return !m_selection.isEmpty(); }
        void clearSelection() { m_selection = QRegion(); m_polygon.clear(); }

    private:
        QPolygon m_polygon;
        QRegion m_selection;
        bool m_isSelecting = false;
    };

    class MarqueeEllipseTool : public Tool {
    public:
        MarqueeEllipseTool() : Tool(ToolType::MarqueeEllipse) {}
        QString getName() const override { return "Elliptical Marquee"; }
        QCursor getCursor() const override { return QCursor(Qt::CrossCursor); }

        void onMousePress(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;
        void onMouseMove(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;
        void onMouseRelease(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;

        QRegion getSelection() const { return m_selection; }
        bool hasSelection() const { return !m_selection.isEmpty(); }
        void clearSelection() { m_selection = QRegion(); }

    private:
        QPoint m_startPos;
        QRegion m_selection;
        bool m_isSelecting = false;
    };

} // namespace LibreCanvas

