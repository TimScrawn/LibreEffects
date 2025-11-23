#include "mainwindow.h"
#include "canvaswidget.h"
#include "layerpanel.h"
#include "toolpanel.h"
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
    
    // Create canvas widget
    m_canvasWidget = new CanvasWidget(this);
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
    // Apply dark theme styling
    auto bgColor = m_theme.getBackgroundColor();
    auto panelColor = m_theme.getPanelColor();
    auto textColor = m_theme.getTextColor();
    
    QString styleSheet = QString(
        "QMainWindow {"
        "    background-color: rgb(%1, %2, %3);"
        "    color: rgb(%4, %5, %6);"
        "}"
        "QMenuBar {"
        "    background-color: rgb(%7, %8, %9);"
        "    color: rgb(%4, %5, %6);"
        "}"
        "QToolBar {"
        "    background-color: rgb(%7, %8, %9);"
        "    border: none;"
        "}"
        "QStatusBar {"
        "    background-color: rgb(%7, %8, %9);"
        "    color: rgb(%4, %5, %6);"
        "}"
    ).arg(bgColor[0]).arg(bgColor[1]).arg(bgColor[2])
     .arg(textColor[0]).arg(textColor[1]).arg(textColor[2])
     .arg(panelColor[0]).arg(panelColor[1]).arg(panelColor[2]);
    
    setStyleSheet(styleSheet);
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

