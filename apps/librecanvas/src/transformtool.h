#pragma once

#include "tool.h"
#include <QPoint>
#include <QRect>

namespace LibreCanvas {

    enum class TransformMode {
        None,
        Move,
        ScaleTopLeft,
        ScaleTopRight,
        ScaleBottomLeft,
        ScaleBottomRight,
        ScaleTop,
        ScaleBottom,
        ScaleLeft,
        ScaleRight,
        Rotate
    };

    class TransformTool : public Tool {
    public:
        TransformTool();
        ~TransformTool() override = default;

        QString getName() const override { return "Transform"; }
        QCursor getCursor() const override;

        void onMousePress(QMouseEvent* event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;
        void onMouseMove(QMouseEvent* event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;
        void onMouseRelease(QMouseEvent* event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;
        void onKeyPress(QKeyEvent* event) override;

        // State access for drawing handles
        bool isTransforming() const { return m_isTransforming; }
        QRect getCurrentBounds() const { return m_currentBounds; }
        QRect getOriginalBounds() const { return m_originalBounds; }
        TransformMode getMode() const { return m_mode; }

    private:
        bool m_isTransforming;
        QPoint m_startPos;
        QRect m_originalBounds;
        QRect m_currentBounds;
        TransformMode m_mode;
        QPoint m_rotationCenter;
        float m_startAngle;

        QRect getSelectionBounds() const;
        TransformMode getHandleAt(const QPoint& pos, const QRect& bounds) const;
        void drawHandles(QPainter& painter, const QRect& bounds);
        QPoint getHandlePosition(TransformMode mode, const QRect& bounds) const;
        void applyTransform(QPainter& painter, const QRect& bounds);
    };

} // namespace LibreCanvas

