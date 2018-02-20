#include "PdfViewer.h"

#include <QPainter>
#include <QtCore/qmath.h>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include <poppler/qt4/poppler-qt4.h>

/**
 * Compare to floating points value up to a given precision.
 * @param a First number.
 * @param b Second number.
 * @param precision The `log10(precision)`th digit following the comma is guaranteed to be equal.
 * @return Whether the two numbers are equal up to the given precision.
 */
static bool
equalReals(
        qreal const a,
        qreal const b,
        int const precision
)
{
    return qRound(a * precision) == qRound(b * precision);
}

PdfViewer::PdfViewer(
        QDeclarativeItem * const parent
)
    : QDeclarativeItem{parent}
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MiddleButton);
    setAcceptTouchEvents(true);
    setAcceptHoverEvents(false);
    setSmooth(false);
    setFocus(true);

    connect(this, SIGNAL(widthChanged()), this, SLOT(renderPdf()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(renderPdf()));
    connect(this, SIGNAL(pageNumberChanged()), this, SLOT(renderPdf()));
    connect(this, SIGNAL(panChanged()), this, SLOT(renderPdf()));
    connect(this, SIGNAL(zoomChanged()), this, SLOT(renderPdf()));
    connect(this, SIGNAL(pageOrientationChanged()), this, SLOT(renderPdf()));

    connect(this, SIGNAL(widthChanged()), this, SIGNAL(coverZoomChanged()));
    connect(this, SIGNAL(heightChanged()), this, SIGNAL(coverZoomChanged()));
    connect(this, SIGNAL(pageOrientationChanged()), this, SIGNAL(coverZoomChanged()));
    connect(this, SIGNAL(pageNumberChanged()), this, SIGNAL(coverZoomChanged()));
}

