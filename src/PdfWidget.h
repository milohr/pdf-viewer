#ifndef PDFWIDGET_H
#define PDFWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QTransform>

namespace Poppler {
class Document;
class Page;
}

/**
 * @brief The PdfWidget rendering the PDF
 */
class PdfWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PdfWidget(const QString &path, QWidget *parent = nullptr);
    ~PdfWidget();

    const Poppler::Document *getPopplerDocument() const;

    void open();

protected:
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void pagesChanged(int count, int currentPage);
    void zoomChanged(double zoom);

public slots:
    void screenFit();
    void pageFit();
    void setZoom(double zoom);
    void prevPage();
    void nextPage();
    void rotateLeft();
    void rotateRight();
    void pan(int x, int y);

private:

    /// Internal document handle.
    Poppler::Document *mDocument;

    /// Internal page handle of current page.
    Poppler::Page *mPage = nullptr;

    /// Index of current page.
    int mPageIndex = 0;

    /// Path of file.
    const QString &mPath;

    /// Stored mouse position, used to implement grabbing.
    QPoint mLastMousePosition;

    /// Current panning.
    QPoint mPanning;

    /// Current scale factor.
    double mZoom = 1;

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

    QSizeF getPageSize() const;

    QSizeF getRotatedPageSize() const;

    /// @return Page size considering zoom.
    QSizeF getZoomedPageSize() const;

    /// @return Reset any page grabbing.
    void centralizePage();
    bool isRotated() const;

    static const double AUTO_ZOOM_COEFFICIENT;
};

#endif // PDFWIDGET_H
