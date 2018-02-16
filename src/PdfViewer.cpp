#include "PdfViewer.h"

#include <QDebug>

#include <poppler/qt4/poppler-qt4.h>

PdfViewer::~PdfViewer()
{
    // Release resources acquired from Poppler:
    delete mPage;
    delete mDocument;
}

QString
PdfViewer::source() const
{
    return mSource;
}

void
PdfViewer::setSource(
        QString const &source
)
{
    if(source != mSource)
    {
        mSource = source;
        emit sourceChanged();

        // Open document:
        delete mDocument;
        mDocument = Poppler::Document::load(source);

        // Check whether document is valid:
        if(!mDocument)
        {
            setStatus(CANNOT_OPEN_DOCUMENT);
            return;
        }
        if(mDocument->isLocked())
        {
            setStatus(DOCUMENT_IS_LOCKED);
            return;
        }
        if(mDocument->numPages() == 0)
        {
            setStatus(NO_PAGES);
            return;
        }
        setStatus(OK);

        // Reset page number to zero:
        setPageNumber(0);
    }
}

int
PdfViewer::pageNumber() const
{
    return mPageNumber;
}

void
PdfViewer::setPageNumber(
        int const pageNumber
)
{
    if((pageNumber != mPageNumber) && (mStatus == OK))
    {
        mPageNumber = pageNumber;
        emit pageNumberChanged();

        delete mPage;
        mPage = mDocument->page(mPageNumber);
    }
}

PdfViewer::Status
PdfViewer::status() const
{
    return mStatus;
}

QString
PdfViewer::statusMessage() const
{
    switch (mStatus) {

    case OK:
        return "Okay";
        break;

    case NOT_OPEN:
        return "No document opened";
        break;

    case NO_PAGES:
        return "Document has no pages";
        break;

    case CANNOT_OPEN_DOCUMENT:
        return "Unable to open document";
        break;

    case DOCUMENT_IS_LOCKED:
        return "Document is locked";
        break;

    default:
        return "undefined status";
        break;

    }
}

void
PdfViewer::setStatus(
        PdfViewer::Status const status
)
{
    if(status != mStatus)
    {
        mStatus = status;
        emit statusChanged();
    }
}
