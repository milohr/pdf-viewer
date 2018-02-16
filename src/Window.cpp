#include <QKeyEvent>
#include <QIcon>

#include "Window.h"
#include "PdfWidget.h"

Window::Window(
        QString const &pdfPath,
        QWidget *parent
)
    : QMainWindow(parent)
    , mPdfWidget(new PdfWidget(pdfPath))
{
    setWindowIcon(QIcon(":/img/icon_mres.png"));
    setWindowTitle(mPdfWidget->documentTitle());
    setCentralWidget(mPdfWidget);
}

void Window::keyPressEvent(
        QKeyEvent * const event
)
{
    switch(event->key()) {
    case Qt::Key_Escape:
        close();
        break;

    case Qt::Key_Y:
        mPdfWidget->prevPage();
        break;

    case Qt::Key_X:
        mPdfWidget->nextPage();
        break;

    case Qt::Key_Q:
        mPdfWidget->rotateLeft();
        break;

    case Qt::Key_E:
        mPdfWidget->rotateRight();
        break;

    case Qt::Key_R:
        mPdfWidget->pageFit();
        break;

    case Qt::Key_F:
        mPdfWidget->screenFit();
        break;

    case Qt::Key_Plus:
        mPdfWidget->setZoom(mPdfWidget->zoom() * 1.1);
        break;

    case Qt::Key_Minus:
        mPdfWidget->setZoom(mPdfWidget->zoom() * 0.9);
        break;

    case Qt::Key_W:
        mPdfWidget->pan(0, -10);
        mPdfWidget->update();
        break;

    case Qt::Key_S:
        mPdfWidget->pan(0, 10);
        mPdfWidget->update();
        break;

    case Qt::Key_A:
        mPdfWidget->pan(-10, 0);
        mPdfWidget->update();
        break;

    case Qt::Key_D:
        mPdfWidget->pan(10, 0);
        mPdfWidget->update();
        break;
    }
}
