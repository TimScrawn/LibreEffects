#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QDockWidget>
#include "../../libs/branding/theme.h"
#include "canvaswidget.h"
#include "layerpanel.h"

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void fitToWindow();
    void updateStatusBar();
    void about();

private:
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void applyTheme();
    
    // Menu actions
    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_exitAction;
    QAction *m_aboutAction;
    
    // View actions
    QAction *m_zoomInAction;
    QAction *m_zoomOutAction;
    QAction *m_resetZoomAction;
    QAction *m_fitToWindowAction;
    
    // Menus
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_viewMenu;
    QMenu *m_helpMenu;
    
    // Toolbars
    QToolBar *m_mainToolBar;
    
    // Status bar
    QLabel *m_statusLabel;
    QLabel *m_zoomLabel;
    QLabel *m_sizeLabel;
    
    // Canvas widget
    CanvasWidget *m_canvasWidget;
    
    // Panels
    LayerPanel *m_layerPanel;
    QDockWidget *m_layerDock;
    class ToolPanel *m_toolPanel;
    QDockWidget *m_toolDock;
    
    // History
    std::shared_ptr<LibreCanvas::HistoryManager> m_historyManager;
    
    // Layer management
    void createLayer();
    void deleteLayer();
    void setupPanels();
    
    // Theme
    LibreEffects::Branding::Theme m_theme;
};

