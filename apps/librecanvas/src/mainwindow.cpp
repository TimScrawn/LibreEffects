#include "mainwindow.h"
#include "canvaswidget.h"
#include "layerpanel.h"
#include "toolpanel.h"
#include "history.h"
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QStyleFactory>
#include <QInputDialog>
#include <QHBoxLayout>
#include <QWidget>
#include <QFileInfo>
#include <QDockWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_canvasWidget(nullptr)
    , m_theme(LibreEffects::Branding::Theme::ThemeMode::Dark)
{
    setWindowTitle("LibreCanvas - LibreEffects");
    setMinimumSize(800, 600);
    resize(1200, 800);
    
    // Create history manager
    m_historyManager = std::make_shared<LibreCanvas::HistoryManager>(this);
    
    // Create canvas widget
    m_canvasWidget = new CanvasWidget(this);
    m_canvasWidget->setHistoryManager(m_historyManager);
    setCentralWidget(m_canvasWidget);
    
    // Connect canvas signals
    connect(m_canvasWidget, &CanvasWidget::imageChanged, this, &MainWindow::updateStatusBar);
    connect(m_canvasWidget, &CanvasWidget::zoomChanged, this, &MainWindow::updateStatusBar);
    
    createMenus();
    createToolBars();
    createStatusBar();
    setupPanels();
    applyTheme();
    updateStatusBar();
    
    // Connect canvas document changes to layer panel
    connect(m_canvasWidget, &CanvasWidget::documentChanged, this, [this]() {
        if (m_canvasWidget->getDocument()) {
            m_layerPanel->setDocument(m_canvasWidget->getDocument());
        }
    });
    
    // Connect layer panel signals
    connect(m_layerPanel, &LayerPanel::layerSelected, this, [this](std::shared_ptr<LibreCanvas::Layer> layer) {
        if (m_canvasWidget->getDocument()) {
            m_canvasWidget->getDocument()->setActiveLayer(layer);
            m_canvasWidget->update();
        }
    });
    connect(m_layerPanel, &LayerPanel::layerVisibilityChanged, this, [this](auto, bool) {
        m_canvasWidget->update();
    });
    connect(m_layerPanel, &LayerPanel::layerOpacityChanged, this, [this](auto, float) {
        m_canvasWidget->update();
    });
    connect(m_layerPanel, &LayerPanel::layerBlendModeChanged, this, [this](auto, auto) {
        m_canvasWidget->update();
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::createMenus()
{
    // File Menu
    m_fileMenu = menuBar()->addMenu("&File");
    
    m_newAction = new QAction("&New", this);
    m_newAction->setShortcut(QKeySequence::New);
    m_newAction->setStatusTip("Create a new image");
    connect(m_newAction, &QAction::triggered, this, &MainWindow::newFile);
    m_fileMenu->addAction(m_newAction);
    
    m_openAction = new QAction("&Open...", this);
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setStatusTip("Open an existing image");
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openFile);
    m_fileMenu->addAction(m_openAction);
    
    m_fileMenu->addSeparator();
    
    m_saveAction = new QAction("&Save", this);
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setStatusTip("Save the current image");
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    m_fileMenu->addAction(m_saveAction);
    
    m_fileMenu->addSeparator();
    
    m_exitAction = new QAction("E&xit", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip("Exit LibreCanvas");
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);
    m_fileMenu->addAction(m_exitAction);
    
    // Edit Menu
    m_editMenu = menuBar()->addMenu("&Edit");
    // Edit actions will be added here
    
    // View Menu
    m_viewMenu = menuBar()->addMenu("&View");
    
    QAction *zoomInAction = new QAction("Zoom &In", this);
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    zoomInAction->setStatusTip("Zoom in");
    connect(zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);
    m_viewMenu->addAction(zoomInAction);
    
    QAction *zoomOutAction = new QAction("Zoom &Out", this);
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    zoomOutAction->setStatusTip("Zoom out");
    connect(zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);
    m_viewMenu->addAction(zoomOutAction);
    
    m_viewMenu->addSeparator();
    
    QAction *resetZoomAction = new QAction("&Reset Zoom", this);
    resetZoomAction->setShortcut(Qt::CTRL | Qt::Key_0);
    resetZoomAction->setStatusTip("Reset zoom to 100%");
    connect(resetZoomAction, &QAction::triggered, this, &MainWindow::resetZoom);
    m_viewMenu->addAction(resetZoomAction);
    
    QAction *fitToWindowAction = new QAction("&Fit to Window", this);
    fitToWindowAction->setShortcut(Qt::CTRL | Qt::Key_9);
    fitToWindowAction->setStatusTip("Fit image to window");
    connect(fitToWindowAction, &QAction::triggered, this, &MainWindow::fitToWindow);
    m_viewMenu->addAction(fitToWindowAction);
    
    // Help Menu
    m_helpMenu = menuBar()->addMenu("&Help");
    
    m_aboutAction = new QAction("&About LibreCanvas", this);
    m_aboutAction->setStatusTip("Show application information");
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::about);
    m_helpMenu->addAction(m_aboutAction);
}

void MainWindow::createToolBars()
{
    m_mainToolBar = addToolBar("Main");
    m_mainToolBar->addAction(m_newAction);
    m_mainToolBar->addAction(m_openAction);
    m_mainToolBar->addAction(m_saveAction);
    m_mainToolBar->addSeparator();
    // More tools will be added here
}

void MainWindow::createStatusBar()
{
    m_statusLabel = new QLabel("Ready");
    statusBar()->addWidget(m_statusLabel);
    
    m_zoomLabel = new QLabel("Zoom: 100%");
    statusBar()->addPermanentWidget(m_zoomLabel);
    
    m_sizeLabel = new QLabel("Size: -");
    statusBar()->addPermanentWidget(m_sizeLabel);
    
    statusBar()->showMessage("Welcome to LibreCanvas", 3000);
}

void MainWindow::setupPanels()
{
    // Tool Panel (left side)
    m_toolPanel = new ToolPanel(this);
    m_toolDock = new QDockWidget("Tools", this);
    m_toolDock->setWidget(m_toolPanel);
    m_toolDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, m_toolDock);
    
    // Layer Panel (right side)
    m_layerPanel = new LayerPanel(this);
    m_layerDock = new QDockWidget("Layers", this);
    m_layerDock->setWidget(m_layerPanel);
    m_layerDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_layerDock);
    
    // Connect tool panel to canvas
    connect(m_toolPanel, &ToolPanel::toolChanged, m_canvasWidget, &CanvasWidget::setTool);
    connect(m_toolPanel, &ToolPanel::brushSizeChanged, m_toolPanel, [this](int size) {
        if (auto brushTool = std::dynamic_pointer_cast<LibreCanvas::BrushTool>(m_canvasWidget->getCurrentTool())) {
            brushTool->setSize(size);
        }
    });
    connect(m_toolPanel, &ToolPanel::brushColorChanged, m_toolPanel, [this](const QColor& color) {
        if (auto brushTool = std::dynamic_pointer_cast<LibreCanvas::BrushTool>(m_canvasWidget->getCurrentTool())) {
            brushTool->setColor(color);
        }
    });
}

