#include "document.h"
#include <QPainter>
#include <algorithm>

namespace LibreCanvas {

    Document::Document(int width, int height, const QColor& backgroundColor)
        : m_size(width, height)
        , m_backgroundColor(backgroundColor)
    {
        // Create initial background layer
        auto bgLayer = std::make_shared<Layer>("Background", width, height);
        bgLayer->getImage().fill(backgroundColor);
        bgLayer->setLocked(true);
        m_layers.push_back(bgLayer);
        m_activeLayer = bgLayer;
    }

    Document::~Document() = default;

    void Document::setSize(const QSize& size)
    {
        m_size = size;
        // Resize all layers
        for (auto& layer : m_layers) {
            QImage resized = layer->getImage().scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            layer->getImage() = resized;
        }
    }

    void Document::addLayer(std::shared_ptr<Layer> layer)
    {
        m_layers.push_back(layer);
        m_activeLayer = layer;
    }

    void Document::insertLayer(std::shared_ptr<Layer> layer, int index)
    {
        if (index < 0 || index > static_cast<int>(m_layers.size())) {
            m_layers.push_back(layer);
        } else {
            m_layers.insert(m_layers.begin() + index, layer);
        }
        m_activeLayer = layer;
    }

    void Document::removeLayer(std::shared_ptr<Layer> layer)
    {
        auto it = std::find(m_layers.begin(), m_layers.end(), layer);
        if (it != m_layers.end()) {
            m_layers.erase(it);
            // Set new active layer
            if (m_layers.empty()) {
                m_activeLayer = nullptr;
            } else {
                m_activeLayer = m_layers.back();
            }
        }
    }

    void Document::moveLayer(int fromIndex, int toIndex)
    {
        if (fromIndex < 0 || fromIndex >= static_cast<int>(m_layers.size()) ||
            toIndex < 0 || toIndex >= static_cast<int>(m_layers.size())) {
            return;
        }

        auto layer = m_layers[fromIndex];
        m_layers.erase(m_layers.begin() + fromIndex);
        m_layers.insert(m_layers.begin() + toIndex, layer);
    }

    std::shared_ptr<Layer> Document::getLayer(int index) const
    {
        if (index < 0 || index >= static_cast<int>(m_layers.size())) {
            return nullptr;
        }
        return m_layers[index];
    }

    void Document::setActiveLayer(std::shared_ptr<Layer> layer)
    {
        if (std::find(m_layers.begin(), m_layers.end(), layer) != m_layers.end()) {
            m_activeLayer = layer;
        }
    }

    void Document::setActiveLayer(int index)
    {
        auto layer = getLayer(index);
        if (layer) {
            m_activeLayer = layer;
        }
    }

    void Document::addGroup(std::shared_ptr<LayerGroup> group)
    {
        m_groups.push_back(group);
    }

    void Document::removeGroup(std::shared_ptr<LayerGroup> group)
    {
        m_groups.erase(
            std::remove(m_groups.begin(), m_groups.end(), group),
            m_groups.end()
        );
    }

    QImage Document::render() const
    {
        return renderToImage(m_size);
    }

    QImage Document::renderToImage(const QSize& size) const
    {
        QImage result(size, QImage::Format_ARGB32);
        result.fill(m_backgroundColor);

        QPainter painter(&result);
        painter.setRenderHint(QPainter::Antialiasing);

        // Render all layers from bottom to top
        for (const auto& layer : m_layers) {
            layer->render(painter, QRect(QPoint(0, 0), size));
        }

        painter.end();
        return result;
    }

    void Document::saveState()
    {
        // TODO: Implement history/undo system
    }

    void Document::undo()
    {
        // TODO: Implement undo
    }

    void Document::redo()
    {
        // TODO: Implement redo
    }

} // namespace LibreCanvas

