#pragma once

#include <QDeclarativeItem>
#include <QDateTime>

namespace Poppler {
    class Document;
    class Page;
}

class PdfViewer
        : public QDeclarativeItem
{
    Q_OBJECT
    Q_ENUMS(Status PageOrientation)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int pageNumber READ pageNumber WRITE setPageNumber NOTIFY pageNumberChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusChanged)
    Q_PROPERTY(QString documentTitle READ documentTitle NOTIFY sourceChanged)
    Q_PROPERTY(QString documentAuthor READ documentAuthor NOTIFY sourceChanged)
    Q_PROPERTY(QString documentCreator READ documentCreator NOTIFY sourceChanged)
    Q_PROPERTY(QDateTime documentCreationDate READ documentCreationDate NOTIFY sourceChanged)
    Q_PROPERTY(QDateTime documentModificationDate READ documentModificationDate NOTIFY sourceChanged)
    Q_PROPERTY(QPoint pan READ pan WRITE setPan NOTIFY panChanged)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY(PageOrientation pageOrientation READ pageOrientation WRITE setPageOrientation NOTIFY pageOrientationChanged)

public:

    enum Status {
        NOT_OPEN,               ///< Initial state the viewer resides in, until the source is set
        OK,                     ///< Everything is okay. Implies that the document pointer is not null.
        CANNOT_OPEN_DOCUMENT,   ///< The document cannot be opened, e.g. the path is invalid or the file just doesn't exist
        NO_PAGES,               ///< The document has no pages to display
        DOCUMENT_IS_LOCKED      ///< A password is required to open the document
    };

    enum PageOrientation {
        ZERO_PI,                ///< 0°, initial state
        HALF_PI,                ///< 90°, counter-clockwise
        ONE_PI,                 ///< 180°
        ONE_HALF_PI             ///< 270°, counter-clockwise
    };

    PdfViewer(
            QDeclarativeItem * const parent = nullptr
    );

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

    QString
    documentTitle() const;

    QString
    documentAuthor() const;

    QString
    documentCreator() const;

    QDateTime
    documentCreationDate() const;

    QDateTime
    documentModificationDate() const;

    QPoint
    pan() const;

    void
    setPan(
            QPoint const pan
    );

    qreal
    zoom() const;

    void
    setZoom(
            qreal const zoom
    );

    PageOrientation
    pageOrientation() const;

    void
    setPageOrientation(
            PageOrientation const orientation
    );

    void
    paint(
            QPainter * const painter,
            QStyleOptionGraphicsItem const * const option,
            QWidget * const widget
    ) override;

signals:

    void
    sourceChanged();

    void
    pageNumberChanged();

    void
    statusChanged();

    void
    panChanged();

    void
    zoomChanged();

    void
    pageOrientationChanged();

private slots:

    void
    renderPdf();

private:

    Status mStatus{NOT_OPEN};
    QString mSource;
    Poppler::Document *mDocument{nullptr};
    Poppler::Page const *mPage{nullptr};
    int mPageNumber{-1};
    QPoint mPan;
    qreal mZoom{1};
    PageOrientation mPageOrientation{ZERO_PI};

    void
    setStatus(
            Status const status
    );

    /**
     * @return Whether the page has an orientation changing it's bounds, i.e. rotated by 0.5π or 1.5π.
     */
    bool
    hasRotatedOrientation() const;

    /**
     * @return Page orientation expressed in radians.
     */
    qreal
    pageRotation();

    /**
     * @return Size of page.
     */
    QSizeF
    pageSize() const;

    /**
     * @return Size of page considering rotation, so width and height might be swapped.
     */
    QSizeF
    rotatedPageSize() const;

    /**
     * @return A real page scalement, computed from the logical zoom.
     */
    qreal
    convertZoomToScale() const;

    qreal
    fitScale() const;

    qreal
    coverScale() const;

};
