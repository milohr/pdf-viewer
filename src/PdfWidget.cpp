#include "PdfWidget.h"

PdfWidget::PdfWidget(QString const &path, QWidget *parent)
    : QWidget(parent)
    , mDocument(Poppler::Document::load(path))
    , mPath(path)
{
    if(nullptr == mDocument) throw std::runtime_error("Cannot open PDF");
    if(mDocument->isLocked()) throw std::runtime_error("PDF is locked");
    if(mDocument->numPages() == 0) throw std::runtime_error("PDF does not contain any pages");
    mDocument->setRenderHint(Poppler::Document::TextAntialiasing);

    setCursor(Qt::ArrowCursor);
}

const Poppler::Document *PdfWidget::getPopplerDocument() const
{
    return mDocument.data();
}

void PdfWidget::open()
{
    mPage.reset(mDocument->page(mPageIndex));
    pageFit();
    emit pagesChanged(mDocument->numPages(), mPageIndex);
}

void PdfWidget::resizeEvent(QResizeEvent *)
{
    open();
}

void PdfWidget::mousePressEvent(QMouseEvent *event)
{
    mLastMousePosition = QPoint(event->x(), event->y());
    setCursor(Qt::ClosedHandCursor);
}

void PdfWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mLastMousePosition = QPoint(event->x(), event->y());
    setCursor(Qt::ArrowCursor);
}

void PdfWidget::mouseMoveEvent(QMouseEvent *event)
{
    pan(event->x() - mLastMousePosition.x(), event->y() - mLastMousePosition.y());
    mLastMousePosition = QPoint(event->x(), event->y());
    update();
}

void PdfWidget::pan(int const x, int const y)
{
    switch(mPageRotation) {
    case 0:
        mPanning.rx() += x;
        mPanning.ry() += y;
        break;
    case 1:
        mPanning.rx() += y;
        mPanning.ry() -= x;
        break;
    case 2:
        mPanning.rx() -= x;
        mPanning.ry() -= y;
        break;
    case 3:
        mPanning.rx() -= y;
        mPanning.ry() += x;
        break;
    }
}

bool PdfWidget::isRotated() const
{
    return mPageRotation % 2 == 1;
}

void PdfWidget::paintEvent(QPaintEvent *event)
{
    (void)event;
    if(nullptr == mPage) return;

    // Image dimensions the image would have if fully rendered by Poppler:
    int imageHeight = getZoomedPageSize().height();
    int imageWidth = getZoomedPageSize().width();

    QRect pdfRect(0, 0, imageWidth, imageHeight);

    QTransform transform;
    transform.translate(width() / 2, height() / 2);
    transform.rotate(mPageRotation * 90);
    transform.translate(mPanning.x(), mPanning.y());
    transform.translate(-getZoomedPageSize().width() / 2, -getZoomedPageSize().height() / 2);

    QRect renderRect = transform.inverted(nullptr).mapRect(event->rect());
    QRect visiblePdf = renderRect.intersect(pdfRect);

    QImage subImage = mPage->renderToImage(physicalDpiX() * mZoom, physicalDpiY() * mZoom,
                                           visiblePdf.x(), visiblePdf.y(), visiblePdf.width(), visiblePdf.height()
                                  );

    // Create full image onto which the sub-image is drawn:
    QImage image = QImage(imageWidth, imageHeight, subImage.format());
    QPainter subImagePainter(&image);
    subImagePainter.drawImage(visiblePdf.x(), visiblePdf.y(), subImage);
    QImage scaled = image.scaledToHeight(imageHeight);

    // Paint image onto widget:
    QPainter painter(this);
    painter.setTransform(transform);
    painter.drawImage(0, 0, scaled);
    painter.drawRect(scaled.rect());
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
    if(getRotatedPageSize().height() > getRotatedPageSize().width()) {
        // Portrait orientation:
        mZoom = static_cast<qreal>(height()) / getRotatedPageSize().height() * autoZoomCoefficient();
    }
    else {
        // Landscape orientation:
        mZoom = static_cast<qreal>(width()) / getRotatedPageSize().width() * autoZoomCoefficient();
    }
    resetPanning();
    emit zoomChanged(mZoom);
}

void PdfWidget::pageFit()
{
    if(getRotatedPageSize().height() > getRotatedPageSize().width()) {
        // Portrait orientation:
        mZoom = static_cast<qreal>(width()) / getRotatedPageSize().width() * autoZoomCoefficient();

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
        mZoom = static_cast<qreal>(height()) / getRotatedPageSize().height() * autoZoomCoefficient();

        // Move to left page edge:
        mPanning.setX((getZoomedPageSize().width() / 2) - (width() / 2));
        mPanning.setY(0);
    }

    update();
    emit zoomChanged(mZoom);
}

void PdfWidget::setZoom(qreal const zoom)
{
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
