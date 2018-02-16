#pragma once

#include <QDeclarativeItem>

class PdfViewer
        : public QDeclarativeItem
{
    Q_OBJECT

public:

    PdfViewer(
            QDeclarativeItem * const parent = nullptr
    );

    ~PdfViewer() override = default;

};
