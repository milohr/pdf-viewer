#include "PdfViewer.h"

#include <QTimer>
#include <QPainter>
#include <QtCore/qmath.h>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include <poppler/qt4/poppler-qt4.h>

namespace pdf_viewer {

// Compare to floating points value up to a given precision.
// The `log10(precision)`th digit following the comma is guaranteed to be equal.
bool
equalReals(qreal const a, qreal const b, int const precision = 1000);

const qreal PdfViewer::SLIDE_MILLIS = 300.0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        PDF Viewer
/////////////////////////////////////////////////////////////////////////////////////////////////////////

PdfViewer::PdfViewer(QDeclarativeItem * const parent)
    : QDeclarativeItem(parent)
    , mStatus(NOT_OPEN)
    , mDocument(Q_NULLPTR)
    , mPage(Q_NULLPTR)
    , mPageNumber(-1)
    , mInfo(new PdfDocument(this))
    , mZoom(fitZoom())
    , mMaxZoom(6)
    , mPageOrientation(ZERO_PI)
    , mRenderTextAntiAliased(false)
    , mSliding(false)
    , mSlidingToPage(false)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MiddleButton);
    setAcceptTouchEvents(true);
    setAcceptHoverEvents(false);
    setSmooth(false); // Anti-aliasing is done by Poppler itself
    setFocus(true);

    connect(this, SIGNAL(widthChanged()), this, SLOT(allocateFramebuffer()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(allocateFramebuffer()));

    connect(this, SIGNAL(sourceChanged()), this, SLOT(requestRenderWholePdf()));
    connect(this, SIGNAL(widthChanged()), this, SLOT(requestRenderWholePdf()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(requestRenderWholePdf()));
    connect(this, SIGNAL(pageNumberChanged()), this, SLOT(requestRenderWholePdf()));
    connect(this, SIGNAL(pageOrientationChanged()), this, SLOT(requestRenderWholePdf()));
    connect(this, SIGNAL(zoomChanged()), this, SLOT(requestRenderWholePdf()));

    connect(this, SIGNAL(widthChanged()), this, SIGNAL(coverZoomChanged()));
    connect(this, SIGNAL(heightChanged()), this, SIGNAL(coverZoomChanged()));
    connect(this, SIGNAL(pageOrientationChanged()), this, SIGNAL(coverZoomChanged()));
    connect(this, SIGNAL(pageNumberChanged()), this, SIGNAL(coverZoomChanged()));

    connect(this, SIGNAL(coverZoomChanged()), this, SLOT(resetToFitPanIfFitZoom()));
    connect(this, SIGNAL(pageNumberChanged()), this, SLOT(resetPageViewToFit()));
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
        // Release page object:
        delete mPage;
        mPage = Q_NULLPTR;

        // Open new document:
        delete mDocument;
        mDocument = Poppler::Document::load(source);

        // Emit new source signal as soon as new document object is retrieved:
        mSource = source;
        mInfo->setInformation(mDocument->title(), mDocument->author(), mDocument->creator(), mDocument->creationDate(), mDocument->modificationDate());
        emit sourceChanged();
        emit infoChanged();

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
        mDocument->setRenderHint(Poppler::Document::TextAntialiasing, mRenderTextAntiAliased);

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
    if(OK != mStatus)
    {
        return;
    }

    pageNumber = qBound(0, pageNumber, mDocument->numPages() - 1);

    if((pageNumber != mPageNumber) || !mPage)
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

PdfDocument *PdfViewer::info() const
{
    return mInfo;
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

QSize
PdfViewer::scaledPageQuad() const
{
    return QSize(pageQuad().width(), pageQuad().height()) * computeScale();
}

QSize PdfViewer::viewport() const
{
    return QSize(qRound(width()), qRound(height()));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Panning
/////////////////////////////////////////////////////////////////////////////////////////////////////////

QPoint
PdfViewer::pan() const
{
    return mPan;
}

void
PdfViewer::setPan(
        QPoint pan
)
{
    static qreal lastZoom;
    if(mPan != pan || !equalReals(lastZoom, zoom()))
    {
        lastZoom = zoom();

        bool const scrolling = zoom() > fitZoom() && zoom() <= coverZoom();
        bool const hScrolling = scrolling && scaledPageQuad().width() > width();
        bool const vScrolling = scrolling && !hScrolling;

        // Restrict pan at certain zoom levels:
        if(equalReals(zoom(), fitZoom()))
        {
            // You cannot pan the page at all when at fit-zoom:
            pan = fitPan();
        }
        if(hScrolling)
        {
            // The page can only be horizontally scrolled:
            pan.setY(fitPan().y());
        }
        else if(vScrolling)
        {
            // The page can only be vertically scrolled:
            pan.setX(fitPan().x());
        }

        // Prevent scrolling over edges:
        if(zoom() > coverZoom() || vScrolling)
        {
            pan.setY(qMin(pan.y(), -zoomPan().y()));
            pan.setY(qMax(pan.y(), -zoomPan().y() - scaledPageQuad().height() + viewport().height()));
        }
        if(zoom() > coverZoom() || hScrolling)
        {
            pan.setX(qMin(pan.x(), -zoomPan().x()));
            pan.setX(qMax(pan.x(), -zoomPan().x() - scaledPageQuad().width() + viewport().width()));
        }

        int const dx = pan.x() - mPan.x();
        int const dy = pan.y() - mPan.y();
        int const w = viewport().width();
        int const h = viewport().height();

        mFramebuffer.scroll(dx, dy, mFramebuffer.rect(), Q_NULLPTR);

        mPan = pan;
        emit panChanged();

        if(dy > 0)
        {
            renderPdfIntoFramebuffer(QRect(0, 0, w, dy));
        }
        if(dy < 0)
        {
            renderPdfIntoFramebuffer(QRect(0, h + dy, w, -dy));
        }
        if(dx > 0)
        {
            renderPdfIntoFramebuffer(QRect(0, 0, dx, h));
        }
        if(dx < 0)
        {
            renderPdfIntoFramebuffer(QRect(w + dx, 0, -dx, h));
        }

        update();
    }
}

QPoint
PdfViewer::fitPan() const
{
    return QPoint(
        qRound(viewport().width() - pageQuad().width() * fitScale()) / 2,
        qRound(viewport().height() - pageQuad().height() * fitScale()) / 2
    );
}

QPoint
PdfViewer::coverPan() const
{
    return QPoint(
                (qRound(pageQuad().width() * (coverScale() - fitScale()))) / 2,
                (qRound(pageQuad().height() * (coverScale() - fitScale()))) / 2);
}

void
PdfViewer::resetToFitPanIfFitZoom()
{
    if(equalReals(zoom(), fitZoom()))
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

        setPan(pan());
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
/////////////////////        Render hints
/////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
PdfViewer::renderTextAntiAliased() const
{
    return mRenderTextAntiAliased;
}

void
PdfViewer::setRenderTextAntiAliased(
        bool const on
)
{
    if(mRenderTextAntiAliased != on) {
        mRenderTextAntiAliased = on;
        emit renderTextAntiAliasedChanged();

        if(mDocument)
        {
            mDocument->setRenderHint(Poppler::Document::TextAntialiasing, mRenderTextAntiAliased);
        }
    }
}

bool
PdfViewer::renderImageAntiAliased() const
{
    return mRenderImageAntiAliased;
}

void
PdfViewer::setRenderImageAntiAliased(
        bool const on
)
{
    if(mRenderImageAntiAliased != on) {
        mRenderImageAntiAliased = on;
        emit renderTextAntiAliasedChanged();

        if(mDocument)
        {
            mDocument->setRenderHint(Poppler::Document::Antialiasing, mRenderImageAntiAliased);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Background color
/////////////////////////////////////////////////////////////////////////////////////////////////////////

QColor
PdfViewer::backgroundColor() const
{
    return mBackgroundColor;
}

void
PdfViewer::setBackgroundColor(
        QColor const backgroundColor
)
{
    if(backgroundColor != mBackgroundColor)
    {
        mBackgroundColor = backgroundColor;
        emit backgroundColorChanged();
    }
}

void PdfViewer::resetPageViewToFit()
{
    setZoom(fitZoom());
    setPan(fitPan());
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
    setPageOrientation(static_cast<PageOrientation>(pageOrientation() + HALF_PI));
}

void
PdfViewer::rotatePageCounterClockwise()
{
    setPageOrientation(static_cast<PageOrientation>(pageOrientation() - HALF_PI));
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
PdfViewer::mouseReleaseEvent(
        QGraphicsSceneMouseEvent * const
)
{
    // Release mouse focus
    mSliding = false;
}

void
PdfViewer::mouseMoveEvent(
        QGraphicsSceneMouseEvent * const event
)
{
    if(mSlidingToPage) return;

    int const dx = qRound(event->pos().x() - event->lastPos().x());
    int const dy = qRound(event->pos().y() - event->lastPos().y());

    if(mZoom > 1.0 && !mSliding) {
        setPan(pan() + QPoint(dx, dy));
    }

    else if(dx != 0) {

        qDebug() << "sliding by" << dx;

        mSliding = true;
        mSlidingOffset.rx() += dx;

        static qint64 tOld = QDateTime::currentMSecsSinceEpoch();
        qint64 tNow = QDateTime::currentMSecsSinceEpoch();
        qint64 dt = tNow - tOld;

        //qDebug() << ((width() - scaledPageQuad().width()) / 2) + mSlidingOffset.x(); // => left hit
        //qDebug() << ((width() - scaledPageQuad().width()) / 2) - mSlidingOffset.x(); // => right hit

        if(((viewport().width() - scaledPageQuad().width()) / 2 + mSlidingOffset.x() < 0)
                && pageNumber() < mDocument->numPages() - 1) {
            // => Left hit
            mSlidingTStart = tNow;
            mSlidingVelocity = dt > 0.0 ? static_cast<qreal>(dx) / dt : 1.0;
            mSlidingToPage = true;
            mSlidingPolynomial.set(SLIDE_MILLIS, copysign(scaledPageQuad().width(), mSlidingVelocity), mSlidingVelocity);

            qDebug("slider to next page,  n=%dpx  m=%lfms  v=%lfpx/ms", scaledPageQuad().width(), SLIDE_MILLIS, mSlidingVelocity);

            mSlidingImage = mPage->renderToImage(
                        72.0 * computeScale(),
                        72.0 * computeScale(),
                        0,
                        0,
                        scaledPageQuad().width(),
                        scaledPageQuad().height(),
                        static_cast<Poppler::Page::Rotation>(pageOrientation()));

            startTimer(10);
        }

        tOld = tNow;

        mFramebuffer.scroll(dx, 0, mFramebuffer.rect(), Q_NULLPTR);

        update();
    }
}

void
PdfViewer::timerEvent(QTimerEvent *event)
{
    qint64 t = QDateTime::currentMSecsSinceEpoch() - mSlidingTStart;

    int shift = static_cast<int>(mSlidingPolynomial(t));

    QPainter painter(&mFramebuffer);
    painter.fillRect(0, 0, mFramebuffer.width(), mFramebuffer.height(), mBackgroundColor);
    painter.drawImage(shift, fitPan().y(), mSlidingImage);
    update();

    qDebug() << zoomPan().y();
    //qDebug("dt=%lldms  y=%dpx  maxY=%lfpx", t, shift, mSlidingPolynomial.n());
    //if(y > scaledPageQuad().width() /*|| (mSlidingCoeffecient > 0 ? dy > 0 : dy < 0)*/)
    if(t > SLIDE_MILLIS)
    {
        qDebug("Stop, took %lldms, v0=%lf", QDateTime::currentMSecsSinceEpoch() - mSlidingTStart, mSlidingVelocity);
        killTimer(event->timerId());
        mSlidingToPage = false;
    }
}

void
PdfViewer::mouseDoubleClickEvent(
        QGraphicsSceneMouseEvent * const
)
{
    if(equalReals(zoom(), fitZoom()))
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

QSize
PdfViewer::pageQuad() const
{
    if(!mPage)
    {
        return viewport();
    }
    return (mPageOrientation == ZERO_PI) || (mPageOrientation == ONE_PI)
            ? mPage->pageSize() // Take page size as is.
            : QSize(mPage->pageSize().height(), mPage->pageSize().width()); // Swap the page's width and height.
}

qreal
PdfViewer::computeScale() const
{
    return zoom() * fitScale(); // Remember, a zoom of 1 *is defined* as the fit scale.
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
PdfViewer::requestRenderWholePdf()
{
    mRenderRegion = QRect(0, 0, viewport().width(), viewport().height());
    update();
}

void
PdfViewer::allocateFramebuffer()
{
    if(mFramebuffer.isNull())
    {
        // No framebuffer has been allocated yet, so do that now:
        mFramebuffer = QPixmap(viewport().width(), viewport().height());
    }
    else
    {
        // Resize the current framebuffer instead of creating a new one:
        mFramebuffer = mFramebuffer.scaled(viewport().width(), viewport().height());
    }
}

QPoint PdfViewer::zoomPan() const
{
    return -QPoint(pageQuad().width(),pageQuad().height()) * (computeScale() - fitScale()) / 2;
}

QRect
PdfViewer::visiblePdfRect(
        QRect const viewportSpaceClip
) const
{
    // This rect is equally in size as the final Pdf which would appear on screen:
    QRect const pdfRect(0, 0, scaledPageQuad().width(), scaledPageQuad().height());

    // Transform mapping the document onto it's position on screen:
    QPoint const translation = pan() + zoomPan();

    // Now figure out which rect a currently visible to the user by inverting that transform matrix:
    return viewportSpaceClip.translated(-translation) & pdfRect;
}

void PdfViewer::renderPdfIntoFramebuffer(
        QRect const viewportSpaceRect
)
{
    // If no page is set currently, or no framebuffer is allocated yet, skip:
    if(mFramebuffer.isNull())
    {
        return;
    }

    QRect const visiblePdf = visiblePdfRect(viewportSpaceRect);

    QPainter painter(&mFramebuffer);
    painter.setPen(Qt::transparent);
    painter.setBrush(backgroundColor());
    painter.drawRect(viewportSpaceRect);

    // Painter should start drawing the image at the current clipped visible rect position:
    painter.translate(pan() + zoomPan() + visiblePdf.topLeft());

    if(!mPage)
    {
        return;
    }

    QImage const image = mPage->renderToImage(
                72.0 * computeScale(),
                72.0 * computeScale(),
                visiblePdf.x(),
                visiblePdf.y(),
                visiblePdf.width(),
                visiblePdf.height(),
                static_cast<Poppler::Page::Rotation>(pageOrientation()));

    painter.drawImage(0, 0, image);
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
    // Clean render regions:
    mRenderRegion = QRect();

    painter->drawPixmap(0, 0, mFramebuffer);
}

} // namespace pdf_viewer
