#pragma once

#include <QDeclarativeItem>
#include <QDateTime>

#ifndef Q_NULLPTR
#define Q_NULLPTR NULL
#endif // Q_NULLPTR

namespace Poppler {
    class Document;
    class Page;
}

namespace pdf_viewer {

/**
 * Display a PDF file.
 */
class PdfViewer
        : public QDeclarativeItem
{

    Q_OBJECT

    Q_ENUMS(
            Status
            PageOrientation)

    /**
     * Document file path.
     */
    Q_PROPERTY(
            QString source
            READ source
            WRITE setSource
            NOTIFY sourceChanged)

    /**
     * Page number.
     */
    Q_PROPERTY(
            int pageNumber
            READ pageNumber
            WRITE setPageNumber
            NOTIFY pageNumberChanged)

    /**
     * Document status.
     */
    Q_PROPERTY(
            Status status
            READ status
            NOTIFY statusChanged)

    /**
     * Document status as message string.
     */
    Q_PROPERTY(
            QString statusMessage
            READ statusMessage
            NOTIFY statusChanged)

    /**
     * Document title.
     */
    Q_PROPERTY(
            QString documentTitle
            READ documentTitle
            NOTIFY sourceChanged)

    /**
     * Document author.
     */
    Q_PROPERTY(
            QString documentAuthor
            READ documentAuthor
            NOTIFY sourceChanged)

    /**
     * Document creator.
     */
    Q_PROPERTY(
            QString documentCreator
            READ documentCreator
            NOTIFY sourceChanged)

    /**
     * Document creation date.
     */
    Q_PROPERTY(
            QDateTime documentCreationDate
            READ documentCreationDate
            NOTIFY sourceChanged)

    /**
     * Document modification date.
     */
    Q_PROPERTY(
            QDateTime documentModificationDate
            READ documentModificationDate
            NOTIFY sourceChanged)

    /**
     * Page orientation.
     */
    Q_PROPERTY(
            PageOrientation pageOrientation
            READ pageOrientation
            WRITE setPageOrientation
            NOTIFY pageOrientationChanged)

    /**
     * Page panning.
     */
    Q_PROPERTY(
            QPointF pan
            READ pan
            WRITE setPan
            NOTIFY panChanged)

    /**
     * The pan at which the page would be centralized at fit zoom.
     */
    Q_PROPERTY(
            QPointF fitPan
            READ fitPan
            NOTIFY coverZoomChanged)

    /**
     * The pan at which the page should sit when cover zoom is requested.
     */
    Q_PROPERTY(
            QPointF coverPan
            READ coverPan
            NOTIFY coverZoomChanged)

    /**
     * Zoom.
     * @note Zoom and scale refer to the same term but from different views.
     * Scale is the internal value to which the page is scaled when rendered.
     * Zoom is the scalement value exposed to the user and a relative measurement.
     * A zoom of 1 is by definition the fit zoom and therefore does not depend
     * on page orientation or graphics item size. A zoom < 1 is not allowed.
     */
    Q_PROPERTY(
            qreal zoom
            READ zoom
            WRITE setZoom
            NOTIFY zoomChanged)

    /**
     * Maximum zoom. Minimum is 1 by definition.
     */
    Q_PROPERTY(
            qreal maxZoom
            READ maxZoom
            WRITE setMaxZoom
            NOTIFY maxZoomChanged)

    /**
     * The zoom at which the page would cover the whole graphics item.
     */
    Q_PROPERTY(
            qreal coverZoom
            READ coverZoom
            NOTIFY coverZoomChanged)

    /**
     * The zoom at which the page would fit exactly into the view.
     */
    Q_PROPERTY(
            qreal fitZoom
            READ fitZoom
            NOTIFY coverZoomChanged)

public:

    /**
     * The page status.
     */
    enum Status {
        NOT_OPEN,               ///< Initial state the viewer resides in, until the source is set
        OK,                     ///< Everything is okay. Implies that the document pointer is not Q_NULLPTR.
        CANNOT_OPEN_DOCUMENT,   ///< The document cannot be opened, e.g. the path is invalid or the file just doesn't exist
        NO_PAGES,               ///< The document has no pages to display
        DOCUMENT_IS_LOCKED      ///< A password is required to open the document
    };

    /**
     * The possible page orientations.
     */
    enum PageOrientation {
        ZERO_PI,                ///< 0°, initial state
        HALF_PI,                ///< 90°, counter-clockwise
        ONE_PI,                 ///< 180°
        ONE_HALF_PI             ///< 270°, counter-clockwise
    };

    /**
     * Construct a new PDF viewer item.
     * @param parent Parent item.
     */
    PdfViewer(
            QDeclarativeItem * const parent = Q_NULLPTR
    );

    virtual ~PdfViewer();

    /**
     * @return The document file path.
     */
    QString
    source() const;

    /**
     * @return The current page number, zero based.
     */
    int
    pageNumber() const;

    /**
     * @return The document status.
     */
    Status
    status() const;

    /**
     * @return The document status as a message string.
     */
    QString
    statusMessage() const;

    /**
     * @return The document title.
     */
    QString
    documentTitle() const;

    /**
     * @return The document author.
     */
    QString
    documentAuthor() const;

    /**
     * @return The document creator.
     */
    QString
    documentCreator() const;

    /**
     * @return The document creation date.
     */
    QDateTime
    documentCreationDate() const;

    /**
     * @return The document modification date.
     */
    QDateTime
    documentModificationDate() const;

    /**
     * @return Current pan.
     */
    QPointF
    pan() const;

    /**
     * @return The pan at which the page would be centralized at fit zoom.
     */
    QPointF
    fitPan();

    /**
     * @return The pan at which the page should sit when cover zoom is requested.
     */
    QPointF
    coverPan();

    /**
     * @return Current zoom.
     */
    qreal
    zoom() const;

    /**
     * @return Maximum zoom.
     * @attention If smaller than cover-zoom, covering will not work properly.
     */
    qreal
    maxZoom() const;

    /**
     * @return The zoom at which the page would cover the whole graphics item.
     */
    qreal
    coverZoom() const;

    /**
     * @return The zoom at which the page would fit exactly into the view.
     */
    qreal
    fitZoom() const;

    /**
     * @return Current page orientation.
     */
    PageOrientation
    pageOrientation() const;

public slots:

    /**
     * Set the document file path and tries to open it.
     * Closes any document currently opened.
     * @param source File path to open.
     */
    void
    setSource(
            QString const &source
    );

    /**
     * Set the current page number.
     * Is clamped between 0 and the total page count.
     * @param pageNumber Number to set.
     */
    void
    setPageNumber(
            int pageNumber
    );

    /**
     * Set the page panning.
     * @param pan Pan to set.
     */
    void
    setPan(
            QPointF const pan
    );

    /**
     * Set the page zoom.
     * Is clamped between 1 and the maximum zoom set by `setMaxZoom()`.
     * @param zoom Zoom to set.
     */
    void
    setZoom(
            qreal zoom
    );

    /**
     * Set the maximum allowed zoom.
     * Must be greater or equal to 1.
     * @param maxZoom Zoom to set.
     */
    void
    setMaxZoom(
            qreal maxZoom
    );

    /**
     * Set the current page orientation.
     * @param orientation Orientation to set.
     */
    void
    setPageOrientation(
            PageOrientation orientation
    );

    /**
     * Paint item and renderbPDF.
     * @param painter Painter.
     * @param option Graphics options, ignored.
     * @param widget Widget, ignored.
     */
    virtual void
    paint(
            QPainter * const painter,
            QStyleOptionGraphicsItem const * const option,
            QWidget * const widget
    );

    /**
     * Rotate page clockwise.
     */
    Q_INVOKABLE void
    rotatePageClockwise();

    /**
     * Rotate page counter-clockwise.
     */
    Q_INVOKABLE void
    rotatePageCounterClockwise();

signals:

    /**
     * Emitted when page source changed.
     */
    void
    sourceChanged();

    /**
     * Emitted when page number changed.
     */
    void
    pageNumberChanged();

    /**
     * Emitted when document status changed.
     */
    void
    statusChanged();

    /**
     * Emitted when panning changed.
     */
    void
    panChanged();

    /**
     * Emitted when zoom changed.
     */
    void
    zoomChanged();

    /**
     * Emitted when page orientation changed.
     */
    void
    pageOrientationChanged();

    /**
     * Emitted when cover zoom changed.
     * This happens due to item resizement and page geometry changes.
     */
    void
    coverZoomChanged();

    /**
     * Emitted when maximum zoom changed.
     */
    void
    maxZoomChanged();

protected:

    /**
     * Grabs mouse focus.
     * @param event Mouse event.
     */
    virtual void
    mousePressEvent(
            QGraphicsSceneMouseEvent * const event
    );

    /**
     * Implements panning based on mouse movements.
     * @param event Mouse event.
     */
    virtual void
    mouseMoveEvent(
            QGraphicsSceneMouseEvent * const event
    );

    /**
     * A convinient way to toggle automatically between fit and cover zoom.
     * @param event Mouse event.
     */
    virtual void
    mouseDoubleClickEvent(
            QGraphicsSceneMouseEvent * const event
    );

private slots:

    /**
     * Dispatch a new render request.
     */
    void
    renderPdf();

private:

    /**
     * The document status.
     * The status will be set to OK as soon as a document is successfully opened.
     */
    Status mStatus;

    /**
     * The document file path.
     */
    QString mSource;

    /**
     * The internal document handle.
     */
    Poppler::Document *mDocument;

    /**
     * The internal page handle.
     */
    Poppler::Page const *mPage;

    /**
     * The current page number, which is zero based.
     */
    int mPageNumber;

    /**
     * Linear translation of the page.
     */
    QPointF mPan;

    /**
     * Page zoom.
     */
    qreal mZoom;

    /**
     * The upper zoom bound, which can be set by the user.
     */
    qreal mMaxZoom;

    /**
     * The current page orientation.
     * @note Do not use rotate() in QML to do rotation, since that does not refer to page rotation.
     * @see rotateClockwise(), rotateCounterClockwise()
     */
    PageOrientation mPageOrientation;

    void
    setStatus(
            Status const status
    );

    /**
     * @return Page orientation expressed in radians.
     */
    qreal
    pageRotation();

    /**
     * @return Size of page considering rotation, so width and height might be swapped.
     */
    QSizeF
    pageQuad() const;

    /**
     * @return A real page scalement, computed from the logical zoom.
     */
    qreal
    convertZoomToScale() const;

    /**
     * @return The scale to which the page is guaranteed to be completely observable. This is, by defintion, a zoom of 1.
     */
    qreal
    fitScale() const;

    /**
     * @return The scale to which the page is guaranteed to completely cover its bounding rect. This is, by defintion, a zoom > 1.
     */
    qreal
    coverScale() const;

private slots:

    /**
     * Centralize page if page zoom is fitting.
     * Used to initially centralize a page upon opening.
     */
    void
    centralizePage();

};

} // namespace pdf_viewer
