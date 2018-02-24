#include "PdfViewer.h"

#include <QPainter>
#include <QtCore/qmath.h>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QPixmap>

#include <poppler/qt4/poppler-qt4.h>

namespace pdf_viewer {

// Compare to floating points value up to a given precision.
// The `log10(precision)`th digit following the comma is guaranteed to be equal.
bool
equalReals(
        qreal const a,
        qreal const b,
        int const precision = 1000
);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        PDF Viewer
/////////////////////////////////////////////////////////////////////////////////////////////////////////

PdfViewer::PdfViewer(
        QDeclarativeItem * const parent
)
    : QDeclarativeItem(parent)
    , mStatus(NOT_OPEN)
    , mDocument(Q_NULLPTR)
    , mPage(Q_NULLPTR)
    , mPageNumber(-1)
    , mZoom(fitZoom())
    , mMaxZoom(6)
    , mPageOrientation(ZERO_PI)
    , mFramebuffer(Q_NULLPTR)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MiddleButton);
    setAcceptTouchEvents(true);
    setAcceptHoverEvents(false);
    setSmooth(false); // Anti-aliasing is done by Poppler itself
    setFocus(true);

    connect(this, SIGNAL(widthChanged()), this, SLOT(setupFramebuffer()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(setupFramebuffer()));

    connect(this, SIGNAL(widthChanged()), this, SLOT(renderPdf()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(renderPdf()));
    connect(this, SIGNAL(pageNumberChanged()), this, SLOT(renderPdf()));
    connect(this, SIGNAL(pageOrientationChanged()), this, SLOT(renderPdf()));
    connect(this, SIGNAL(panChanged()), this, SLOT(renderPdf()));
    connect(this, SIGNAL(zoomChanged()), this, SLOT(renderPdf()));

    connect(this, SIGNAL(widthChanged()), this, SIGNAL(coverZoomChanged()));
    connect(this, SIGNAL(heightChanged()), this, SIGNAL(coverZoomChanged()));
    connect(this, SIGNAL(pageOrientationChanged()), this, SIGNAL(coverZoomChanged()));
    connect(this, SIGNAL(pageNumberChanged()), this, SIGNAL(coverZoomChanged()));

    connect(this, SIGNAL(coverZoomChanged()), this, SLOT(centralizePage()));
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
        if(0 == mDocument->numPages())
        {
            setStatus(NO_PAGES);
            return;
        }
        setStatus(OK);

        // Enable anti-aliased rendering in Poppler:
        mDocument->setRenderHint(Poppler::Document::Antialiasing);
        mDocument->setRenderHint(Poppler::Document::TextAntialiasing);
        mDocument->setRenderHint(Poppler::Document::TextHinting);
        mDocument->setRenderHint(Poppler::Document::TextSlightHinting);
        mDocument->setRenderHint(Poppler::Document::ThinLineShape);

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
/////////////////////        Document getter and setter
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
    return mDocument ? mDocument->creationDate() : QDateTime();
}

QDateTime
PdfViewer::documentModificationDate() const
{
    return mDocument ? mDocument->modificationDate() : QDateTime();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Panning
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        scroll(pan.x() - mPan.x(), pan.y() - mPan.y());

        mPan = pan;
        emit panChanged();
    }
}

QPointF
PdfViewer::fitPan()
{
    return QPointF(
        (width() - pageQuad().width() * fitScale()) / 2,
        (height() - pageQuad().height() * fitScale()) / 2
    );
}

QPointF
PdfViewer::coverPan()
{
    return QPointF(
                (pageQuad().width() * (coverScale() - fitScale())) / 2,
                (pageQuad().height() * (coverScale() - fitScale())) / 2);
}

