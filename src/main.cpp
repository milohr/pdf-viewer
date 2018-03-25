#include <QApplication>
#include <QMainWindow>
#include <QIcon>
#include <QDeclarativeView>

#include "pdf_viewer/PdfViewer.h"
#include "pdf_viewer/PdfDocument.h"
#include "pdf_viewer/Polynomial.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    pdf_viewer::Polynomial poly;
    poly.set(1.5, 0.5, 2, 4);
    qDebug() << poly(0) << poly(1) << poly(1.5);

    // Register PDF viewer component to QML:
    qmlRegisterType<pdf_viewer::PdfDocument>("PdfViewing", 1, 0, "PdfDocument");
    qmlRegisterType<pdf_viewer::PdfViewer>("PdfViewing", 1, 0, "PdfViewer");

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
