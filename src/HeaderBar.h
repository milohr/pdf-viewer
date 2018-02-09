#ifndef HEADERBAR_H
#define HEADERBAR_H

#include <QWidget>

/**
 * @brief A header bar containing important information like the file name and the exit button.
 * It is display above the PDF.
 */
class HeaderBar : public QWidget
{
    Q_OBJECT

public:

    /**
     * @brief HeaderBar Conctruct a new header bar.
     * @param pdfFile File name of pdf without path.
     * @param parent Qt parent widget.
     */
    explicit HeaderBar(QString const &pdfFile, QWidget *parent = nullptr);

signals:

    /**
     * @brief Called when exit button clicked
     */
    void exit();

    /**
     * @brief showInfo File information is requested.
     */
    void showInfo();

};

#endif // HEADERBAR_H
