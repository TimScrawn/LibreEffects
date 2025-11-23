#include "layer.h"
#include <QPainter>
#include <QPainterPath>
#include <algorithm>

namespace LibreCanvas {

    Layer::Layer(const QString& name, int width, int height)
        : m_name(name)
        , m_image(width, height, QImage::Format_ARGB32)
        , m_offset(0, 0)
        , m_visible(true)
        , m_locked(false)
        , m_opacity(1.0f)
        , m_blendMode(BlendMode::Normal)
    {
        m_image.fill(Qt::transparent);
    }

    Layer::Layer(const QString& name, const QImage& image)
        : m_name(name)
        , m_image(image.copy())
        , m_offset(0, 0)
        , m_visible(true)
        , m_locked(false)
        , m_opacity(1.0f)
        , m_blendMode(BlendMode::Normal)
    {
    }

    Layer::~Layer() = default;

    void Layer::createMask()
    {
        m_mask = QImage(m_image.size(), QImage::Format_Grayscale8);
        m_mask.fill(Qt::white); // White = visible, Black = hidden
    }

    void Layer::deleteMask()
    {
        m_mask = QImage();
    }

    void Layer::applyMask()
    {
        if (m_mask.isNull()) return;

        QImage masked = m_image.copy();
        for (int y = 0; y < masked.height(); ++y) {
            for (int x = 0; x < masked.width(); ++x) {
                QColor pixel = masked.pixelColor(x, y);
                int maskValue = qGray(m_mask.pixel(x, y));
                pixel.setAlpha(pixel.alpha() * maskValue / 255);
                masked.setPixelColor(x, y, pixel);
            }
        }
        m_image = masked;
        m_mask = QImage(); // Clear mask after applying
    }

    void Layer::render(QPainter& painter, const QRect& destRect) const
    {
        if (!m_visible || m_image.isNull()) return;

        painter.save();

        // Set opacity
        painter.setOpacity(m_opacity);

        // Apply blend mode
        applyBlendMode(painter);

        // Apply offset
        QRect sourceRect = m_image.rect();
        QRect targetRect = destRect.translated(m_offset);

        // Apply mask if present
        if (!m_mask.isNull()) {
            QPainterPath clipPath;
            QImage maskScaled = m_mask.scaled(targetRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            // Create clipping path from mask
            // For now, simple alpha channel masking
            painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            painter.drawImage(targetRect, maskScaled);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        }

        // Draw the layer image
        painter.drawImage(targetRect, m_image, sourceRect);

        painter.restore();
    }

    void Layer::applyBlendMode(QPainter& painter) const
    {
        switch (m_blendMode) {
            case BlendMode::Normal:
                painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
                break;
            case BlendMode::Multiply:
                painter.setCompositionMode(QPainter::CompositionMode_Multiply);
                break;
            case BlendMode::Screen:
                painter.setCompositionMode(QPainter::CompositionMode_Screen);
                break;
            case BlendMode::Overlay:
                painter.setCompositionMode(QPainter::CompositionMode_Overlay);
                break;
            case BlendMode::SoftLight:
                painter.setCompositionMode(QPainter::CompositionMode_SoftLight);
                break;
            case BlendMode::HardLight:
                painter.setCompositionMode(QPainter::CompositionMode_HardLight);
                break;
            case BlendMode::ColorDodge:
                painter.setCompositionMode(QPainter::CompositionMode_ColorDodge);
                break;
            case BlendMode::ColorBurn:
                painter.setCompositionMode(QPainter::CompositionMode_ColorBurn);
                break;
            case BlendMode::Darken:
                painter.setCompositionMode(QPainter::CompositionMode_Darken);
                break;
            case BlendMode::Lighten:
                painter.setCompositionMode(QPainter::CompositionMode_Lighten);
                break;
            case BlendMode::Difference:
                painter.setCompositionMode(QPainter::CompositionMode_Difference);
                break;
            case BlendMode::Exclusion:
                painter.setCompositionMode(QPainter::CompositionMode_Exclusion);
                break;
        }
    }

    // LayerGroup implementation
    LayerGroup::LayerGroup(const QString& name)
        : m_name(name)
        , m_visible(true)
        , m_expanded(true)
    {
    }

    LayerGroup::~LayerGroup() = default;

    void LayerGroup::addLayer(std::shared_ptr<Layer> layer)
    {
        m_layers.push_back(layer);
    }

    void LayerGroup::removeLayer(std::shared_ptr<Layer> layer)
    {
        m_layers.erase(
            std::remove(m_layers.begin(), m_layers.end(), layer),
            m_layers.end()
        );
    }

    void LayerGroup::addGroup(std::shared_ptr<LayerGroup> group)
    {
        m_groups.push_back(group);
    }

    void LayerGroup::removeGroup(std::shared_ptr<LayerGroup> group)
    {
        m_groups.erase(
            std::remove(m_groups.begin(), m_groups.end(), group),
            m_groups.end()
        );
    }

} // namespace LibreCanvas

