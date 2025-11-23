#include "toolpanel.h"
#include "lassotool.h"
#include "clonestamptool.h"
#include <QGridLayout>
#include <QGroupBox>

ToolPanel::ToolPanel(QWidget *parent)
    : QWidget(parent)
    , m_brushColor(Qt::black)
{
    setupUI();
}

void ToolPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // Tool buttons
    QGroupBox *toolGroup = new QGroupBox("Tools", this);
    QGridLayout *toolLayout = new QGridLayout(toolGroup);
    
    m_toolGroup = new QButtonGroup(this);
    
    m_brushBtn = new QToolButton(this);
    m_brushBtn->setText("B");
    m_brushBtn->setToolTip("Brush Tool (B)");
    m_brushBtn->setCheckable(true);
    m_brushBtn->setChecked(true);
    m_toolGroup->addButton(m_brushBtn, 0);
    toolLayout->addWidget(m_brushBtn, 0, 0);
    
    m_eraserBtn = new QToolButton(this);
    m_eraserBtn->setText("E");
    m_eraserBtn->setToolTip("Eraser Tool (E)");
    m_eraserBtn->setCheckable(true);
    m_toolGroup->addButton(m_eraserBtn, 1);
    toolLayout->addWidget(m_eraserBtn, 0, 1);
    
    m_marqueeBtn = new QToolButton(this);
    m_marqueeBtn->setText("M");
    m_marqueeBtn->setToolTip("Rectangular Marquee (M)");
    m_marqueeBtn->setCheckable(true);
    m_toolGroup->addButton(m_marqueeBtn, 2);
    toolLayout->addWidget(m_marqueeBtn, 1, 0);
    
    m_magicWandBtn = new QToolButton(this);
    m_magicWandBtn->setText("W");
    m_magicWandBtn->setToolTip("Magic Wand (W)");
    m_magicWandBtn->setCheckable(true);
    m_toolGroup->addButton(m_magicWandBtn, 3);
    toolLayout->addWidget(m_magicWandBtn, 1, 1);
    
    QToolButton *lassoBtn = new QToolButton(this);
    lassoBtn->setText("L");
    lassoBtn->setToolTip("Lasso Tool (L)");
    lassoBtn->setCheckable(true);
    m_toolGroup->addButton(lassoBtn, 4);
    toolLayout->addWidget(lassoBtn, 2, 0);
    
    QToolButton *cloneBtn = new QToolButton(this);
    cloneBtn->setText("S");
    cloneBtn->setToolTip("Clone Stamp (S)");
    cloneBtn->setCheckable(true);
    m_toolGroup->addButton(cloneBtn, 5);
    toolLayout->addWidget(cloneBtn, 2, 1);
    
    connect(m_toolGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), 
            this, &ToolPanel::onToolButtonClicked);
    
    mainLayout->addWidget(toolGroup);

    // Brush properties
    QGroupBox *brushGroup = new QGroupBox("Brush", this);
    QVBoxLayout *brushLayout = new QVBoxLayout(brushGroup);
    
    // Size
    QHBoxLayout *sizeLayout = new QHBoxLayout();
    sizeLayout->addWidget(new QLabel("Size:", this));
    m_sizeSlider = new QSlider(Qt::Horizontal, this);
    m_sizeSlider->setRange(1, 500);
    m_sizeSlider->setValue(20);
    connect(m_sizeSlider, &QSlider::valueChanged, this, &ToolPanel::onBrushSizeChanged);
    sizeLayout->addWidget(m_sizeSlider);
    m_sizeLabel = new QLabel("20", this);
    m_sizeLabel->setMinimumWidth(30);
    connect(m_sizeSlider, &QSlider::valueChanged, m_sizeLabel, [this](int value) {
        m_sizeLabel->setText(QString::number(value));
    });
    sizeLayout->addWidget(m_sizeLabel);
    brushLayout->addLayout(sizeLayout);
    
    // Hardness
    QHBoxLayout *hardnessLayout = new QHBoxLayout();
    hardnessLayout->addWidget(new QLabel("Hardness:", this));
    m_hardnessSlider = new QSlider(Qt::Horizontal, this);
    m_hardnessSlider->setRange(0, 100);
    m_hardnessSlider->setValue(100);
    connect(m_hardnessSlider, &QSlider::valueChanged, this, &ToolPanel::onBrushHardnessChanged);
    hardnessLayout->addWidget(m_hardnessSlider);
    m_hardnessLabel = new QLabel("100%", this);
    m_hardnessLabel->setMinimumWidth(40);
    connect(m_hardnessSlider, &QSlider::valueChanged, m_hardnessLabel, [this](int value) {
        m_hardnessLabel->setText(QString("%1%").arg(value));
    });
    hardnessLayout->addWidget(m_hardnessLabel);
    brushLayout->addLayout(hardnessLayout);
    
    // Opacity
    QHBoxLayout *opacityLayout = new QHBoxLayout();
    opacityLayout->addWidget(new QLabel("Opacity:", this));
    m_opacitySlider = new QSlider(Qt::Horizontal, this);
    m_opacitySlider->setRange(0, 100);
    m_opacitySlider->setValue(100);
    connect(m_opacitySlider, &QSlider::valueChanged, this, &ToolPanel::onBrushOpacityChanged);
    opacityLayout->addWidget(m_opacitySlider);
    m_opacityLabel = new QLabel("100%", this);
    m_opacityLabel->setMinimumWidth(40);
    connect(m_opacitySlider, &QSlider::valueChanged, m_opacityLabel, [this](int value) {
        m_opacityLabel->setText(QString("%1%").arg(value));
    });
    opacityLayout->addWidget(m_opacityLabel);
    brushLayout->addLayout(opacityLayout);
    
    // Color
    QHBoxLayout *colorLayout = new QHBoxLayout();
    colorLayout->addWidget(new QLabel("Color:", this));
    m_colorBtn = new QPushButton(this);
    m_colorBtn->setMinimumSize(50, 30);
    m_colorBtn->setStyleSheet(QString("background-color: %1;").arg(m_brushColor.name()));
    connect(m_colorBtn, &QPushButton::clicked, this, &ToolPanel::onColorButtonClicked);
    colorLayout->addWidget(m_colorBtn);
    brushLayout->addLayout(colorLayout);
    
    mainLayout->addWidget(brushGroup);
    mainLayout->addStretch();
}

