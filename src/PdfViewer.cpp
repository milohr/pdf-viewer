#include "PdfViewer.h"

#include <QPainter>
#include <QtCore/qmath.h>
#include <QApplication>
#include <QDesktopWidget>

#include <QDebug>

#include <poppler/qt4/poppler-qt4.h>

/*
 * TODO:
 *
 *
 * - use QTransform::rotateRadians()
 * - a zoom of 1 means fit-zoom, which is the smallest allowed zoom
 * - take a look at scroll()
 *
 */


PdfViewer::PdfViewer(
        QDeclarativeItem * const parent
)
    : QDeclarativeItem{parent}
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setSmooth(true);

    connect(this, SIGNAL(widthChanged()), this, SLOT(renderPdf()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(renderPdf()));
    connect(this, SIGNAL(pageNumberChanged()), this, SLOT(renderPdf()));
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
        int const pageNumber
)
{
    if((mStatus == OK) && (pageNumber != mPageNumber) && (pageNumber >= 0) && (pageNumber < mDocument->numPages()))
    {
        mPageNumber = pageNumber;

        delete mPage;
        mPage = mDocument->page(mPageNumber);

        emit pageNumberChanged();
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

    default:
        return "Undefined status";
    }
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
    return mDocument->title();
}

QString
PdfViewer::documentAuthor() const
{
    return mDocument->author();
}

QString
PdfViewer::documentCreator() const
{
    return mDocument->creator();
}

QDateTime
PdfViewer::documentCreationDate() const
{
    return mDocument->creationDate();
}

QDateTime
PdfViewer::documentModificationDate() const
{
    return mDocument->modificationDate();
}

QPoint
PdfViewer::pan() const
{
    return mPan;
}

void
PdfViewer::setPan(
        QPoint const pan
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
        qreal const zoom
)
{
    if(mZoom != zoom && zoom >= 1)
    {
        mZoom = zoom;
        emit zoomChanged();
    }
}

PdfViewer::PageOrientation
PdfViewer::pageOrientation() const
{
    return mPageOrientation;
}

void
PdfViewer::setPageOrientation(
        PageOrientation const orientation
)
{
    if(mPageOrientation != orientation)
    {
        mPageOrientation = orientation;
        emit pageOrientationChanged();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Helper functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
PdfViewer::hasRotatedOrientation() const
{
    return (mPageOrientation == HALF_PI) || (mPageOrientation == ONE_HALF_PI);
}

qreal
PdfViewer::pageRotation()
{
    return mPageOrientation * M_PI;
}

QSizeF
PdfViewer::pageSize() const
{
    return mPage->pageSizeF();
}

QSizeF
PdfViewer::rotatedPageSize() const
{
    return !hasRotatedOrientation()
            ? mPage->pageSize()
            : QSizeF{pageSize().height(), pageSize().width()};
}

qreal
PdfViewer::convertZoomToScale() const
{
    return mZoom * fitScale();
}

qreal
PdfViewer::fitScale() const
{
    if(rotatedPageSize().height() > rotatedPageSize().width())
    {
        return (height() / rotatedPageSize().height());
    }
    else
    {
        return (width() / rotatedPageSize().width());
    }
}

qreal
PdfViewer::coverScale() const
{
    if(rotatedPageSize().height() > rotatedPageSize().width())
    {
        return width() / rotatedPageSize().width();
    }
    else
    {
        return height() / rotatedPageSize().height();
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

    qDebug() << "paint: ";

    qreal scale = convertZoomToScale();
    qDebug() << "scale: " << scale;

    // Image dimensions the image would have if fully rendered by Poppler:
    qreal const imageHeight = scale * pageSize().height();
    qreal const imageWidth = scale * pageSize().width();

    // A rect completely containing the Pdf:
    QRectF const pdfRect{0, 0, imageWidth, imageHeight};

    // Calculate a transformation matrix based on rotation, scaling and grabbing:
    QTransform transform;
    transform.translate(width() / 2, height() / 2);
    transform.rotateRadians(pageRotation());
    transform.translate(mPan.x(), mPan.y());
    transform.translate(-imageWidth / 2, -imageHeight / 2);

    // Now figure out which rect a currently visible to the user by inverting that transform matrix:
    QRectF const rect{0, 0, boundingRect().width(), boundingRect().height()};
    QRectF const visiblePdf = transform.inverted(nullptr).mapRect(rect).intersected(pdfRect);

    qDebug() << "Visible rect:" << visiblePdf;

    QImage const image = mPage->renderToImage(
                72.0 * scale,
                72.0 * scale,
                visiblePdf.x(),
                visiblePdf.y(),
                visiblePdf.width(),
                visiblePdf.height());

    // Painter should start drawing the image at the current clipped visible rect position:
    transform.translate(visiblePdf.x(), visiblePdf.y());
    transform.translate(boundingRect().width(), 0);
    painter->setTransform(transform);

    // Draw rendered page itself:
    painter->drawImage(0, 0, image);

    // Draw page border:
    painter->setPen(Qt::black);
    painter->drawRect(QRect{0, 0, image.width() - 1, image.height() - 1});
}
