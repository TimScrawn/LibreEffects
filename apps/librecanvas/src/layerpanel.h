#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <memory>
#include "document.h"
#include "layer.h"

class LayerPanel : public QWidget
{
    Q_OBJECT

public:
    explicit LayerPanel(QWidget *parent = nullptr);
    void setDocument(std::shared_ptr<LibreCanvas::Document> document);
    void updateLayerList();
    void setActiveLayer(std::shared_ptr<LibreCanvas::Layer> layer);

signals:
    void layerSelected(std::shared_ptr<LibreCanvas::Layer> layer);
    void layerVisibilityChanged(std::shared_ptr<LibreCanvas::Layer> layer, bool visible);
    void layerOpacityChanged(std::shared_ptr<LibreCanvas::Layer> layer, float opacity);
    void layerBlendModeChanged(std::shared_ptr<LibreCanvas::Layer> layer, LibreCanvas::BlendMode mode);
    void createNewLayer();
    void deleteLayer(std::shared_ptr<LibreCanvas::Layer> layer);
    void duplicateLayer(std::shared_ptr<LibreCanvas::Layer> layer);
    void createGroup();

private slots:
    void onLayerItemClicked(QTreeWidgetItem *item, int column);
    void onLayerItemChanged(QTreeWidgetItem *item, int column);
    void onNewLayerClicked();
    void onDeleteLayerClicked();
    void onDuplicateLayerClicked();
    void onNewGroupClicked();
    void onOpacitySliderChanged(int value);
    void onBlendModeChanged(int index);

private:
    void setupUI();
    void updateLayerItem(QTreeWidgetItem *item, std::shared_ptr<LibreCanvas::Layer> layer);
    QTreeWidgetItem* createLayerItem(std::shared_ptr<LibreCanvas::Layer> layer);
    QTreeWidgetItem* createGroupItem(std::shared_ptr<LibreCanvas::LayerGroup> group);
    void populateLayers();

    QTreeWidget *m_layerTree;
    QPushButton *m_newLayerBtn;
    QPushButton *m_deleteLayerBtn;
    QPushButton *m_duplicateLayerBtn;
    QPushButton *m_newGroupBtn;
    QSlider *m_opacitySlider;
    QComboBox *m_blendModeCombo;
    QCheckBox *m_lockCheckbox;
    QLineEdit *m_layerNameEdit;

    std::shared_ptr<LibreCanvas::Document> m_document;
    std::shared_ptr<LibreCanvas::Layer> m_activeLayer;
    QTreeWidgetItem *m_activeItem;
};

