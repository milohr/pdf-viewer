#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QFileInfo>
#include <QDebug>

#include "PdfWidget.h"
#include "StatusBar.h"
#include "ZoomSlider.h"
#include "HeaderBar.h"
#include "PdfInfoWidget.h"

/**
 * @brief Provides window.
 */
class Window : public QMainWindow
{
    Q_OBJECT
public:

    /**
     * @brief Window Contruct a new window.
     * @param pdfPath Path to pdf to display.
     * @param parent Qt parent widget.
     */
    explicit Window(const QString &pdfPath, QWidget *parent = 0);

protected:

    virtual void resizeEvent(QResizeEvent *event) override;

private:

    PdfWidget *mPdfWidget;
    ZoomSlider *mZoomSlider = new ZoomSlider;
    PdfInfoWidget *mPdfInfo;
    bool mInitialPageFitRequested = false;

signals:

public slots:
    virtual void keyPressEvent(QKeyEvent *event) override;
    void showInfo();
    void hideInfo();
};

#endif // WINDOW_H
