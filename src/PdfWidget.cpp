#include "PdfWidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QDebug>

#include <poppler/qt4/poppler-qt4.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Constructor and destructor
/////////////////////////////////////////////////////////////////////////////////////////////////////////

PdfWidget::PdfWidget(
        QString const &path,
        QWidget * const parent
)
    : QWidget(parent)
    , mDocument(Poppler::Document::load(path))
    , mPath(path)
{
    if(nullptr == mDocument) throw std::runtime_error("Cannot open PDF");
    if(mDocument->isLocked()) throw std::runtime_error("PDF is locked");
    if(mDocument->numPages() == 0) throw std::runtime_error("PDF does not contain any pages");
    mDocument->setRenderHint(Poppler::Document::TextAntialiasing);

    setCursor(Qt::ArrowCursor);

    open();
}

PdfWidget::~PdfWidget()
{
    delete mPage;
    delete mDocument;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Document setter and getter
/////////////////////////////////////////////////////////////////////////////////////////////////////////

qreal
PdfWidget::zoom() const
{
    return mZoom;
}

QString
PdfWidget::documentTitle() const
{
    return mDocument->title();
}

QString
PdfWidget::documentAuthor() const
{
    return mDocument->author();
}

QString
PdfWidget::documentCreator() const
{
    return mDocument->creator();
}

QDateTime
PdfWidget::documentCreationDate() const
{
    return mDocument->creationDate();
}

QDateTime
PdfWidget::documentModificationDate() const
{
    return mDocument->modificationDate();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////        Page navigation
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void PdfWidget::open()
{
    delete mPage;
    mPage = mDocument->page(mPageIndex);

    pageFit();
    emit pagesChanged(mDocument->numPages(), mPageIndex);
}

void PdfWidget::mousePressEvent(
        QMouseEvent * const event
)
{
    mLastMousePosition = QPoint(event->x(), event->y());
    setCursor(Qt::ClosedHandCursor);
}

void PdfWidget::mouseReleaseEvent(
        QMouseEvent * const event
)
{
    mLastMousePosition = QPoint(event->x(), event->y());
    setCursor(Qt::ArrowCursor);
}

void PdfWidget::mouseMoveEvent(
        QMouseEvent * const event
)
{
    pan(mLastMousePosition.x() - event->x(), mLastMousePosition.y() - event->y());
    mLastMousePosition = QPoint(event->x(), event->y());
    update();
}

void PdfWidget::pan(
        int const x,
        int const y
)
{
    switch(mPageRotation) {
    case 0:
        mPanning.rx() -= x;
        mPanning.ry() -= y;
        break;
    case 1:
        mPanning.rx() -= y;
        mPanning.ry() += x;
        break;
    case 2:
        mPanning.rx() += x;
        mPanning.ry() += y;
        break;
    case 3:
        mPanning.rx() += y;
        mPanning.ry() -= x;
        break;
    }
}

bool PdfWidget::isRotated() const
{
    return mPageRotation % 2 == 1;
}

void PdfWidget::paintEvent(
        QPaintEvent * const event
)
{
    // If no page is set currently, skip:
    if(nullptr == mPage) return;

    // Image dimensions the image would have if fully rendered by Poppler:
    int const imageHeight = getZoomedPageSize().height();
    int const imageWidth = getZoomedPageSize().width();

    // A rect completely containing the Pdf:
    QRect const pdfRect(0, 0, imageWidth, imageHeight);

    // Calculate a transformation matrix based on rotation, scaling and grabbing:
    QTransform transform;
    transform.translate(width() / 2, height() / 2);
    transform.rotate(mPageRotation * 90);
    transform.translate(mPanning.x(), mPanning.y());
    transform.translate(-imageWidth / 2, -imageHeight / 2);

    // Now figure out which rect a currently visible to the user by inverting that transform matrix:
    QRect const visiblePdf = transform.inverted(nullptr).mapRect(event->rect()).intersected(pdfRect);

    QImage const image = mPage->renderToImage(
                physicalDpiX() * mZoom,
                physicalDpiY() * mZoom,
                visiblePdf.x(),
                visiblePdf.y(),
                visiblePdf.width(),
                visiblePdf.height());

    // Paint image onto widget:
    QPainter painter(this);

    // Painter should start drawing the image at the current clipped visible rect position:
    transform.translate(visiblePdf.x(), visiblePdf.y());
    painter.setTransform(transform);

    // Draw rendered page itself:
    painter.drawImage(0, 0, image);

    // Draw page border:
    painter.drawRect(QRect(0, 0, image.width() - 1, image.height() - 1));
}

QSizeF PdfWidget::getPageSize() const
{
    return {
        (mPage->pageSize().width() / 72.0) * physicalDpiX(),
        (mPage->pageSize().height() / 72.0) * physicalDpiY()
    };
}

QSizeF PdfWidget::getRotatedPageSize() const
{
    return !isRotated() ? getPageSize() : QSizeF{getPageSize().height(), getPageSize().width()};
}

QSizeF PdfWidget::getZoomedPageSize() const {
    return {
        mZoom * getPageSize().width(),
        mZoom * getPageSize().height()
    };
}

void PdfWidget::screenFit()
{
    if(!isRotated()) {
        // Portrait orientation:
        mZoom = static_cast<qreal>(height()) / getRotatedPageSize().height();
        resetPanning();
    }
    else {
        // Landscape orientation:
        mZoom = static_cast<qreal>(height()) / getPageSize().width();
        mPanning.setX(0);

        // Pan page so start edge is visible, since that is usually the page start:
        mPanning.setY((getZoomedPageSize().height() - width()) / 2);
    }
    emit zoomChanged(mZoom);
}

void PdfWidget::pageFit()
{
    if(!isRotated()) {
        // Portrait orientation:
        mZoom = static_cast<qreal>(width()) / getPageSize().width();

        // Move to top page edge:
        mPanning.setX(0);
        if(isRotated()) {
            mPanning.setY((getZoomedPageSize().width() / 2) - (width() / 2));
        }
        else {
            mPanning.setY((getZoomedPageSize().height() / 2) - (height() / 2));
        }
    }
    else {
        // Landscape orientation:
        mZoom = static_cast<qreal>(width()) / getPageSize().height();

        // Move to left page edge:
        mPanning.setX(0);
        mPanning.setY(0);
    }

    update();
    emit zoomChanged(mZoom);
}

void PdfWidget::setZoom(
        qreal zoom
)
{
    zoom = qBound(minZoom(), zoom, maxZoom());
    if(zoom == mZoom) return;

    qreal const diff = zoom / mZoom;
    mZoom = zoom;
    mPanning.rx() *= diff;
    mPanning.ry() *= diff;
    update();

    emit zoomChanged(mZoom);
}

void PdfWidget::prevPage()
{
    if(0 == mPageIndex) return;
    mPageIndex--;
    open();
}

void PdfWidget::nextPage()
{
    if(mDocument->numPages() <= mPageIndex + 1) return;
    mPageIndex++;
    open();
}

void PdfWidget::rotateLeft()
{
    mPageRotation = mPageRotation > 0 ? (mPageRotation - 1) % 4 : 3;
    update();
}

void PdfWidget::rotateRight()
{
    mPageRotation = (mPageRotation + 1) % 4;
    update();
}

void PdfWidget::resetPanning()
{
    mPanning.setX(0);
    mPanning.setY(0);
    update();
}
