#include "ZoomSlider.h"
#include "PdfWidget.h"

ZoomSlider::ZoomSlider()
{
    setOrientation(Qt::Vertical);
    setMinimum(PdfWidget::minZoom() * 100);
    setMaximum(PdfWidget::maxZoom() * 100);
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(zoomRouter(int)));
}

void ZoomSlider::setZoom(qreal const zoom)
{
    int const convertedZoom = zoom * 100;
    if(convertedZoom == value()) return;
    setValue(convertedZoom);
}

void ZoomSlider::zoomRouter(int const value)
{
    emit zoomChanged(value / 100.0);
}
