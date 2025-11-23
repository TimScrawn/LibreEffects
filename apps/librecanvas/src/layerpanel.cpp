#include "layerpanel.h"
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>

LayerPanel::LayerPanel(QWidget *parent)
    : QWidget(parent)
    , m_activeLayer(nullptr)
    , m_activeItem(nullptr)
{
    setupUI();
}

void LayerPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // Layer tree
    m_layerTree = new QTreeWidget(this);
    m_layerTree->setHeaderLabel("Layers");
    m_layerTree->setRootIsDecorated(true);
    m_layerTree->setAlternatingRowColors(true);
    m_layerTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_layerTree->setDragDropMode(QAbstractItemView::InternalMove);
    m_layerTree->setDefaultDropAction(Qt::MoveAction);
    connect(m_layerTree, &QTreeWidget::itemClicked, this, &LayerPanel::onLayerItemClicked);
    connect(m_layerTree, &QTreeWidget::itemChanged, this, &LayerPanel::onLayerItemChanged);
    mainLayout->addWidget(m_layerTree);

    // Button row
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(2);

    m_newLayerBtn = new QPushButton("+", this);
    m_newLayerBtn->setToolTip("New Layer");
    m_newLayerBtn->setMaximumWidth(30);
    connect(m_newLayerBtn, &QPushButton::clicked, this, &LayerPanel::onNewLayerClicked);
    buttonLayout->addWidget(m_newLayerBtn);

    m_deleteLayerBtn = new QPushButton("−", this);
    m_deleteLayerBtn->setToolTip("Delete Layer");
    m_deleteLayerBtn->setMaximumWidth(30);
    connect(m_deleteLayerBtn, &QPushButton::clicked, this, &LayerPanel::onDeleteLayerClicked);
    buttonLayout->addWidget(m_deleteLayerBtn);

    m_duplicateLayerBtn = new QPushButton("⧉", this);
    m_duplicateLayerBtn->setToolTip("Duplicate Layer");
    m_duplicateLayerBtn->setMaximumWidth(30);
    connect(m_duplicateLayerBtn, &QPushButton::clicked, this, &LayerPanel::onDuplicateLayerClicked);
    buttonLayout->addWidget(m_duplicateLayerBtn);

    m_newGroupBtn = new QPushButton("📁", this);
    m_newGroupBtn->setToolTip("New Group");
    m_newGroupBtn->setMaximumWidth(30);
    connect(m_newGroupBtn, &QPushButton::clicked, this, &LayerPanel::onNewGroupClicked);
    buttonLayout->addWidget(m_newGroupBtn);

    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // Layer properties panel
    QVBoxLayout *propsLayout = new QVBoxLayout();
    
    // Layer name
    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->addWidget(new QLabel("Name:", this));
    m_layerNameEdit = new QLineEdit(this);
    connect(m_layerNameEdit, &QLineEdit::editingFinished, this, [this]() {
        if (m_activeLayer) {
            m_activeLayer->setName(m_layerNameEdit->text());
            if (m_activeItem) {
                m_activeItem->setText(0, m_layerNameEdit->text());
            }
        }
    });
    nameLayout->addWidget(m_layerNameEdit);
    propsLayout->addLayout(nameLayout);

    // Opacity
    QHBoxLayout *opacityLayout = new QHBoxLayout();
    opacityLayout->addWidget(new QLabel("Opacity:", this));
    m_opacitySlider = new QSlider(Qt::Horizontal, this);
    m_opacitySlider->setRange(0, 100);
    m_opacitySlider->setValue(100);
    connect(m_opacitySlider, &QSlider::valueChanged, this, &LayerPanel::onOpacitySliderChanged);
    opacityLayout->addWidget(m_opacitySlider);
    QLabel *opacityLabel = new QLabel("100%", this);
    opacityLabel->setMinimumWidth(40);
    connect(m_opacitySlider, &QSlider::valueChanged, opacityLabel, [opacityLabel](int value) {
        opacityLabel->setText(QString("%1%").arg(value));
    });
    opacityLayout->addWidget(opacityLabel);
    propsLayout->addLayout(opacityLayout);

    // Blend mode
    QHBoxLayout *blendLayout = new QHBoxLayout();
    blendLayout->addWidget(new QLabel("Blend:", this));
    m_blendModeCombo = new QComboBox(this);
    m_blendModeCombo->addItems({
        "Normal", "Multiply", "Screen", "Overlay", "Soft Light",
        "Hard Light", "Color Dodge", "Color Burn", "Darken",
        "Lighten", "Difference", "Exclusion"
    });
    connect(m_blendModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LayerPanel::onBlendModeChanged);
    blendLayout->addWidget(m_blendModeCombo);
    propsLayout->addLayout(blendLayout);

    // Lock checkbox
    m_lockCheckbox = new QCheckBox("Lock", this);
    connect(m_lockCheckbox, &QCheckBox::toggled, this, [this](bool locked) {
        if (m_activeLayer) {
            m_activeLayer->setLocked(locked);
        }
    });
    propsLayout->addWidget(m_lockCheckbox);

    propsLayout->addStretch();
    mainLayout->addLayout(propsLayout);
}

