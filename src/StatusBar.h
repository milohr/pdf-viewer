#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>
#include <QLabel>
#include <QPushButton>

/**
 * @brief Implements the status bar on the lower window edge, providing navigation buttons.
 */
class StatusBar
        : public QStatusBar
{
    Q_OBJECT

public:

    /**
     * @brief StatusBar Construct a new status bar.
     */
    StatusBar();

private:

    /// Display current page and total number of pages.
    QLabel *mPages = new QLabel;

    /// Zoom to screen-fit:
    QPushButton *mScreenFit = new QPushButton;

    /// Zoom to page-fit:
    QPushButton *mPageFit = new QPushButton;

    /// Go to next page:
    QPushButton *mNext = new QPushButton;

    /// Go to previous page:
    QPushButton *mPrev = new QPushButton;

    /// Rotate page counter-clockwise:
    QPushButton *mRotateLeft = new QPushButton;

    /// Rotate page clockwise:
    QPushButton *mRotateRight = new QPushButton;

signals:

    /// Emitted when screen-fit is requested:
    void screenFit();

    /// Emitted when page-fit is requested:
    void pageFit();

    /// Emitted when previous page is requested:
    void prevPage();

    /// Emitted when next page is requested:
    void nextPage();

    /// Emitted when counter-clockwise page rotation is requested:
    void rotateLeft();

    /// Emitted when clockwise page rotation is requested:
    void rotateRight();

public slots:

    /**
     * @brief setPageInformation Updates page information
     * @param count Total number of pages.
     * @param currentPage Current page index, zero-based.
     */
    void setPageInformation(int const count, int const currentPage);

};

#endif // STATUSBAR_H
