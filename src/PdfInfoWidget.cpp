#include "PdfInfoWidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include <QGridLayout>
#include <QPushButton>

#include <poppler/qt4/poppler-qt4.h>

PdfInfoWidget::PdfInfoWidget(Poppler::Document const *document)
{
    QWidget * const info = new QWidget;
    QGridLayout * const grid = new QGridLayout;
    info->setLayout(grid);
    grid->addWidget(new QLabel("Document Title:"), grid->rowCount(), 0);
    grid->addWidget(new QLabel(document->title().isEmpty() ? "No title specified" : document->title()), grid->rowCount() - 1, 1);
    grid->addWidget(new QLabel("Author:"), grid->rowCount(), 0);
    grid->addWidget(new QLabel(document->author().isEmpty() ? "No Author specified" : document->author()), grid->rowCount() - 1, 1);
    grid->addWidget(new QLabel("Creation Date:"), grid->rowCount(), 0);
    grid->addWidget(new QLabel(document->creationDate().toString()), grid->rowCount() - 1, 1);
    grid->addWidget(new QLabel("Last Modification Date:"), grid->rowCount(), 0);
    grid->addWidget(new QLabel(document->modificationDate().toString()), grid->rowCount() - 1, 1);

    QLabel * const heading = new QLabel("<b>Document information:</b>");
    heading->setAlignment(Qt::AlignBottom);

    QPushButton * const hideInfo = new QPushButton;
    hideInfo->setIcon(QIcon(":/assets/hide_info.svg"));
    connect(hideInfo, SIGNAL(clicked(bool)), this, SIGNAL(returnToViewer()));

    QVBoxLayout * const layout = new QVBoxLayout;
    layout->addWidget(heading);
    layout->addWidget(info);
    layout->addWidget(hideInfo, 0, Qt::AlignRight);
    layout->setAlignment(Qt::AlignCenter);
    setLayout(layout);
}
