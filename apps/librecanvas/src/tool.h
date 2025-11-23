#pragma once

#include <QObject>
#include <QPoint>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <memory>
#include "document.h"
#include "layer.h"

namespace LibreCanvas {

    enum class ToolType {
        Brush,
        Eraser,
        MarqueeRect,
        MarqueeEllipse,
        Lasso,
        MagicWand,
        Move,
        CloneStamp,
        HealingBrush,
        Gradient,
        Text,
        ShapeRect,
        ShapeEllipse,
        Pen,
        Eyedropper,
        Zoom,
        Hand
    };

    class Tool : public QObject {
        Q_OBJECT

    public:
        Tool(ToolType type) : m_type(type) {}
        virtual ~Tool() = default;

        ToolType getType() const { return m_type; }
        virtual QString getName() const = 0;
        virtual QCursor getCursor() const { return Qt::ArrowCursor; }

        virtual void onMousePress(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) {}
        virtual void onMouseMove(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) {}
        virtual void onMouseRelease(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) {}
        virtual void onKeyPress(QKeyEvent *event) {}
        virtual void onKeyRelease(QKeyEvent *event) {}

    protected:
        ToolType m_type;
    };

    class BrushTool : public Tool {
    public:
        BrushTool() : Tool(ToolType::Brush) {}
        QString getName() const override { return "Brush"; }
        QCursor getCursor() const override { return QCursor(Qt::CrossCursor); }

        void onMousePress(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;
        void onMouseMove(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;
        void onMouseRelease(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;

        void setSize(int size) { m_size = size; }
        void setHardness(float hardness) { m_hardness = qBound(0.0f, hardness, 1.0f); }
        void setOpacity(float opacity) { m_opacity = qBound(0.0f, opacity, 1.0f); }
        void setFlow(float flow) { m_flow = qBound(0.0f, flow, 1.0f); }
        void setColor(const QColor& color) { m_color = color; }

    private:
        void drawBrush(QPainter& painter, const QPoint& pos, int size, float hardness, const QColor& color, float opacity);
        
        int m_size = 20;
        float m_hardness = 1.0f;
        float m_opacity = 1.0f;
        float m_flow = 1.0f;
        QColor m_color = Qt::black;
        QPoint m_lastPos;
        bool m_isDrawing = false;
    };

    class EraserTool : public Tool {
    public:
        EraserTool() : Tool(ToolType::Eraser) {}
        QString getName() const override { return "Eraser"; }
        QCursor getCursor() const override { return QCursor(Qt::CrossCursor); }

        void onMousePress(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;
        void onMouseMove(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;
        void onMouseRelease(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;

        void setSize(int size) { m_size = size; }
        void setHardness(float hardness) { m_hardness = qBound(0.0f, hardness, 1.0f); }
        void setOpacity(float opacity) { m_opacity = qBound(0.0f, opacity, 1.0f); }

    private:
        void eraseBrush(QPainter& painter, const QPoint& pos, int size, float hardness, float opacity);
        
        int m_size = 20;
        float m_hardness = 1.0f;
        float m_opacity = 1.0f;
        QPoint m_lastPos;
        bool m_isErasing = false;
    };

    class MarqueeRectTool : public Tool {
    public:
        MarqueeRectTool() : Tool(ToolType::MarqueeRect) {}
        QString getName() const override { return "Rectangular Marquee"; }
        QCursor getCursor() const override { return QCursor(Qt::CrossCursor); }

        void onMousePress(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;
        void onMouseMove(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;
        void onMouseRelease(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;

        QRect getSelection() const { return m_selection; }
        bool hasSelection() const { return !m_selection.isEmpty(); }
        void clearSelection() { m_selection = QRect(); }

    private:
        QPoint m_startPos;
        QRect m_selection;
        bool m_isSelecting = false;
    };

    class MagicWandTool : public Tool {
    public:
        MagicWandTool() : Tool(ToolType::MagicWand) {}
        QString getName() const override { return "Magic Wand"; }
        QCursor getCursor() const override { return QCursor(Qt::CrossCursor); }

        void onMousePress(QMouseEvent *event, std::shared_ptr<Document> doc, const QPoint& imagePos) override;

        void setTolerance(int tolerance) { m_tolerance = qBound(0, tolerance, 255); }
        int getTolerance() const { return m_tolerance; }

        QRegion getSelection() const { return m_selection; }
        bool hasSelection() const { return !m_selection.isEmpty(); }
        void clearSelection() { m_selection = QRegion(); }

    private:
        QRegion floodFill(const QImage& image, const QPoint& start, int tolerance);
        
        int m_tolerance = 32;
        QRegion m_selection;
    };

} // namespace LibreCanvas