void LayerPanel::setDocument(std::shared_ptr<LibreCanvas::Document> document)
{
    m_document = document;
    updateLayerList();
}

void LayerPanel::updateLayerList()
{
    m_layerTree->clear();
    m_activeItem = nullptr;

    if (!m_document) return;

    populateLayers();
}

void LayerPanel::populateLayers()
{
    if (!m_document) return;

    // Add layers from bottom to top (reverse order for display)
    const auto& layers = m_document->getLayers();
    for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
        QTreeWidgetItem *item = createLayerItem(*it);
        m_layerTree->addTopLevelItem(item);
    }
}

QTreeWidgetItem* LayerPanel::createLayerItem(std::shared_ptr<LibreCanvas::Layer> layer)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, layer->getName());
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(0, layer->isVisible() ? Qt::Checked : Qt::Unchecked);
    item->setData(0, Qt::UserRole, QVariant::fromValue<void*>(layer.get()));
    
    // Store layer reference
    item->setData(1, Qt::UserRole, QVariant::fromValue<std::shared_ptr<LibreCanvas::Layer>>(layer));
    
    updateLayerItem(item, layer);
    return item;
}

QTreeWidgetItem* LayerPanel::createGroupItem(std::shared_ptr<LibreCanvas::LayerGroup> group)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, group->getName());
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(0, group->isVisible() ? Qt::Checked : Qt::Unchecked);
    item->setExpanded(group->isExpanded());
    
    // Add layers in group
    for (const auto& layer : group->getLayers()) {
        QTreeWidgetItem *layerItem = createLayerItem(layer);
        item->addChild(layerItem);
    }
    
    return item;
}

void LayerPanel::updateLayerItem(QTreeWidgetItem *item, std::shared_ptr<LibreCanvas::Layer> layer)
{
    if (!item || !layer) return;
    
    item->setText(0, layer->getName());
    item->setCheckState(0, layer->isVisible() ? Qt::Checked : Qt::Unchecked);
    
    // Visual indicator for active layer
    QFont font = item->font(0);
    font.setBold(m_activeLayer == layer);
    item->setFont(0, font);
}

void LayerPanel::setActiveLayer(std::shared_ptr<LibreCanvas::Layer> layer)
{
    m_activeLayer = layer;
    
    // Update UI
    if (m_activeLayer) {
        m_layerNameEdit->setText(m_activeLayer->getName());
        m_opacitySlider->setValue(static_cast<int>(m_activeLayer->getOpacity() * 100));
        m_blendModeCombo->setCurrentIndex(static_cast<int>(m_activeLayer->getBlendMode()));
        m_lockCheckbox->setChecked(m_activeLayer->isLocked());
    }
    
    // Update all items to show which is active
    for (int i = 0; i < m_layerTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = m_layerTree->topLevelItem(i);
        if (item->data(1, Qt::UserRole).value<std::shared_ptr<LibreCanvas::Layer>>() == layer) {
            m_activeItem = item;
            m_layerTree->setCurrentItem(item);
        }
        updateLayerItem(item, item->data(1, Qt::UserRole).value<std::shared_ptr<LibreCanvas::Layer>>());
    }
}

