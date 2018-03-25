#include "Polynomial.h"

namespace pdf_viewer {

void Polynomial::set(qreal const m, qreal const n, qreal const g)
{
    mM = m;
    mN = n;
    mG = g;
    mA = (g * m - 2 * n) / m / m / m;
    mB = (3 * n - 2 * g * m) / m / m;
    mC = g;
}

qreal Polynomial::operator()(qreal x)
{
    return mA * x * x * x + mB * x * x + mC * x;
}

qreal Polynomial::m() const
{
    return mM;
}

qreal Polynomial::n() const
{
    return mN;
}

qreal Polynomial::g() const
{
    return mG;
}

} // namespace pdf_viewer
