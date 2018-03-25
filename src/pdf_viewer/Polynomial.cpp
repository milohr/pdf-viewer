#include "Polynomial.h"

namespace pdf_viewer {

void Polynomial::set(qreal const x0, qreal const y0, qreal const x1, qreal const y1)
{
    mX0 = x0;
    mY0 = y0;
    mA = (y1 - y0) / (x1 - x0) / (x1 - x0);
}

qreal Polynomial::operator()(qreal const x) const
{
    return mA * (x - mX0) * (x - mX0) + mY0;
}

} // namespace pdf_viewer
