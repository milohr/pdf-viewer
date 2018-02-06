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
    explicit HeaderBar(const QString &pdfFile, QWidget *parent = nullptr);

signals:
    /**
     * @brief Called when exit button clicked
     */
    void exit();

    void showInfo();

public slots:
};

#endif // HEADERBAR_H
