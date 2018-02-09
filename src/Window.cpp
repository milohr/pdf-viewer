#include "Window.h"

Window::Window(QString const &pdfPath, QWidget *parent)
    : QMainWindow(parent)
    , mPdfWidget(new PdfWidget(pdfPath))
    , mPdfInfo(new PdfInfoWidget(mPdfWidget->getPopplerDocument()))
{
    setWindowIcon(QIcon(":/assets/icon_mres.png"));

    auto centerLayout = new QHBoxLayout();
    auto verticalLayout = new QVBoxLayout;
    auto headerBar = new HeaderBar(QFileInfo(pdfPath).fileName());
    auto rootWidget = new QWidget();
    auto statusBar = new  StatusBar;

    centerLayout->addWidget(mPdfWidget);
    centerLayout->addWidget(mZoomSlider);
    centerLayout->addWidget(mPdfInfo);
    centerLayout->setMargin(0);
    verticalLayout->addWidget(headerBar);
    verticalLayout->addLayout(centerLayout, 1);
    verticalLayout->setMargin(0);
    rootWidget->setLayout(verticalLayout);

    hideInfo();

    resize(360 * 1.4, 600 * 1.4);
    setStatusBar(statusBar);
    setWindowTitle("PdfViewer - " + QFileInfo(pdfPath).fileName());
    setCentralWidget(rootWidget);

    connect(mPdfWidget, SIGNAL(pagesChanged(int,int)), statusBar, SLOT(setPageInformation(int,int)));
    connect(statusBar, SIGNAL(screenFit()), mPdfWidget, SLOT(screenFit()));
    connect(statusBar, SIGNAL(pageFit()), mPdfWidget, SLOT(pageFit()));
    connect(statusBar, SIGNAL(prevPage()), mPdfWidget, SLOT(prevPage()));
    connect(statusBar, SIGNAL(nextPage()), mPdfWidget, SLOT(nextPage()));
    connect(statusBar, SIGNAL(rotateLeft()), mPdfWidget, SLOT(rotateLeft()));
    connect(statusBar, SIGNAL(rotateRight()), mPdfWidget, SLOT(rotateRight()));
    connect(mPdfWidget, SIGNAL(zoomChanged(double)), mZoomSlider, SLOT(setZoom(double)));
    connect(mZoomSlider, SIGNAL(zoomChanged(double)), mPdfWidget, SLOT(setZoom(double)));
    connect(headerBar, SIGNAL(exit()), this, SLOT(close()));
    connect(headerBar, SIGNAL(showInfo()), this, SLOT(showInfo()));
    connect(mPdfInfo, SIGNAL(returnToViewer()), this, SLOT(hideInfo()));

    mPdfWidget->open();
}

void Window::keyPressEvent(QKeyEvent *event)
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

void Window::showInfo()
{
    mPdfWidget->hide();
    mZoomSlider->hide();
    mPdfInfo->show();
}

void Window::hideInfo()
{
    mPdfInfo->hide();
    mPdfWidget->show();
    mZoomSlider->show();
}
