#include "ZoomSlider.h"

ZoomSlider::ZoomSlider()
{
    setOrientation(Qt::Vertical);
    setMinimum(10);
    setMaximum(400);
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(zoomRouter(int)));
}

void ZoomSlider::reset()
{
    setValue(100);
}

void ZoomSlider::setZoom(double zoom)
{
    int convertedZoom = zoom * 100;
    if(convertedZoom == value()) return;
    setValue(convertedZoom);
}

void ZoomSlider::zoomRouter(int value)
{
    emit zoomChanged(value / 100.0);
}
