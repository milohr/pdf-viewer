#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>
#include <QLabel>
#include <QPushButton>

class StatusBar
        : public QStatusBar
{
    Q_OBJECT

public:
    StatusBar();

private:
    QLabel *mPages = new QLabel;
    QPushButton *mScreenFit = new QPushButton;
    QPushButton *mPageFit = new QPushButton;
    QPushButton *mNext = new QPushButton;
    QPushButton *mPrev = new QPushButton;
    QPushButton *mRotateLeft = new QPushButton;
    QPushButton *mRotateRight = new QPushButton;

signals:
    void screenFit();
    void pageFit();
    void prevPage();
    void nextPage();
    void rotateLeft();
    void rotateRight();

public slots:
    void setPageInformation(int count, int currentPage);

};

#endif // STATUSBAR_H
