#include <QLabel>
#include <QPushButton>

#include "StatusBar.h"

StatusBar::StatusBar()
    : mPages(new QLabel)
{
    auto *screenFit = new QPushButton;
    auto *pageFit = new QPushButton;
    auto *next = new QPushButton;
    auto *prev = new QPushButton;
    auto *rotateLeft = new QPushButton;
    auto *rotateRight = new QPushButton;

    addWidget(pageFit);
    addWidget(screenFit);
    addWidget(rotateLeft);
    addWidget(rotateRight);

    addWidget(new QWidget, 1);
    addWidget(mPages);
    addWidget(prev);
    addWidget(next);

    setSizeGripEnabled(false);

    prev->setIcon(QIcon(":/assets/prev.svg"));
    next->setIcon(QIcon(":/assets/next.svg"));
    rotateLeft->setIcon(QIcon(":/assets/rotate_left.svg"));
    rotateRight->setIcon(QIcon(":/assets/rotate_right.svg"));
    pageFit->setIcon(QIcon(":/assets/width-fit.svg"));
    screenFit->setIcon(QIcon(":/assets/height-fit.svg"));

    connect(screenFit, SIGNAL(clicked(bool)), this, SIGNAL(screenFit()));
    connect(pageFit, SIGNAL(clicked(bool)), this, SIGNAL(pageFit()));
    connect(prev, SIGNAL(clicked(bool)), this, SIGNAL(prevPage()));
    connect(next, SIGNAL(clicked(bool)), this, SIGNAL(nextPage()));
    connect(rotateLeft, SIGNAL(clicked(bool)), this, SIGNAL(rotateLeft()));
    connect(rotateRight, SIGNAL(clicked(bool)), this, SIGNAL(rotateRight()));

    setContentsMargins(10, 2, 10, 2);
}

void StatusBar::setPageInformation(int const count, int const currentPage)
{
    mPages->setText(QString("%1 / %2").arg(QString::number(currentPage + 1), QString::number(count)));
}
