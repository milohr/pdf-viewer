#include "Polynomial.h"

namespace pdf_viewer {

void Polynomial::set(qreal const m, qreal const n, qreal const t, qreal const d)
{
    mM = m;
    mN = n;
    mA = (n - d) / (m - t) / (m - t);
    mD = d;
    mT = t;
}

qreal Polynomial::operator()(qreal const x) const
{
    return mA * (x - mT) * (x - mT) + mD;
}

qreal Polynomial::m() const
{
    return mM;
}

qreal Polynomial::n() const
{
    return mN;
}

} // namespace pdf_viewer
