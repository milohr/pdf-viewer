#include "PdfWidget.h"

const double PdfWidget::AUTO_ZOOM_COEFFICIENT = 0.95;

PdfWidget::PdfWidget(const QString &path, QWidget *parent)
    : QWidget(parent)
    , mPath(path)
{
    mDocument = Poppler::Document::load(mPath);
    if(nullptr == mDocument) throw std::runtime_error("Cannot open PDF");
    if(mDocument->isLocked()) throw std::runtime_error("PDF is locked");
    if(mDocument->numPages() == 0) throw std::runtime_error("PDF does not contain any pages");
    mDocument->setRenderHint(Poppler::Document::TextAntialiasing);

    setCursor(Qt::OpenHandCursor);
}

PdfWidget::~PdfWidget()
{
    if(nullptr != mPage) delete mPage;
    delete mDocument;
}

const Poppler::Document *PdfWidget::getPopplerDocument() const
{
    return mDocument;
}

void PdfWidget::open()
{
    mPage = mDocument->page(mPageIndex);
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
    setCursor(Qt::OpenHandCursor);
}

void PdfWidget::mouseMoveEvent(QMouseEvent *event)
{
    pan(event->x() - mLastMousePosition.x(), event->y() - mLastMousePosition.y());
    mLastMousePosition = QPoint(event->x(), event->y());
    update();
}

void PdfWidget::pan(int x, int y)
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

QSize PdfWidget::getPdfImageSize() const {
    return {
        qRound(((physicalDpiY() * mZoom) / 72.0) * mPage->pageSize().width()),
        qRound(((physicalDpiX() * mZoom) / 72.0) * mPage->pageSize().height())
    };
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
    int imageHeight = getPdfImageSize().height();
    int imageWidth = getPdfImageSize().width();

    QRect pdfRect(0, 0, imageWidth, imageHeight);

    QTransform transform;
    transform.translate(width() / 2, height() / 2);
    transform.rotate(mPageRotation * 90);
    transform.translate(mPanning.x(), mPanning.y());
    transform.translate(-(getPageWidth() * mZoom) / 2, -(getPageHeight() * mZoom) / 2);

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

int PdfWidget::getPageWidth() const
{
    return (mPage->pageSize().width() / 72.0) * physicalDpiX();
}

int PdfWidget::getPageHeight() const
{
    return (mPage->pageSize().height() / 72.0) * physicalDpiY();
}

int PdfWidget::getRotatedPageWidth() const
{
    return isRotated() ? getPageHeight() : getPageWidth();
}

int PdfWidget::getRotatedPageHeight() const
{
    return isRotated() ? getPageWidth() : getPageHeight();
}

void PdfWidget::screenFit()
{
    if(getRotatedPageHeight() > getRotatedPageWidth()) {
        // Portrait orientation:
        mZoom = static_cast<double>(height()) / getRotatedPageHeight() * AUTO_ZOOM_COEFFICIENT;
    }
    else {
        // Landscape orientation:
        mZoom = static_cast<double>(width()) / getRotatedPageWidth() * AUTO_ZOOM_COEFFICIENT;
    }
    centralizePage();
    emit zoomChanged(mZoom);
}

void PdfWidget::pageFit()
{
    if(getRotatedPageHeight() > getRotatedPageWidth()) {
        // Portrait orientation:
        mZoom = static_cast<double>(width()) / getRotatedPageWidth() * AUTO_ZOOM_COEFFICIENT;

        // Move to top page edge:
        mPanning.setX(0);
        if(isRotated()) {
            mPanning.setY((getPdfImageSize().width() / 2) - (width() / 2));
        }
        else {
            mPanning.setY((getPdfImageSize().height() / 2) - (height() / 2));
        }
    }
    else {
        // Landscape orientation:
        mZoom = static_cast<double>(height()) / getRotatedPageHeight() * AUTO_ZOOM_COEFFICIENT;

        // Move to left page edge:
        mPanning.setX((getPdfImageSize().width() / 2) - (width() / 2));
        mPanning.setY(0);
    }

    update();
    emit zoomChanged(mZoom);
}

void PdfWidget::setZoom(double zoom)
{
    if(zoom == mZoom) return;

    double diff = zoom / mZoom;
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

void PdfWidget::centralizePage()
{
    mPanning.setX(0);
    mPanning.setY(0);
    update();
}
