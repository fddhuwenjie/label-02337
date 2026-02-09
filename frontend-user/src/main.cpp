#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    app.setApplicationName("XML to DOCX Converter");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("DocX Studio");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
