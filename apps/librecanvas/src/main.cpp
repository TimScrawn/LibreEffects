#include <QApplication>
#include "mainwindow.h"
#include "../../libs/branding/theme.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName("LibreCanvas");
    app.setApplicationDisplayName("LibreCanvas");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("LibreEffects");
    app.setOrganizationDomain("libreeffects.org");
    
    // Initialize branding theme
    LibreEffects::Branding::Theme theme(LibreEffects::Branding::Theme::ThemeMode::Dark);
    
    // Create and show main window
    MainWindow window;
    window.show();
    
    return app.exec();
}

