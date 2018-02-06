#include "StatusBar.h"

StatusBar::StatusBar()
{
    addWidget(mPageFit);
    addWidget(mScreenFit);
    addWidget(mRotateLeft);
    addWidget(mRotateRight);

    addWidget(new QWidget(), 1);
    addWidget(mPages);
    addWidget(mPrev);
    addWidget(mNext);

    setSizeGripEnabled(false);

    mPrev->setIcon(QIcon(":/assets/prev.svg"));
    mNext->setIcon(QIcon(":/assets/next.svg"));
    mRotateLeft->setIcon(QIcon(":/assets/rotate_left.svg"));
    mRotateRight->setIcon(QIcon(":/assets/rotate_right.svg"));
    mScreenFit->setText("&Screen-Fit");
    mPageFit->setText("&Page-Fit");

    connect(mScreenFit, SIGNAL(clicked(bool)), this, SIGNAL(screenFit()));
    connect(mPageFit, SIGNAL(clicked(bool)), this, SIGNAL(pageFit()));
    connect(mPrev, SIGNAL(clicked(bool)), this, SIGNAL(prevPage()));
    connect(mNext, SIGNAL(clicked(bool)), this, SIGNAL(nextPage()));
    connect(mRotateLeft, SIGNAL(clicked(bool)), this, SIGNAL(rotateLeft()));
    connect(mRotateRight, SIGNAL(clicked(bool)), this, SIGNAL(rotateRight()));

    setContentsMargins(10, 2, 10, 2);
}

void StatusBar::setPageInformation(int count, int currentPage)
{
    mPages->setText(QString("%1 / %2").arg(QString::number(currentPage + 1), QString::number(count)));
}
