#ifndef PDFINFOWIDGET_H
#define PDFINFOWIDGET_H

#include <QWidget>
#include <poppler/qt4/poppler-qt4.h>

class PdfInfoWidget : public QWidget
{
    Q_OBJECT

public:
    PdfInfoWidget(const Poppler::Document *document);

signals:
    void returnToViewer();
};

#endif // PDFINFOWIDGET_H
