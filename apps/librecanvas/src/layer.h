#pragma once

#include <QImage>
#include <QString>
#include <QPainter>
#include <memory>

namespace LibreCanvas {

    enum class BlendMode {
        Normal,
        Multiply,
        Screen,
        Overlay,
        SoftLight,
        HardLight,
        ColorDodge,
        ColorBurn,
        Darken,
        Lighten,
        Difference,
        Exclusion
    };

    class Layer {
    public:
        Layer(const QString& name, int width, int height);
        Layer(const QString& name, const QImage& image);
        ~Layer();

        // Layer properties
        QString getName() const { return m_name; }
        void setName(const QString& name) { m_name = name; }

        bool isVisible() const { return m_visible; }
        void setVisible(bool visible) { m_visible = visible; }

        bool isLocked() const { return m_locked; }
        void setLocked(bool locked) { m_locked = locked; }

        float getOpacity() const { return m_opacity; }
        void setOpacity(float opacity) { m_opacity = qBound(0.0f, opacity, 1.0f); }

        BlendMode getBlendMode() const { return m_blendMode; }
        void setBlendMode(BlendMode mode) { m_blendMode = mode; }

        // Image access
        QImage& getImage() { return m_image; }
        const QImage& getImage() const { return m_image; }

        QSize getSize() const { return m_image.size(); }

        // Layer mask
        bool hasMask() const { return !m_mask.isNull(); }
        QImage& getMask() { return m_mask; }
        const QImage& getMask() const { return m_mask; }
        void createMask();
        void deleteMask();
        void applyMask();

        // Rendering
        void render(QPainter& painter, const QRect& destRect) const;

        // Transform
        void setOffset(const QPoint& offset) { m_offset = offset; }
        QPoint getOffset() const { return m_offset; }

    private:
        QString m_name;
        QImage m_image;
        QImage m_mask;
        QPoint m_offset;
        
        bool m_visible;
        bool m_locked;
        float m_opacity;
        BlendMode m_blendMode;

        void applyBlendMode(QPainter& painter) const;
    };

    class LayerGroup {
    public:
        LayerGroup(const QString& name);
        ~LayerGroup();

        QString getName() const { return m_name; }
        void setName(const QString& name) { m_name = name; }

        bool isVisible() const { return m_visible; }
        void setVisible(bool visible) { m_visible = visible; }

        bool isExpanded() const { return m_expanded; }
        void setExpanded(bool expanded) { m_expanded = expanded; }

        void addLayer(std::shared_ptr<Layer> layer);
        void removeLayer(std::shared_ptr<Layer> layer);
        void addGroup(std::shared_ptr<LayerGroup> group);
        void removeGroup(std::shared_ptr<LayerGroup> group);

        const std::vector<std::shared_ptr<Layer>>& getLayers() const { return m_layers; }
        const std::vector<std::shared_ptr<LayerGroup>>& getGroups() const { return m_groups; }

    private:
        QString m_name;
        std::vector<std::shared_ptr<Layer>> m_layers;
        std::vector<std::shared_ptr<LayerGroup>> m_groups;
        bool m_visible;
        bool m_expanded;
    };

} // namespace LibreCanvas

