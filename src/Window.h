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

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(const QString &pdfPath, QWidget *parent = 0);

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    QWidget *mRootWidget = new QWidget;
    QHBoxLayout *mCenterLayout = new QHBoxLayout;
    QVBoxLayout *mVerticalLayout = new QVBoxLayout;
    PdfWidget *mPdfWidget;
    StatusBar *mStatusBar = new  StatusBar;
    HeaderBar *mHeaderBar;
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
