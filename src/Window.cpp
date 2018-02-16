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
    //setWindowTitle("PdfViewer - " + QFileInfo(pdfPath).fileName()); // todo: pdf component provides document information
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

    case Qt::Key_D:
        mPdfWidget->nextPage();
        break;

    case Qt::Key_A:
        mPdfWidget->prevPage();
        break;

    case Qt::Key_Q:
        mPdfWidget->rotateLeft();
        break;

    case Qt::Key_E:
        mPdfWidget->rotateRight();
        break;

    case Qt::Key_W:
        mPdfWidget->pageFit();
        break;

    case Qt::Key_S:
        mPdfWidget->screenFit();
        break;
    }
}