void ToolPanel::setTool(std::shared_ptr<LibreCanvas::Tool> tool)
{
    m_currentTool = tool;
    updateToolProperties();
}

void ToolPanel::updateToolProperties()
{
    if (!m_currentTool) return;
    
    // Update UI based on tool type
    switch (m_currentTool->getType()) {
        case LibreCanvas::ToolType::Brush:
            m_brushBtn->setChecked(true);
            break;
        case LibreCanvas::ToolType::Eraser:
            m_eraserBtn->setChecked(true);
            break;
        case LibreCanvas::ToolType::MarqueeRect:
            m_marqueeBtn->setChecked(true);
            break;
        case LibreCanvas::ToolType::MagicWand:
            m_magicWandBtn->setChecked(true);
            break;
        default:
            break;
    }
}

void ToolPanel::onToolButtonClicked(int id)
{
    std::shared_ptr<LibreCanvas::Tool> tool;
    
    switch (id) {
        case 0: // Brush
            tool = std::make_shared<LibreCanvas::BrushTool>();
            break;
        case 1: // Eraser
            tool = std::make_shared<LibreCanvas::EraserTool>();
            break;
        case 2: // Marquee
            tool = std::make_shared<LibreCanvas::MarqueeRectTool>();
            break;
        case 3: // Magic Wand
            tool = std::make_shared<LibreCanvas::MagicWandTool>();
            break;
        case 4: // Lasso
            tool = std::make_shared<LibreCanvas::LassoTool>();
            break;
        case 5: // Clone Stamp
            tool = std::make_shared<LibreCanvas::CloneStampTool>();
            if (auto cloneTool = std::dynamic_pointer_cast<LibreCanvas::CloneStampTool>(tool)) {
                cloneTool->setSize(m_sizeSlider->value());
                cloneTool->setHardness(m_hardnessSlider->value() / 100.0f);
                cloneTool->setOpacity(m_opacitySlider->value() / 100.0f);
            }
            break;
    }
    
    if (tool) {
        // Apply brush properties if it's a brush tool
        if (auto brushTool = std::dynamic_pointer_cast<LibreCanvas::BrushTool>(tool)) {
            brushTool->setSize(m_sizeSlider->value());
            brushTool->setHardness(m_hardnessSlider->value() / 100.0f);
            brushTool->setOpacity(m_opacitySlider->value() / 100.0f);
            brushTool->setColor(m_brushColor);
        } else if (auto eraserTool = std::dynamic_pointer_cast<LibreCanvas::EraserTool>(tool)) {
            eraserTool->setSize(m_sizeSlider->value());
            eraserTool->setHardness(m_hardnessSlider->value() / 100.0f);
            eraserTool->setOpacity(m_opacitySlider->value() / 100.0f);
        }
        
        emit toolChanged(tool);
    }
}

