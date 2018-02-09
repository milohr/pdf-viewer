#ifndef ZOOMSLIDER_H
#define ZOOMSLIDER_H

#include <QSlider>

/**
 * @brief A vertical slider to control the zoom level.
 */
class ZoomSlider
        : public QSlider
{
    Q_OBJECT

public:
    ZoomSlider();

signals:

    /**
     * @brief zoomChanged Emitted when zoom level was changed.
     * @param zoom New zoom, [0;1].
     */
    void zoomChanged(double zoom);

public slots:

    /**
     * @brief setZoom Set current zoom level, used e.g. when page-fit is requested.
     * @param zoom New zoom, [0;1].
     */
    void setZoom(double zoom);

private slots:

    /// Internally used to convert the zoom value.
    void zoomRouter(int value);
};

#endif // ZOOMSLIDER_H