void LayerPanel::onLayerItemClicked(QTreeWidgetItem *item, int column)
{
    if (!item) return;
    
    auto layer = item->data(1, Qt::UserRole).value<std::shared_ptr<LibreCanvas::Layer>>();
    if (layer) {
        setActiveLayer(layer);
        emit layerSelected(layer);
    }
}

void LayerPanel::onLayerItemChanged(QTreeWidgetItem *item, int column)
{
    if (column == 0) {
        auto layer = item->data(1, Qt::UserRole).value<std::shared_ptr<LibreCanvas::Layer>>();
        if (layer) {
            bool visible = item->checkState(0) == Qt::Checked;
            layer->setVisible(visible);
            emit layerVisibilityChanged(layer, visible);
        }
    }
}

void LayerPanel::onNewLayerClicked()
{
    if (!m_document) return;
    
    bool ok;
    QString name = QInputDialog::getText(this, "New Layer", "Layer name:", 
                                         QLineEdit::Normal, "Layer " + QString::number(m_document->getLayerCount() + 1), &ok);
    if (!ok || name.isEmpty()) return;
    
    auto layer = std::make_shared<LibreCanvas::Layer>(name, m_document->getSize().width(), m_document->getSize().height());
    m_document->addLayer(layer);
    updateLayerList();
    setActiveLayer(layer);
    emit createNewLayer();
}

void LayerPanel::onDeleteLayerClicked()
{
    if (!m_activeLayer || !m_document) return;
    
    if (m_document->getLayerCount() <= 1) {
        QMessageBox::warning(this, "Cannot Delete", "Cannot delete the last layer.");
        return;
    }
    
    m_document->removeLayer(m_activeLayer);
    emit deleteLayer(m_activeLayer);
    m_activeLayer = nullptr;
    updateLayerList();
}

void LayerPanel::onDuplicateLayerClicked()
{
    if (!m_activeLayer || !m_document) return;
    
    auto newLayer = std::make_shared<LibreCanvas::Layer>(
        m_activeLayer->getName() + " copy",
        m_activeLayer->getImage()
    );
    newLayer->setOpacity(m_activeLayer->getOpacity());
    newLayer->setBlendMode(m_activeLayer->getBlendMode());
    
    m_document->addLayer(newLayer);
    updateLayerList();
    setActiveLayer(newLayer);
    emit duplicateLayer(newLayer);
}

void LayerPanel::onNewGroupClicked()
{
    if (!m_document) return;
    
    bool ok;
    QString name = QInputDialog::getText(this, "New Group", "Group name:", 
                                         QLineEdit::Normal, "Group 1", &ok);
    if (!ok || name.isEmpty()) return;
    
    auto group = std::make_shared<LibreCanvas::LayerGroup>(name);
    m_document->addGroup(group);
    updateLayerList();
    emit createGroup();
}

void LayerPanel::onOpacitySliderChanged(int value)
{
    if (m_activeLayer) {
        float opacity = value / 100.0f;
        m_activeLayer->setOpacity(opacity);
        emit layerOpacityChanged(m_activeLayer, opacity);
    }
}

void LayerPanel::onBlendModeChanged(int index)
{
    if (m_activeLayer) {
        LibreCanvas::BlendMode mode = static_cast<LibreCanvas::BlendMode>(index);
        m_activeLayer->setBlendMode(mode);
        emit layerBlendModeChanged(m_activeLayer, mode);
    }
}

