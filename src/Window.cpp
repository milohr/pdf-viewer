#include "Window.h"

Window::Window(const QString &pdfPath, QWidget *parent)
    : QMainWindow(parent)
    , mPdfWidget(new PdfWidget(pdfPath))
    , mHeaderBar(new HeaderBar(QFileInfo(pdfPath).fileName()))
    , mPdfInfo(new PdfInfoWidget(mPdfWidget->getPopplerDocument()))
{
    setWindowIcon(QIcon(":/assets/icon_mres.png"));

    mCenterLayout->addWidget(mPdfWidget);
    mCenterLayout->addWidget(mZoomSlider);
    mCenterLayout->addWidget(mPdfInfo);
    mCenterLayout->setMargin(0);
    mVerticalLayout->addWidget(mHeaderBar);
    mVerticalLayout->addLayout(mCenterLayout, 1);
    mVerticalLayout->setMargin(0);
    mRootWidget->setLayout(mVerticalLayout);

    hideInfo();

    resize(360 * 1.4, 600 * 1.4);
    setStatusBar(mStatusBar);
    setWindowTitle("PdfViewer - " + QFileInfo(pdfPath).fileName());
    setCentralWidget(mRootWidget);

    connect(mPdfWidget, SIGNAL(pagesChanged(int,int)), mStatusBar, SLOT(setPageInformation(int,int)));
    connect(mStatusBar, SIGNAL(screenFit()), mPdfWidget, SLOT(screenFit()));
    connect(mStatusBar, SIGNAL(pageFit()), mPdfWidget, SLOT(pageFit()));
    connect(mStatusBar, SIGNAL(prevPage()), mPdfWidget, SLOT(prevPage()));
    connect(mStatusBar, SIGNAL(nextPage()), mPdfWidget, SLOT(nextPage()));
    connect(mStatusBar, SIGNAL(rotateLeft()), mPdfWidget, SLOT(rotateLeft()));
    connect(mStatusBar, SIGNAL(rotateRight()), mPdfWidget, SLOT(rotateRight()));
    connect(mPdfWidget, SIGNAL(zoomChanged(double)), mZoomSlider, SLOT(setZoom(double)));
    connect(mZoomSlider, SIGNAL(zoomChanged(double)), mPdfWidget, SLOT(setZoom(double)));
    connect(mHeaderBar, SIGNAL(exit()), this, SLOT(close()));
    connect(mHeaderBar, SIGNAL(showInfo()), this, SLOT(showInfo()));
    connect(mPdfInfo, SIGNAL(returnToViewer()), this, SLOT(hideInfo()));

    mZoomSlider->reset();
    mPdfWidget->open();
}

void Window::resizeEvent(QResizeEvent *)
{
    if(!mInitialPageFitRequested) {
        mInitialPageFitRequested = true;
    }
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
