#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QFileInfo>

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
    explicit Window(QString const &pdfPath, QWidget *parent = 0);

private:

    /**
     * Widget displaying the Pdf file.
     */
    PdfWidget *mPdfWidget;

    /**
     * Slider to control zoom level.
    */
    ZoomSlider *mZoomSlider = new ZoomSlider;

    /**
     * Widget displaying Pdf file information. Initially hidden.
    */
    PdfInfoWidget *mPdfInfo;

public slots:

    /**
     * Handles keyboard input to control the viewer.
    */
    virtual void keyPressEvent(QKeyEvent *event) override;

    /**
     * Shows the info widget on top of the pdf widget.
    */
    void showInfo();

    /**
     * Hides the info widget.
    */
    void hideInfo();

};

#endif // WINDOW_H
