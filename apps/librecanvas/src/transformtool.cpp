#include "transformtool.h"
#include "document.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QtMath>

namespace LibreCanvas {

    TransformTool::TransformTool()
        : Tool(ToolType::Move)
        , m_isTransforming(false)
        , m_mode(TransformMode::None)
        , m_startAngle(0.0f)
    {
    }

    QCursor TransformTool::getCursor() const {
        if (m_isTransforming) {
            return QCursor(Qt::SizeAllCursor);
        }
        return QCursor(Qt::CrossCursor);
    }

    void TransformTool::onMousePress(QMouseEvent* event, std::shared_ptr<Document> doc, const QPoint& imagePos) {
        if (event->button() != Qt::LeftButton || !doc) return;

        QRect bounds = getSelectionBounds();
        if (bounds.isEmpty()) {
            // No selection, transform the active layer
            if (doc->getActiveLayer()) {
                auto layer = doc->getActiveLayer();
                QSize layerSize = layer->getSize();
                QPoint offset = layer->getOffset();
                bounds = QRect(offset, layerSize);
            } else {
                return;
            }
        }

        m_startPos = imagePos;
        m_originalBounds = bounds;
        m_currentBounds = bounds;
        m_rotationCenter = bounds.center();
        m_mode = getHandleAt(imagePos, bounds);

        if (m_mode == TransformMode::None) {
            // Check if clicking inside bounds for move
            if (bounds.contains(imagePos)) {
                m_mode = TransformMode::Move;
            }
        }

        if (m_mode != TransformMode::None) {
            m_isTransforming = true;
            if (m_mode == TransformMode::Rotate) {
                QPoint delta = imagePos - m_rotationCenter;
                m_startAngle = qAtan2(delta.y(), delta.x()) * 180.0 / M_PI;
            }
        }
    }

    void TransformTool::onMouseMove(QMouseEvent* event, std::shared_ptr<Document> doc, const QPoint& imagePos) {
        if (!m_isTransforming || m_mode == TransformMode::None) return;

        QPoint delta = imagePos - m_startPos;

        switch (m_mode) {
            case TransformMode::Move: {
                m_currentBounds = m_originalBounds.translated(delta);
                break;
            }
            case TransformMode::ScaleTopLeft: {
                m_currentBounds.setTopLeft(m_originalBounds.topLeft() + delta);
                break;
            }
            case TransformMode::ScaleTopRight: {
                m_currentBounds.setTopRight(m_originalBounds.topRight() + delta);
                break;
            }
            case TransformMode::ScaleBottomLeft: {
                m_currentBounds.setBottomLeft(m_originalBounds.bottomLeft() + delta);
                break;
            }
            case TransformMode::ScaleBottomRight: {
                m_currentBounds.setBottomRight(m_originalBounds.bottomRight() + delta);
                break;
            }
            case TransformMode::ScaleTop: {
                m_currentBounds.setTop(m_originalBounds.top() + delta.y());
                break;
            }
            case TransformMode::ScaleBottom: {
                m_currentBounds.setBottom(m_originalBounds.bottom() + delta.y());
                break;
            }
            case TransformMode::ScaleLeft: {
                m_currentBounds.setLeft(m_originalBounds.left() + delta.x());
                break;
            }
            case TransformMode::ScaleRight: {
                m_currentBounds.setRight(m_originalBounds.right() + delta.x());
                break;
            }
            case TransformMode::Rotate: {
                QPoint deltaFromCenter = imagePos - m_rotationCenter;
                float currentAngle = qAtan2(deltaFromCenter.y(), deltaFromCenter.x()) * 180.0 / M_PI;
                float rotation = currentAngle - m_startAngle;
                // For now, just update the angle (full rotation transform would require matrix)
                m_startAngle = currentAngle;
                break;
            }
            default:
                break;
        }

        // Normalize bounds
        if (m_currentBounds.width() < 0) {
            int left = m_currentBounds.left();
            m_currentBounds.setLeft(m_currentBounds.right());
            m_currentBounds.setRight(left);
        }
        if (m_currentBounds.height() < 0) {
            int top = m_currentBounds.top();
            m_currentBounds.setTop(m_currentBounds.bottom());
            m_currentBounds.setBottom(top);
        }
    }