PdfViewer::~PdfViewer()
{
    // Release resources acquired from Poppler:
    delete mPage;
    delete mDocument;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Document loading
/////////////////////////////////////////////////////////////////////////////////////////////////////////

QString
PdfViewer::source() const
{
    return mSource;
}

void
PdfViewer::setSource(
        QString const &source
)
{
    if(source != mSource)
    {
        mSource = source;
        emit sourceChanged();

        // Open document:
        delete mDocument;
        mDocument = Poppler::Document::load(source);

        // Check whether document is valid:
        if(!mDocument)
        {
            setStatus(CANNOT_OPEN_DOCUMENT);
            return;
        }
        if(mDocument->isLocked())
        {
            setStatus(DOCUMENT_IS_LOCKED);
            return;
        }
        if(mDocument->numPages() == 0)
        {
            setStatus(NO_PAGES);
            return;
        }
        setStatus(OK);

        // Enable anti-aliased rendering in Poppler:
        mDocument->setRenderHint(Poppler::Document::TextAntialiasing);

        // Reset page number to zero:
        setPageNumber(0);
    }
}

int
PdfViewer::pageNumber() const
{
    return mPageNumber;
}

void
PdfViewer::setPageNumber(
        int pageNumber
)
{
    if(!mDocument)
    {
        return;
    }

    pageNumber = qBound(0, pageNumber, mDocument->numPages() - 1);
    if((mStatus == OK) && (pageNumber != mPageNumber))
    {
        mPageNumber = pageNumber;

        delete mPage;
        mPage = mDocument->page(mPageNumber);

        emit pageNumberChanged();
        emit coverZoomChanged();
    }
}

PdfViewer::Status
PdfViewer::status() const
{
    return mStatus;
}

QString
PdfViewer::statusMessage() const
{
    switch (mStatus) {

    case OK:
        return "Okay";

    case NOT_OPEN:
        return "No document opened";

    case NO_PAGES:
        return "Document has no pages";

    case CANNOT_OPEN_DOCUMENT:
        return "Unable to open document";

    case DOCUMENT_IS_LOCKED:
        return "Document is locked";
    }

    return "Undefined status";
}

void
PdfViewer::setStatus(
        PdfViewer::Status const status
)
{
    if(status != mStatus)
    {
        mStatus = status;
        emit statusChanged();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Property getter and setter
/////////////////////////////////////////////////////////////////////////////////////////////////////////

QString
PdfViewer::documentTitle() const
{
    return mDocument ? mDocument->title() : "";
}

QString
PdfViewer::documentAuthor() const
{
    return mDocument ? mDocument->author() : "";
}

QString
PdfViewer::documentCreator() const
{
    return mDocument ? mDocument->creator() : "";
}

QDateTime
PdfViewer::documentCreationDate() const
{
    return mDocument ? mDocument->creationDate() : QDateTime{};
}

QDateTime
PdfViewer::documentModificationDate() const
{
    return mDocument ? mDocument->modificationDate() : QDateTime{};
}

QPointF
PdfViewer::pan() const
{
    return mPan;
}

void
PdfViewer::setPan(
        QPointF const pan
)
{
    if(mPan != pan)
    {
        mPan = pan;
        emit panChanged();
    }
}

qreal
PdfViewer::zoom() const
{
    return mZoom;
}

void
PdfViewer::setZoom(
        qreal zoom
)
{
    zoom = qBound(1.0, zoom, mMaxZoom);
    if(!equalReals(mZoom, zoom, 1000))
    {
        mZoom = zoom;
        emit zoomChanged();
    }
}

qreal
PdfViewer::maxZoom() const
{
    return mMaxZoom;
}

void
PdfViewer::setMaxZoom(
        qreal maxZoom
)
{
    maxZoom = qMax(1.0, maxZoom);
    if(!equalReals(mMaxZoom, maxZoom, 1000))
    {
        mMaxZoom = maxZoom;
        emit maxZoomChanged();

        // Eventually re-clamp the current zoom:
        setZoom(zoom());
    }
}

PdfViewer::PageOrientation
PdfViewer::pageOrientation() const
{
    return mPageOrientation;
}

void
PdfViewer::setPageOrientation(
        PageOrientation orientation
)
{
    orientation = static_cast<PageOrientation>(orientation % (ONE_HALF_PI + 1)); // Two pi equals to zero pi
    while(orientation < 0)
    {
        orientation = static_cast<PageOrientation>(orientation + 4);
    }

    if(mPageOrientation != orientation)
    {
        mPageOrientation = orientation;
        emit pageOrientationChanged();
    }
}

qreal
PdfViewer::coverZoom() const
{
    return coverScale() / fitScale();
}

void
PdfViewer::rotatePageClockwise()
{
    setPageOrientation(static_cast<PageOrientation>(mPageOrientation + HALF_PI));
}

void
PdfViewer::rotatePageCounterClockwise()
{
    setPageOrientation(static_cast<PageOrientation>(mPageOrientation - HALF_PI));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Panning
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void
PdfViewer::mousePressEvent(
        QGraphicsSceneMouseEvent * const
)
{
    // Simply grab mouse focus
}

void
PdfViewer::mouseMoveEvent(
        QGraphicsSceneMouseEvent * const event
)
{
    auto const delta = event->pos() - event->lastPos();
    auto const page = pageQuad() * convertZoomToScale();
    qreal const panMargin = 0.9;

    setPan({qBound(
                  -page.width() * panMargin,
                  (mPan + delta).x(),
                  width() - page.width() * (1 - panMargin)),
            qBound(
                  -page.height() * panMargin,
                  (mPan + delta).y(),
                  height() - page.height() * (1 - panMargin))});

    renderPdf();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Helper functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////

qreal
PdfViewer::pageRotation()
{
    return mPageOrientation * M_PI_2;
}

QSizeF
PdfViewer::pageQuad() const
{
    return (mPageOrientation == ZERO_PI) || (mPageOrientation == ONE_PI)
            ? mPage->pageSize()
            : QSizeF(mPage->pageSize().height(), mPage->pageSize().width());
}

qreal
PdfViewer::convertZoomToScale() const
{
    return mZoom * fitScale();
}

qreal
PdfViewer::fitScale() const
{
    if(!mPage)
    {
        return 1;
    }

    qreal const pageWidth = pageQuad().width();
    qreal const pageHeight = pageQuad().height();
    qreal const pageAspect = pageWidth / pageHeight;

    if((width()) > height() * pageAspect)
    {
        return height() / pageHeight;
    }
    else
    {
        return width() / pageWidth;
    }
}

qreal
PdfViewer::coverScale() const
{
    if(!mPage)
    {
        return 1;
    }

    qreal const pageWidth = pageQuad().width();
    qreal const pageHeight = pageQuad().height();
    qreal const pageAspect = pageWidth / pageHeight;

    if((width()) > height() * pageAspect)
    {
        return width() / pageWidth;
    }
    else
    {
        return height() / pageHeight;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Pdf rendering
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void
PdfViewer::renderPdf()
{
    update();
}

void
PdfViewer::paint(
        QPainter * const painter,
        QStyleOptionGraphicsItem const * const,
        QWidget * const
)
{
    // If no page is set currently, skip:
    if(!mPage)
    {
        return;
    }

    qreal scale = convertZoomToScale();

    // Image dimensions the image would have if fully rendered by Poppler:
    qreal const imageHeight = scale * pageQuad().height();
    qreal const imageWidth = scale * pageQuad().width();

    // A rect completely containing the Pdf:
    QRectF const pdfRect{0, 0, imageWidth, imageHeight};

    // Calculate a transformation matrix based on rotation, scaling and grabbing:
    QTransform const transform{QTransform{}
            .translate(mPan.x(), mPan.y())
            //.translate((width() - imageWidth) / 2, (height() - imageHeight) / 2) // TODO: cleanup
    };

    // Now figure out which rect a currently visible to the user by inverting that transform matrix:
    QRectF const rect{0, 0, boundingRect().width(), boundingRect().height()};
    QRectF const visiblePdf = transform.inverted(nullptr).mapRect(rect).intersected(pdfRect);

    QImage const image = mPage->renderToImage(
                72.0 * scale,
                72.0 * scale,
                static_cast<int>(visiblePdf.x()),
                static_cast<int>(visiblePdf.y()),
                static_cast<int>(visiblePdf.width()),
                static_cast<int>(visiblePdf.height()),
                static_cast<Poppler::Page::Rotation>(mPageOrientation));

    // Painter should start drawing the image at the current clipped visible rect position:
    painter->setTransform(transform, true);
    painter->setTransform(QTransform{}.translate(visiblePdf.x(), visiblePdf.y()), true);

    // Draw rendered page itself:
    painter->drawImage(0, 0, image);

    // Draw page border:
    painter->setPen(Qt::black);
    painter->drawRect(QRect{0, 0, image.width() - 1, image.height() - 1});
}
