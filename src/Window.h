#pragma once

#include <QMainWindow>

class QKeyEvent;
class PdfWidget;

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
    explicit Window(
            QString const &pdfPath,
            QWidget *parent = nullptr
    );

private:

    /**
     * Widget displaying the Pdf file.
     */
    PdfWidget * const mPdfWidget;

public slots:

    /**
     * Handles keyboard input to control the viewer.
    */
    virtual void keyPressEvent(
            QKeyEvent * const event
    ) override;

};
