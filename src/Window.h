#pragma once

#include <QMainWindow>

class QKeyEvent;

/**
 * @brief Provides window.
 */
class Window : public QMainWindow
{
    Q_OBJECT

public:

    explicit Window(
            QWidget *parent = nullptr
    );

private:

public slots:

    /**
     * Handles keyboard input to control the viewer.
    */
    /*virtual void keyPressEvent(
            QKeyEvent * const event
    ) override;*/

};
