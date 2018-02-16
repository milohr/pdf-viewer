#include <QApplication>
#include <QMainWindow>

#include "Window.h"
#include "PdfViewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Window w;
    w.resize(800, 600);
    w.show();

    return a.exec();
}
