#ifndef ZOOMSLIDER_H
#define ZOOMSLIDER_H

#include <QSlider>

class ZoomSlider
        : public QSlider
{
    Q_OBJECT

public:
    ZoomSlider();

    void reset();

signals:
    void zoomChanged(double zoom);

private slots:
    void setZoom(double zoom);
    void zoomRouter(int value);
};

#endif // ZOOMSLIDER_H
