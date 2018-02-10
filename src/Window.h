#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

class QKeyEvent;
class PdfWidget;
class PdfInfoWidget;
class ZoomSlider;

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
    PdfWidget * const mPdfWidget;

    /**
     * Widget displaying Pdf file information. Initially hidden.
    */
    PdfInfoWidget * const mPdfInfo;

    /**
     * Slider to control zoom level.
    */
    ZoomSlider * const mZoomSlider;

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