void MainWindow::applyTheme()
{
    // Get modern theme colors
    auto bgColor = m_theme.getBackgroundColor();
    auto panelColor = m_theme.getPanelColor();
    auto textColor = m_theme.getTextColor();
    auto textSecondary = m_theme.getTextTertiaryColor();
    auto primaryColor = m_theme.getPrimaryColor();
    auto primaryDark = m_theme.getPrimaryDark();
    auto buttonHover = m_theme.getButtonHoverColor();
    auto buttonActive = m_theme.getButtonActiveColor();
    auto borderColor = m_theme.getBorderColor();
    auto borderHover = m_theme.getBorderHoverColor();
    auto panelHover = m_theme.getPanelHoverColor();
    auto selectionColor = m_theme.getSelectionColor();
    auto selectionHover = m_theme.getSelectionHoverColor();
    
    QString styleSheet = QString(
        // Main Window
        "QMainWindow {"
        "    background-color: rgb(%1, %2, %3);"
        "    color: rgb(%4, %5, %6);"
        "    font-family: 'Segoe UI', 'Inter', system-ui, sans-serif;"
        "    font-size: 13px;"
        "}"
        
        // Menu Bar - Modern flat design
        "QMenuBar {"
        "    background-color: rgb(%7, %8, %9);"
        "    color: rgb(%4, %5, %6);"
        "    border: none;"
        "    padding: 4px;"
        "    font-weight: 500;"
        "}"
        "QMenuBar::item {"
        "    background-color: transparent;"
        "    padding: 6px 12px;"
        "    border-radius: 6px;"
        "    margin: 2px;"
        "}"
        "QMenuBar::item:selected {"
        "    background-color: rgb(%10, %11, %12);"
        "}"
        "QMenuBar::item:pressed {"
        "    background-color: rgb(%13, %14, %15);"
        "}"
        
        // Menus - Modern dropdowns
        "QMenu {"
        "    background-color: rgb(%7, %8, %9);"
        "    color: rgb(%4, %5, %6);"
        "    border: 1px solid rgb(%16, %17, %18);"
        "    border-radius: 8px;"
        "    padding: 4px;"
        "}"
        "QMenu::item {"
        "    padding: 8px 24px 8px 32px;"
        "    border-radius: 6px;"
        "    margin: 2px;"
        "}"
        "QMenu::item:selected {"
        "    background-color: rgb(%10, %11, %12);"
        "}"
        "QMenu::separator {"
        "    height: 1px;"
        "    background-color: rgb(%16, %17, %18);"
        "    margin: 4px 8px;"
        "}"
        
        // Tool Bar
        "QToolBar {"
        "    background-color: rgb(%7, %8, %9);"
        "    border: none;"
        "    border-bottom: 1px solid rgb(%16, %17, %18);"
        "    spacing: 4px;"
        "    padding: 4px;"
        "}"
        
        // Status Bar - Modern bottom bar
        "QStatusBar {"
        "    background-color: rgb(%7, %8, %9);"
        "    color: rgb(%4, %5, %6);"
        "    border: none;"
        "    border-top: 1px solid rgb(%16, %17, %18);"
        "    padding: 4px 8px;"
        "    font-size: 11px;"
        "}"
        
        // Buttons - Modern rounded buttons
        "QPushButton {"
        "    background-color: rgb(%7, %8, %9);"
        "    color: rgb(%4, %5, %6);"
        "    border: 1px solid rgb(%16, %17, %18);"
        "    border-radius: 8px;"
        "    padding: 6px 12px;"
        "    font-weight: 500;"
        "    min-height: 28px;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgb(%10, %11, %12);"
        "    border-color: rgb(%19, %20, %21);"
        "}"
        "QPushButton:pressed {"
        "    background-color: rgb(%13, %14, %15);"
        "    border-color: rgb(%22, %23, %24);"
        "}"
        "QPushButton:disabled {"
        "    background-color: rgb(%7, %8, %9);"
        "    color: rgb(%25, %26, %27);"
        "    border-color: rgb(%16, %17, %18);"
        "}"
        
        // Tool Buttons - Icon buttons
        "QToolButton {"
        "    background-color: rgb(%7, %8, %9);"
        "    color: rgb(%4, %5, %6);"
        "    border: 1px solid rgb(%16, %17, %18);"
        "    border-radius: 8px;"
        "    padding: 8px;"
        "    min-width: 40px;"
        "    min-height: 40px;"
        "    font-weight: 600;"
        "    font-size: 14px;"
        "}"
        "QToolButton:hover {"
        "    background-color: rgb(%10, %11, %12);"
        "    border-color: rgb(%19, %20, %21);"
        "    transform: scale(1.05);"
        "}"
        "QToolButton:checked {"
        "    background-color: rgb(%22, %23, %24);"
        "    border-color: rgb(%22, %23, %24);"
        "    color: rgb(255, 255, 255);"
        "}"
        "QToolButton:pressed {"
        "    background-color: rgb(%13, %14, %15);"
        "}"
        
        // Dock Widgets - Modern panels
        "QDockWidget {"
        "    background-color: rgb(%7, %8, %9);"
        "    color: rgb(%4, %5, %6);"
        "    titlebar-close-icon: none;"
        "    titlebar-normal-icon: none;"
        "    font-weight: 600;"
        "}"
        "QDockWidget::title {"
        "    background-color: rgb(%7, %8, %9);"
        "    padding: 8px 12px;"
        "    border-bottom: 1px solid rgb(%16, %17, %18);"
        "    font-size: 12px;"
        "    text-transform: uppercase;"
        "    letter-spacing: 0.5px;"
        "}"
        
        // Group Boxes - Modern containers
        "QGroupBox {"
        "    background-color: transparent;"
        "    color: rgb(%4, %5, %6);"
        "    border: 1px solid rgb(%16, %17, %18);"
        "    border-radius: 8px;"
        "    margin-top: 12px;"
        "    padding-top: 12px;"
        "    font-weight: 600;"
        "    font-size: 11px;"
        "    text-transform: uppercase;"
        "    letter-spacing: 0.5px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    padding: 0 8px;"
        "    color: rgb(%25, %26, %27);"
        "}"
        
        // Sliders - Modern sliders
        "QSlider::groove:horizontal {"
        "    background-color: rgb(%16, %17, %18);"
        "    height: 4px;"
        "    border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "    background-color: rgb(%22, %23, %24);"
        "    width: 16px;"
        "    height: 16px;"
        "    margin: -6px 0;"
        "    border-radius: 8px;"
        "    border: 2px solid rgb(%7, %8, %9);"
        "}"
        "QSlider::handle:horizontal:hover {"
        "    background-color: rgb(%28, %29, %30);"
        "    transform: scale(1.1);"
        "}"
        "QSlider::sub-page:horizontal {"
        "    background-color: rgb(%22, %23, %24);"
        "    border-radius: 2px;"
        "}"
        
        // Spin Boxes - Modern number inputs
        "QSpinBox, QDoubleSpinBox {"
        "    background-color: rgb(%7, %8, %9);"
        "    color: rgb(%4, %5, %6);"
        "    border: 1px solid rgb(%16, %17, %18);"
        "    border-radius: 6px;"
        "    padding: 4px 8px;"
        "    min-height: 24px;"
        "}"
        "QSpinBox:hover, QDoubleSpinBox:hover {"
        "    border-color: rgb(%19, %20, %21);"
        "}"
        "QSpinBox:focus, QDoubleSpinBox:focus {"
        "    border-color: rgb(%22, %23, %24);"
        "    background-color: rgb(%10, %11, %12);"
        "}"
        
        // Line Edits - Modern text inputs
        "QLineEdit {"
        "    background-color: rgb(%7, %8, %9);"
        "    color: rgb(%4, %5, %6);"
        "    border: 1px solid rgb(%16, %17, %18);"
        "    border-radius: 6px;"
        "    padding: 6px 10px;"
        "    min-height: 24px;"
        "}"
        "QLineEdit:hover {"
        "    border-color: rgb(%19, %20, %21);"
        "}"
        "QLineEdit:focus {"
        "    border-color: rgb(%22, %23, %24);"
        "    background-color: rgb(%10, %11, %12);"
        "}"
        
        // Combo Boxes - Modern dropdowns
        "QComboBox {"
        "    background-color: rgb(%7, %8, %9);"
        "    color: rgb(%4, %5, %6);"
        "    border: 1px solid rgb(%16, %17, %18);"
        "    border-radius: 6px;"
        "    padding: 6px 10px;"
        "    min-height: 24px;"
        "}"
        "QComboBox:hover {"
        "    border-color: rgb(%19, %20, %21);"
        "}"
        "QComboBox:focus {"
        "    border-color: rgb(%22, %23, %24);"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "    width: 20px;"
        "}"
        "QComboBox::down-arrow {"
        "    image: none;"
        "    border-left: 4px solid transparent;"
        "    border-right: 4px solid transparent;"
        "    border-top: 5px solid rgb(%4, %5, %6);"
        "    margin-right: 6px;"
        "}"
        "QComboBox QAbstractItemView {"
        "    background-color: rgb(%7, %8, %9);"
        "    color: rgb(%4, %5, %6);"
        "    border: 1px solid rgb(%16, %17, %18);"
        "    border-radius: 8px;"
        "    selection-background-color: rgb(%22, %23, %24);"
        "    padding: 4px;"
        "}"
        
        // Tree Widgets - Modern layer panel
        "QTreeWidget {"
        "    background-color: rgb(%7, %8, %9);"
        "    color: rgb(%4, %5, %6);"
        "    border: none;"
        "    outline: none;"
        "    font-size: 12px;"
        "}"
        "QTreeWidget::item {"
        "    padding: 6px;"
        "    border-radius: 4px;"
        "    margin: 1px;"
        "}"
        "QTreeWidget::item:hover {"
        "    background-color: rgb(%10, %11, %12);"
        "}"
        "QTreeWidget::item:selected {"
        "    background-color: rgb(%22, %23, %24);"
        "    color: rgb(255, 255, 255);"
        "}"
        "QTreeWidget::branch {"
        "    background-color: transparent;"
        "}"
        
        // Scroll Bars - Modern scrollbars
        "QScrollBar:vertical {"
        "    background-color: rgb(%7, %8, %9);"
        "    width: 10px;"
        "    border: none;"
        "    margin: 0;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background-color: rgb(%16, %17, %18);"
        "    min-height: 20px;"
        "    border-radius: 5px;"
        "    margin: 2px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background-color: rgb(%19, %20, %21);"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0px;"
        "}"
        "QScrollBar:horizontal {"
        "    background-color: rgb(%7, %8, %9);"
        "    height: 10px;"
        "    border: none;"
        "    margin: 0;"
        "}"
        "QScrollBar::handle:horizontal {"
        "    background-color: rgb(%16, %17, %18);"
        "    min-width: 20px;"
        "    border-radius: 5px;"
        "    margin: 2px;"
        "}"
        "QScrollBar::handle:horizontal:hover {"
        "    background-color: rgb(%19, %20, %21);"
        "}"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {"
        "    width: 0px;"
        "}"
        
        // Labels
        "QLabel {"
        "    color: rgb(%4, %5, %6);"
        "    font-size: 12px;"
        "}"
        
        // Checkboxes - Modern checkboxes
        "QCheckBox {"
        "    color: rgb(%4, %5, %6);"
        "    spacing: 8px;"
        "}"
        "QCheckBox::indicator {"
        "    width: 18px;"
        "    height: 18px;"
        "    border: 2px solid rgb(%16, %17, %18);"
        "    border-radius: 4px;"
        "    background-color: rgb(%7, %8, %9);"
        "}"
        "QCheckBox::indicator:hover {"
        "    border-color: rgb(%19, %20, %21);"
        "}"
        "QCheckBox::indicator:checked {"
        "    background-color: rgb(%22, %23, %24);"
        "    border-color: rgb(%22, %23, %24);"
        "    image: none;"
        "}"
        "QCheckBox::indicator:checked::after {"
        "    content: '✓';"
        "    color: white;"
        "}"
        
        // Color Dialog Button (if needed)
        "QPushButton#colorButton {"
        "    border-radius: 6px;"
        "    min-width: 60px;"
        "    min-height: 30px;"
        "}"
    )
    .arg(bgColor[0]).arg(bgColor[1]).arg(bgColor[2])           // Main window bg
    .arg(textColor[0]).arg(textColor[1]).arg(textColor[2])    // Text primary
    .arg(panelColor[0]).arg(panelColor[1]).arg(panelColor[2]) // Panel color
    .arg(buttonHover[0]).arg(buttonHover[1]).arg(buttonHover[2]) // Button hover
    .arg(buttonActive[0]).arg(buttonActive[1]).arg(buttonActive[2]) // Button active
    .arg(borderColor[0]).arg(borderColor[1]).arg(borderColor[2]) // Border
    .arg(borderHover[0]).arg(borderHover[1]).arg(borderHover[2]) // Border hover
    .arg(primaryColor[0]).arg(primaryColor[1]).arg(primaryColor[2]) // Primary/Selection
    .arg(selectionHover[0]).arg(selectionHover[1]).arg(selectionHover[2]) // Selection hover
    .arg(textSecondary[0]).arg(textSecondary[1]).arg(textSecondary[2]); // Text secondary
    
    setStyleSheet(styleSheet);
    
    // Apply modern font
    QFont modernFont("Segoe UI", 13);
    modernFont.setStyleHint(QFont::SansSerif);
    qApp->setFont(modernFont);
}

void MainWindow::newFile()
{
    bool ok;
    int width = QInputDialog::getInt(this, "New Image", "Width:", 1920, 1, 10000, 1, &ok);
    if (!ok) return;
    
    int height = QInputDialog::getInt(this, "New Image", "Height:", 1080, 1, 10000, 1, &ok);
    if (!ok) return;
    
    m_canvasWidget->newImage(width, height);
    if (m_canvasWidget->getDocument()) {
        m_layerPanel->setDocument(m_canvasWidget->getDocument());
    }
    m_statusLabel->setText(QString("New image created: %1x%2").arg(width).arg(height));
    updateStatusBar();
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Image", "", 
        "Image Files (*.png *.jpg *.jpeg *.bmp *.tiff *.gif *.webp);;All Files (*.*)");
    if (!fileName.isEmpty()) {
        if (m_canvasWidget->loadImage(fileName)) {
            if (m_canvasWidget->getDocument()) {
                m_layerPanel->setDocument(m_canvasWidget->getDocument());
            }
            m_statusLabel->setText("Opened: " + QFileInfo(fileName).fileName());
            updateStatusBar();
        }
    }
}