void ToolPanel::onBrushSizeChanged(int value)
{
    emit brushSizeChanged(value);
    if (auto brushTool = std::dynamic_pointer_cast<LibreCanvas::BrushTool>(m_currentTool)) {
        brushTool->setSize(value);
    } else if (auto eraserTool = std::dynamic_pointer_cast<LibreCanvas::EraserTool>(m_currentTool)) {
        eraserTool->setSize(value);
    } else if (auto cloneTool = std::dynamic_pointer_cast<LibreCanvas::CloneStampTool>(m_currentTool)) {
        cloneTool->setSize(value);
    }
}

void ToolPanel::onBrushHardnessChanged(int value)
{
    float hardness = value / 100.0f;
    emit brushHardnessChanged(hardness);
    if (auto brushTool = std::dynamic_pointer_cast<LibreCanvas::BrushTool>(m_currentTool)) {
        brushTool->setHardness(hardness);
    } else if (auto eraserTool = std::dynamic_pointer_cast<LibreCanvas::EraserTool>(m_currentTool)) {
        eraserTool->setHardness(hardness);
    } else if (auto cloneTool = std::dynamic_pointer_cast<LibreCanvas::CloneStampTool>(m_currentTool)) {
        cloneTool->setHardness(hardness);
    }
}

void ToolPanel::onBrushOpacityChanged(int value)
{
    float opacity = value / 100.0f;
    emit brushOpacityChanged(opacity);
    if (auto brushTool = std::dynamic_pointer_cast<LibreCanvas::BrushTool>(m_currentTool)) {
        brushTool->setOpacity(opacity);
    } else if (auto eraserTool = std::dynamic_pointer_cast<LibreCanvas::EraserTool>(m_currentTool)) {
        eraserTool->setOpacity(opacity);
    } else if (auto cloneTool = std::dynamic_pointer_cast<LibreCanvas::CloneStampTool>(m_currentTool)) {
        cloneTool->setOpacity(opacity);
    }
}

void ToolPanel::onColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_brushColor, this, "Select Brush Color");
    if (color.isValid()) {
        m_brushColor = color;
        m_colorBtn->setStyleSheet(QString("background-color: %1;").arg(m_brushColor.name()));
        emit brushColorChanged(m_brushColor);
        
        if (auto brushTool = std::dynamic_pointer_cast<LibreCanvas::BrushTool>(m_currentTool)) {
            brushTool->setColor(m_brushColor);
        }
    }
}

