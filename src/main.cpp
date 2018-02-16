#include <QApplication>

#include "Window.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Window w("test-pdf/Merged.pdf");
    w.resize(800, 600);
    w.show();

    return a.exec();
}
