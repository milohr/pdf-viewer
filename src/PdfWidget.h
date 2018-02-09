#ifndef PDFWIDGET_H
#define PDFWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QTransform>
#include <QScopedPointer>

// No forward-declare possible because of QScopedPointer
#include <poppler/qt4/poppler-qt4.h>

/**
 * @brief The PdfWidget rendering the PDF
 */
class PdfWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * @brief Construct a new Pdf display widget.
     * @param path Complete path of pdf to open.
     * @param parent Qt parent widget.
     * @throw std::runtime_error if file could not be opened.
     */
    explicit PdfWidget(QString const &path, QWidget *parent = nullptr);

    Poppler::Document const * getPopplerDocument() const;

    void open();

protected:

    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

signals:

    void pagesChanged(int const count, int const currentPage);
    void zoomChanged(qreal const zoom);

public slots:

    void screenFit();
    void pageFit();
    void setZoom(qreal const zoom);
    void prevPage();
    void nextPage();
    void rotateLeft();
    void rotateRight();
    void pan(int const x, int const y);

private:

    /**
     * Internal document handle.
    */
    QScopedPointer<Poppler::Document> mDocument;

    /**
     * Internal page handle of current page.
    */
    QScopedPointer<Poppler::Page> mPage;

    /**
     * Index of current page.
    */
    int mPageIndex = 0;

    /**
     * Path of file.
    */
    const QString &mPath;

    /**
     * Stored mouse position, used to implement grabbing.
    */
    QPoint mLastMousePosition;

    /**
     * Current panning.
    */
    QPoint mPanning;

    /**
     * Current scale factor.
     */
    qreal mZoom = 1;

    /**
     * @brief Current page rotation.
     * The actual rotation in degrees can be calculated by multipling this number by 90.
     * Internally, the value is clamped to 3, therfore 360° rotation is still  0.
     * Value | Meaning
     * ------|--------
     * 0     | No rotation
     * 1     | Rotation by 90°
     * 2     | Rotation by 180°
     * 3     | Rotation by 270°
     */
    int mPageRotation = 0;

    /**
     * @return Size of page.
     */
    QSizeF getPageSize() const;

    /**
     * @return Size of page considering rotation, so width and height might be swapped.
     */
    QSizeF getRotatedPageSize() const;

    /**
     * @return Page size considering zoom.
    */
    QSizeF getZoomedPageSize() const;

    /**
     * @return Reset any page grabbing.
    */
    void resetPanning();

    /**
     * @return True if page is rotated by 90° or 270°.
    */
    bool isRotated() const;

    /**
     * @return A number indicating an additional scale factor to automatic zoom,
     * so that a page does not cover the complete display.
     */
    constexpr static qreal autoZoomCoefficient()
    {
        return 0.95;
    }

};

#endif // PDFWIDGET_H