void
PdfViewer::centralizePage()
{
    if(equalReals(mZoom, fitZoom()))
    {
        setPan(fitPan());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Zooming
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    zoom = qBound(fitZoom(), zoom, mMaxZoom);
    if(!equalReals(mZoom, zoom))
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
    maxZoom = qMax(fitZoom(), maxZoom);
    if(!equalReals(mMaxZoom, maxZoom))
    {
        mMaxZoom = maxZoom;
        emit maxZoomChanged();

        // Eventually re-clamp the current zoom:
        setZoom(zoom());
    }
}

qreal
PdfViewer::coverZoom() const
{
    return coverScale() / fitScale();
}

qreal PdfViewer::fitZoom() const
{
    return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Orientation
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void PdfViewer::zoomIn(
        qreal const factor
)
{
    setZoom(zoom() * factor);
}

void PdfViewer::zoomOut(
        qreal const factor
)
{
    setZoom(zoom() / factor);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Mouse interaction
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
    QPointF const delta = event->pos() - event->lastPos();
    QSizeF const page = pageQuad() * convertZoomToScale();
    qreal const panMargin = 0.9;

    setPan(QPointF(qBound(
                  -page.width() * panMargin,
                  (mPan + delta).x(),
                  width() - page.width() * (1 - panMargin)),
            qBound(
                  -page.height() * panMargin,
                  (mPan + delta).y(),
                  height() - page.height() * (1 - panMargin))));

    renderPdf();
}

void
PdfViewer::mouseDoubleClickEvent(
        QGraphicsSceneMouseEvent * const
)
{
    if(equalReals(mZoom, fitZoom()))
    {
        // Zoom to cover:
        setZoom(coverZoom());
        setPan(coverPan());
    }
    else
    {
        // Zoom to fit:
        setZoom(fitZoom());
        setPan(fitPan());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Helper functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////

qreal
PdfViewer::pageRotation()
{
    return mPageOrientation * M_PI_2; // Each step is equal to 0.5π
}

QSizeF
PdfViewer::pageQuad() const
{
    return (mPageOrientation == ZERO_PI) || (mPageOrientation == ONE_PI)
            ? mPage->pageSize() // Take page size as is.
            : QSizeF(mPage->pageSize().height(), mPage->pageSize().width()); // Swap the page's width and height.
}

qreal
PdfViewer::convertZoomToScale() const
{
    return mZoom * fitScale(); // Remember, a zoom of 1 *is defined* as the fit scale.
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
    qreal const pageAspectRatio = pageWidth / pageHeight;

    if(width() > height() * pageAspectRatio)
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
    qreal const pageAspectRatio = pageWidth / pageHeight;

    if(width() > height() * pageAspectRatio)
    {
        return width() / pageWidth;
    }
    else
    {
        return height() / pageHeight;
    }
}

bool
equalReals(
        qreal const a,
        qreal const b,
        int const precision
)
{
    return qRound(a * precision) == qRound(b * precision);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Pdf rendering
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void
PdfViewer::renderPdf()
{
    mRenderRegion = QRect(0, 0, qRound(width()), qRound(height()));
    update();
}

void PdfViewer::setupFramebuffer()
{
    delete mFramebuffer;
    mFramebuffer = new QPixmap(static_cast<int>(width()), static_cast<int>(height()));
    mFramebuffer->fill(Qt::transparent);
}

void PdfViewer::renderPdfIntoFramebuffer(
        QRect const rect
)
{
    // If no page is set currently, skip:
    if(!mPage)
    {
        return;
    }

    qreal const scale = convertZoomToScale();

    // This rect is equally in size as the final Pdf which would appear on screen:
    QRect const pdfRect(0, 0, qRound(scale * pageQuad().width()), qRound(scale * pageQuad().height()));

    // Transform mapping the document onto it's position on screen:
    QPoint const translation =
            QPoint(qRound(mPan.x()), qRound(mPan.y()))
            + QPoint(qRound((-pageQuad().width() * (scale - fitScale())) / 2),
                      qRound((-pageQuad().height() * (scale - fitScale())) / 2));

    // Now figure out which rect a currently visible to the user by inverting that transform matrix:
    QRect const visiblePdf = rect.translated(-translation) & pdfRect;

    // Painter should start drawing the image at the current clipped visible rect position:

    QPainter bufferPainter(mFramebuffer);
    bufferPainter.translate(translation + visiblePdf.topLeft());

    QImage const image = mPage->renderToImage(
                72.0 * scale,
                72.0 * scale,
                visiblePdf.x(),
                visiblePdf.y(),
                visiblePdf.width(),
                visiblePdf.height(),
                static_cast<Poppler::Page::Rotation>(mPageOrientation));

    bufferPainter.drawImage(0, 0, image);
}

void
PdfViewer::paint(
        QPainter * const painter,
        QStyleOptionGraphicsItem const * const,
        QWidget * const
)
{
    for(int i = 0; i < mRenderRegion.rectCount(); i++)
    {
        renderPdfIntoFramebuffer(mRenderRegion.rects()[i]);
    }

    painter->drawPixmap(0, 0, *mFramebuffer);

    // Draw page border:
    /*painter->setPen(Qt::black);
    painter->translate(translation + visiblePdf.topLeft());
    painter->drawRect(QRect(
                          0, 0,
                          static_cast<int>(visiblePdf.width()) - 1,
                          static_cast<int>(visiblePdf.height()) - 1));*/
}

} // namespace pdf_viewer
