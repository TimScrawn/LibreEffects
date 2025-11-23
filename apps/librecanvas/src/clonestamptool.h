#pragma once

#include "tool.h"
#include <QPoint>

namespace LibreCanvas {

    class CloneStampTool : public Tool {
    public:
        CloneStampTool() : Tool(ToolType::CloneStamp) {}
        QString getName() const override { return "Clone Stamp"; }
        QCursor getCursor() const override { return QCursor(Qt::CrossCursor); }

        void onMousePress(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;
        void onMouseMove(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;
        void onMouseRelease(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;

        void setSize(int size) { m_size = size; }
        void setHardness(float hardness) { m_hardness = qBound(0.0f, hardness, 1.0f); }
        void setOpacity(float opacity) { m_opacity = qBound(0.0f, opacity, 1.0f); }

    private:
        void cloneBrush(QPainter& painter, const QPoint& sourcePos, const QPoint& destPos, int size, float hardness, float opacity);
        
        int m_size = 20;
        float m_hardness = 1.0f;
        float m_opacity = 1.0f;
        QPoint m_sourcePoint;
        QPoint m_lastDestPos;
        bool m_sourceSet = false;
        bool m_isCloning = false;
    };

} // namespace LibreCanvas

