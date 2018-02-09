#include <QApplication>
#include <QDesktopWidget>

#include "Window.h"
#include "CLOptions.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(argc <= 1) {
        printf(
                    "Usage: %s   <path-to-pdf>  <options>\n"
                    "Options:\n"
                    "  -windowed       Viewer is opened in windowed mode instead of fullscreen\n"
                    , argv[0]);
        return -1;
    }

    CLOptions const options(argc, argv);

    Window w(options.path);

    if(options.windowed) {
        QDesktopWidget const * const destop = new QDesktopWidget();
        w.move(destop->screenGeometry().width() / 2 - w.width() / 2,
               destop->screenGeometry().height() / 2 - w.height() / 2);
        delete destop;
        w.show();
    }
    else {
        w.showFullScreen();
    }

    return a.exec();
}
