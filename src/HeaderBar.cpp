#include "HeaderBar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

HeaderBar::HeaderBar(const QString &pdfFile, QWidget *parent)
    : QWidget(parent)
{
    QLabel *filename = new QLabel("<b>" + pdfFile + "</b>");

    QPushButton *exit = new QPushButton;
    exit->setIcon(QIcon(":/assets/exit.svg"));

    QPushButton *showInfo = new QPushButton;
    showInfo->setIcon(QIcon(":/assets/show_info.svg"));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(showInfo);
    layout->addWidget(filename, 1, Qt::AlignCenter);
    layout->addWidget(exit);
    setLayout(layout);
    layout->setMargin(0);
    setContentsMargins(5, 2, 5, 2);

    connect(exit, SIGNAL(clicked(bool)), this, SIGNAL(exit()));
    connect(showInfo, SIGNAL(clicked(bool)), this, SIGNAL(showInfo()));
}
