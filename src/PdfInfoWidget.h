#ifndef PDFINFOWIDGET_H
#define PDFINFOWIDGET_H

#include <QWidget>

namespace Poppler {
class Document;
}

/**
 * @brief Display Pdf file information.
 */
class PdfInfoWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * @brief PdfInfoWidget Construct a new info widget.
     * @param document Document whose properties to be displayed.
     */
    PdfInfoWidget(const Poppler::Document *document);

signals:

    /// Emitted when user wants to return to the pdf widget, since it's obscured by this widget.
    void returnToViewer();
};

#endif // PDFINFOWIDGET_H