    void TransformTool::onMouseRelease(QMouseEvent* event, std::shared_ptr<Document> doc, const QPoint& imagePos) {
        if (event->button() != Qt::LeftButton || !doc) return;

        if (m_isTransforming && doc->getActiveLayer()) {
            auto layer = doc->getActiveLayer();
            
            if (m_mode == TransformMode::Move) {
                QPoint offset = m_currentBounds.topLeft() - m_originalBounds.topLeft();
                layer->setOffset(layer->getOffset() + offset);
            } else if (m_mode >= TransformMode::ScaleTopLeft && m_mode <= TransformMode::ScaleRight) {
                // Scale the layer image
                QImage originalImage = layer->getImage();
                QSize newSize = m_currentBounds.size();
                if (newSize.width() > 0 && newSize.height() > 0) {
                    QImage scaled = originalImage.scaled(newSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                    layer->getImage() = scaled;
                    layer->setOffset(m_currentBounds.topLeft());
                }
            }

            doc->saveState("Transform");
        }

        m_isTransforming = false;
        m_mode = TransformMode::None;
    }

    void TransformTool::onKeyPress(QKeyEvent* event) {
        if (event->key() == Qt::Key_Escape && m_isTransforming) {
            m_isTransforming = false;
            m_mode = TransformMode::None;
            m_currentBounds = m_originalBounds;
        }
    }

    QRect TransformTool::getSelectionBounds() const {
        // TODO: Get selection from document/selection manager
        return QRect();
    }

    TransformMode TransformTool::getHandleAt(const QPoint& pos, const QRect& bounds) const {
        const int handleSize = 8;
        const int handleRadius = handleSize / 2;

        // Check corner handles
        QPoint corners[] = {
            bounds.topLeft(),
            bounds.topRight(),
            bounds.bottomLeft(),
            bounds.bottomRight()
        };
        TransformMode cornerModes[] = {
            TransformMode::ScaleTopLeft,
            TransformMode::ScaleTopRight,
            TransformMode::ScaleBottomLeft,
            TransformMode::ScaleBottomRight
        };

        for (int i = 0; i < 4; ++i) {
            QRect handleRect(corners[i].x() - handleRadius, corners[i].y() - handleRadius,
                            handleSize, handleSize);
            if (handleRect.contains(pos)) {
                return cornerModes[i];
            }
        }

        // Check edge handles
        QPoint edges[] = {
            QPoint(bounds.center().x(), bounds.top()),
            QPoint(bounds.center().x(), bounds.bottom()),
            QPoint(bounds.left(), bounds.center().y()),
            QPoint(bounds.right(), bounds.center().y())
        };
        TransformMode edgeModes[] = {
            TransformMode::ScaleTop,
            TransformMode::ScaleBottom,
            TransformMode::ScaleLeft,
            TransformMode::ScaleRight
        };

        for (int i = 0; i < 4; ++i) {
            QRect handleRect(edges[i].x() - handleRadius, edges[i].y() - handleRadius,
                            handleSize, handleSize);
            if (handleRect.contains(pos)) {
                return edgeModes[i];
            }
        }

        // Check rotation handle (top center, offset upward)
        QPoint rotHandle = QPoint(bounds.center().x(), bounds.top() - 20);
        QRect rotHandleRect(rotHandle.x() - handleRadius, rotHandle.y() - handleRadius,
                           handleSize, handleSize);
        if (rotHandleRect.contains(pos)) {
            return TransformMode::Rotate;
        }

        return TransformMode::None;
    }

    void TransformTool::drawHandles(QPainter& painter, const QRect& bounds) {
        if (bounds.isEmpty()) return;

        painter.save();
        QPen pen(Qt::blue, 2);
        painter.setPen(pen);
        painter.setBrush(QBrush(Qt::white));

        // Draw bounding box
        painter.drawRect(bounds);

        // Draw corner handles
        QPoint corners[] = {
            bounds.topLeft(),
            bounds.topRight(),
            bounds.bottomLeft(),
            bounds.bottomRight()
        };
        for (const QPoint& corner : corners) {
            QRect handleRect(corner.x() - 4, corner.y() - 4, 8, 8);
            painter.drawRect(handleRect);
        }

        // Draw edge handles
        QPoint edges[] = {
            QPoint(bounds.center().x(), bounds.top()),
            QPoint(bounds.center().x(), bounds.bottom()),
            QPoint(bounds.left(), bounds.center().y()),
            QPoint(bounds.right(), bounds.center().y())
        };
        for (const QPoint& edge : edges) {
            QRect handleRect(edge.x() - 4, edge.y() - 4, 8, 8);
            painter.drawRect(handleRect);
        }

        // Draw rotation handle
        QPoint rotHandle = QPoint(bounds.center().x(), bounds.top() - 20);
        QRect rotHandleRect(rotHandle.x() - 4, rotHandle.y() - 4, 8, 8);
        painter.drawEllipse(rotHandleRect);

        painter.restore();
    }

    QPoint TransformTool::getHandlePosition(TransformMode mode, const QRect& bounds) const {
        switch (mode) {
            case TransformMode::ScaleTopLeft: return bounds.topLeft();
            case TransformMode::ScaleTopRight: return bounds.topRight();
            case TransformMode::ScaleBottomLeft: return bounds.bottomLeft();
            case TransformMode::ScaleBottomRight: return bounds.bottomRight();
            case TransformMode::ScaleTop: return QPoint(bounds.center().x(), bounds.top());
            case TransformMode::ScaleBottom: return QPoint(bounds.center().x(), bounds.bottom());
            case TransformMode::ScaleLeft: return QPoint(bounds.left(), bounds.center().y());
            case TransformMode::ScaleRight: return QPoint(bounds.right(), bounds.center().y());
            case TransformMode::Rotate: return QPoint(bounds.center().x(), bounds.top() - 20);
            default: return bounds.center();
        }
    }

    void TransformTool::applyTransform(QPainter& painter, const QRect& bounds) {
        // This would apply the transform matrix to the painter
        // For now, handled in onMouseRelease
    }

} // namespace LibreCanvas

