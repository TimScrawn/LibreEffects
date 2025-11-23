#pragma once

#include "layer.h"
#include <QSize>
#include <QColor>
#include <vector>
#include <memory>

namespace LibreCanvas {

    class Document {
    public:
        Document(int width, int height, const QColor& backgroundColor = Qt::white);
        ~Document();

        // Document properties
        QSize getSize() const { return m_size; }
        void setSize(const QSize& size);
        QColor getBackgroundColor() const { return m_backgroundColor; }
        void setBackgroundColor(const QColor& color) { m_backgroundColor = color; }

        // Layer management
        void addLayer(std::shared_ptr<Layer> layer);
        void insertLayer(std::shared_ptr<Layer> layer, int index);
        void removeLayer(std::shared_ptr<Layer> layer);
        void moveLayer(int fromIndex, int toIndex);
        
        std::shared_ptr<Layer> getLayer(int index) const;
        std::shared_ptr<Layer> getActiveLayer() const { return m_activeLayer; }
        void setActiveLayer(std::shared_ptr<Layer> layer);
        void setActiveLayer(int index);

        int getLayerCount() const { return static_cast<int>(m_layers.size()); }
        const std::vector<std::shared_ptr<Layer>>& getLayers() const { return m_layers; }

        // Layer groups
        void addGroup(std::shared_ptr<LayerGroup> group);
        void removeGroup(std::shared_ptr<LayerGroup> group);

        // Rendering
        QImage render() const;
        QImage renderToImage(const QSize& size) const;

        // History/Undo (placeholder for future implementation)
        void saveState();
        bool canUndo() const { return false; } // TODO: implement
        bool canRedo() const { return false; } // TODO: implement
        void undo(); // TODO: implement
        void redo(); // TODO: implement

    private:
        QSize m_size;
        QColor m_backgroundColor;
        std::vector<std::shared_ptr<Layer>> m_layers;
        std::vector<std::shared_ptr<LayerGroup>> m_groups;
        std::shared_ptr<Layer> m_activeLayer;
    };

} // namespace LibreCanvas

