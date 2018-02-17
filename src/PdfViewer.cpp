#include "PdfViewer.h"

#include <QPainter>
#include <QtCore/qmath.h>
#include <QApplication>

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
    if((pageNumber != mPageNumber) && (mStatus == OK))
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

QSizeF
PdfViewer::zoomedPageSize() const {
    return QSizeF{
        mZoom * pageSize().width(),
        mZoom * pageSize().height()
    };
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
        return height() / rotatedPageSize().height();
    }
    else
    {
        return width() / rotatedPageSize().width();
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
    (void)painter;
    qDebug() << "paint";

    painter->setPen(Qt::black);
    painter->drawRoundedRect(boundingRect(), 10, 10);
}
