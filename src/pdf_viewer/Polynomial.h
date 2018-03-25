#ifndef CONTROLLABLEPOLYNOMINAL_H
#define CONTROLLABLEPOLYNOMINAL_H

#include <QObject>

namespace pdf_viewer {

/*!
 * \class Polynomial
 * \brief Controllable 2nd order polynomial used for animation.
 *
 * The polynomial is constructed from four parameters, m, n, d and t.
 * It guarentees to fulfill following requirements:
 * 1. f(0) = 0
 * 2. f'(0) = 0
 * 3. f(m) = n
 *
 * In words, the function is flat at (0|0) at crosses (m|n).
 * If n is negative, the curve open at the bottom.
 */
class Polynomial
{

public:

    /*!
     * \brief Polynomial Construct a new polynomial.
     * \param m M-Parameter.
     * \param n N-Parameter.
     * \attention n must not be 0!
     * \note m and n and g can also be negative.
     * \note If n is 0, the curve is a horizontal line at y = 0.
     */
    void set(qreal const x0, qreal const y0, qreal const x1, qreal const y1);

    /*!
     * \brief operator () Evaluates the polynomial at x.
     * \param x X value to evaluate.
     * \return Y value.
     */
    qreal operator()(qreal const x) const;

private:

    qreal mA, mX0, mY0;

};

} // namespace pdf_viewer

#endif // CONTROLLABLEPOLYNOMINAL_H
