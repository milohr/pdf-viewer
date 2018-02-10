#ifndef PDFWIDGET_H
#define PDFWIDGET_H

#include <QWidget>
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

    /**
     * @return Get pointer to internal Poppler document, used by the pdf info widget.
     * @note Since this class implements RAII, the pointer is guaranteed to be valid for the livetime of this instance.
     */
    Poppler::Document const * getPopplerDocument() const;

protected:

    /**
     * @brief Since widget size has not its final value at construction time,
     * it need to listen to new resizements to zoom to page fit.
     */
    virtual void resizeEvent(QResizeEvent *) override;

    /**
     * @brief Renders the current visible rect of the Pdf.
     * @param event Paint event, telling the rectangular extent to render.
     */
    virtual void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Listen to page grabbing start.
     */
    virtual void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Listen to page grabbing end.
     */
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    /**
     * @brief Listen to page grabbing.
     */
    virtual void mouseMoveEvent(QMouseEvent *event) override;

signals:

    /**
     * @brief Emitted when pages information changed.
     * @param count Total count of pages.
     * @param currentPage Current page index.
     */
    void pagesChanged(int const count, int const currentPage);

    /**
     * @brief Emitted when zoom changed.
     * @param zoom New value of zoom.
     */
    void zoomChanged(qreal const zoom);

public slots:

    /**
     * @brief Adjust zoom and panning to screen fit.
     */
    void screenFit();

    /**
     * @brief Adjust zoom and panning to page fit.
     */
    void pageFit();

    /**
     * @brief Set a specific zoom.
     * @param zoom Zoom to set.
     */
    void setZoom(qreal const zoom);

    /**
     * @brief Go back to previous page.
     * If already on first page, this call has no effect.
     */
    void prevPage();

    /**
     * @brief Go to next page.
     * If already on last page, this call has no effect.
     */
    void nextPage();

    /**
     * @brief Rotate page counter-clockwise around the currently visible viewport center.
     */
    void rotateLeft();

    /**
     * @brief Rotate page clockwise around the currently visible viewport center.
     */
    void rotateRight();

    /**
     * @brief Pan page by a specific x and y value.
     * @param x Amount of pan on x axis, in pixels.
     * @param y Amount of pan on y axis, in pixels.
     */
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
     * @brief Opens the current page denoted by an index and emit the pagesChanged() signal.
     */
    void open();

};

#endif // PDFWIDGET_H
