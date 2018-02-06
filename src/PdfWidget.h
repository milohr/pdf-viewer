#ifndef PDFWIDGET_H
#define PDFWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QTransform>

#include <poppler/qt4/poppler-qt4.h>

/**
 * @brief The PdfWidget rendering the PDF
 */
class PdfWidget : public QWidget
{
    Q_OBJECT

private:
    Poppler::Document *mDocument;
    Poppler::Page *mPage = nullptr;
    int mPageIndex = 0;
    const QString &mPath;
    QPoint mLastMousePosition;
    QPoint mPanning;
    double mZoom = 1;
    int mPageRotation = 0;

    int getPageWidth() const;
    int getPageHeight() const;
    int getRotatedPageWidth() const;
    int getRotatedPageHeight() const;
    void centralizePage();
    QSize getPdfImageSize() const;
    bool isRotated() const;

    static const double AUTO_ZOOM_COEFFICIENT;

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
};

#endif // PDFWIDGET_H
