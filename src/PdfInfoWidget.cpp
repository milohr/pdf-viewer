#include "PdfInfoWidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include <QGridLayout>
#include <QPushButton>

PdfInfoWidget::PdfInfoWidget(const Poppler::Document *document)
{
    QWidget *info = new QWidget;
    QGridLayout *grid = new QGridLayout;
    info->setLayout(grid);
    grid->addWidget(new QLabel("Document Title:"), grid->rowCount(), 0);
    grid->addWidget(new QLabel(document->title().isEmpty() ? "No title specified" : document->title()), grid->rowCount() - 1, 1);
    grid->addWidget(new QLabel("Author:"), grid->rowCount(), 0);
    grid->addWidget(new QLabel(document->author().isEmpty() ? "No Author specified" : document->author()), grid->rowCount() - 1, 1);
    grid->addWidget(new QLabel("Creation Date:"), grid->rowCount(), 0);
    grid->addWidget(new QLabel(document->creationDate().toString()), grid->rowCount() - 1, 1);
    grid->addWidget(new QLabel("Last Modification Date:"), grid->rowCount(), 0);
    grid->addWidget(new QLabel(document->modificationDate().toString()), grid->rowCount() - 1, 1);

    QLabel *heading = new QLabel("<b>Document information:</b>");
    heading->setAlignment(Qt::AlignBottom);

    QPushButton *hideInfo = new QPushButton;
    hideInfo->setIcon(QIcon(":/assets/hide_info.svg"));
    connect(hideInfo, SIGNAL(clicked(bool)), this, SIGNAL(returnToViewer()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(heading);
    layout->addWidget(info);
    layout->addWidget(hideInfo, 0, Qt::AlignRight);
    layout->setAlignment(Qt::AlignCenter);
    setLayout(layout);
}