void MainWindow::saveFile()
{
    if (!m_canvasWidget->hasImage()) {
        QMessageBox::information(this, "Save", "No image to save.");
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Image", "", 
        "PNG Files (*.png);;JPEG Files (*.jpg);;BMP Files (*.bmp);;TIFF Files (*.tiff)");
    if (!fileName.isEmpty()) {
        if (m_canvasWidget->saveImage(fileName)) {
            m_statusLabel->setText("Saved: " + QFileInfo(fileName).fileName());
        }
    }
}

void MainWindow::zoomIn()
{
    m_canvasWidget->zoomIn();
}

void MainWindow::zoomOut()
{
    m_canvasWidget->zoomOut();
}

void MainWindow::resetZoom()
{
    m_canvasWidget->resetZoom();
}

void MainWindow::fitToWindow()
{
    m_canvasWidget->fitToWindow();
}

void MainWindow::updateStatusBar()
{
    if (m_canvasWidget->hasImage()) {
        QImage img = m_canvasWidget->getImage();
        auto doc = m_canvasWidget->getDocument();
        if (doc) {
            int layerCount = doc->getLayerCount();
            m_sizeLabel->setText(QString("Size: %1x%2 | Layers: %3").arg(img.width()).arg(img.height()).arg(layerCount));
        } else {
            m_sizeLabel->setText(QString("Size: %1x%2").arg(img.width()).arg(img.height()));
        }
        m_zoomLabel->setText(QString("Zoom: %1%").arg(static_cast<int>(m_canvasWidget->getZoomLevel() * 100)));
    } else {
        m_sizeLabel->setText("Size: -");
        m_zoomLabel->setText("Zoom: -");
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, "About LibreCanvas",
        "<h2>LibreCanvas</h2>"
        "<p>Version 0.1.0</p>"
        "<p>Professional image editing application</p>"
        "<p>Part of the LibreEffects suite</p>"
        "<p><a href='https://libreeffects.org'>libreeffects.org</a></p>");
}

