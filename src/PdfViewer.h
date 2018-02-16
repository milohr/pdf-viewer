#pragma once

#include <QDeclarativeItem>

namespace Poppler {
    class Document;
    class Page;
}

class PdfViewer
        : public QDeclarativeItem
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int pageNumber READ pageNumber WRITE setPageNumber NOTIFY pageNumberChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusChanged)

public:

    enum Status {
        NOT_OPEN,
        OK, ///< Everything is okay. Implies that the document pointer is not null.
        CANNOT_OPEN_DOCUMENT,
        NO_PAGES,
        DOCUMENT_IS_LOCKED
    };

    ~PdfViewer() override;

    QString
    source() const;

    void
    setSource(
            QString const &source
    );

    int
    pageNumber() const;

    void
    setPageNumber(
            int const pageNumber
    );

    Status
    status() const;

    QString
    statusMessage() const;

signals:

    void
    sourceChanged();

    void
    pageNumberChanged();

    void
    statusChanged();

private:

    Status mStatus{NOT_OPEN};
    QString mSource;
    Poppler::Document const *mDocument{nullptr};
    Poppler::Page const *mPage{nullptr};
    int mPageNumber{0};

    void
    setStatus(
            Status const status
    );

};
