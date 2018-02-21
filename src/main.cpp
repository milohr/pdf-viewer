#include <QApplication>
#include <QMainWindow>
#include <QIcon>
#include <QDeclarativeView>

#include "PdfViewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Register PDF viewer component to QML:
    qmlRegisterType<PdfViewer>("PdfViewer", 1, 0, "PdfViewer");

    // Create the main window:
    QMainWindow window;
    window.setWindowIcon(QIcon(":/img/icon_mres.png"));

    // Create the declarative view, displaying the `main.qml` file:
    QDeclarativeView *view = new QDeclarativeView;
    view->setSource(QUrl("qrc:/qml/main.qml"));
    view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    window.setCentralWidget(view);

    // Display the window:
    window.resize(1200, 800);
    window.show();

    return a.exec();
}
