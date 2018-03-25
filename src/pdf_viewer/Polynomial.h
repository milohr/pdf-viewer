#ifndef CONTROLLABLEPOLYNOMINAL_H
#define CONTROLLABLEPOLYNOMINAL_H

#include <QObject>

namespace pdf_viewer {

/*!
 * \class Polynomial
 * \brief Controllable 2nd order polynomial used for animation.
 *
 * The polynomial is constructed from four parameters: x0, y0, x1 and y1.
 * It guarantees to fulfill following requirements:
 * 1. f(x0) = y0
 * 2. f'(x0) = 0
 * 3. f(x1) = y1
 * 4. f'(x1) != 0 if y0 != y1
 *
 * In short, the function is flat at (x0|y0) at crosses (x1|y1).
 * If n is negative, the curve open at the bottom.
 */
class Polynomial
{

public:

    /*!
     * \brief Construct a new polynomial.
     * \param x0 Origin of curve.
     * \param y0 Origin of curve.
     * \param x1 Control point.
     * \param y1 Control point.
     * \attention x1 must differ from x0!
     * \note If y1 equals y0, the curve is a horizontal line at y = y0.
     */
    void set(qreal const x0, qreal const y0, qreal const x1, qreal const y1);

    /*!
     * \brief Evaluates the polynomial at x.
     * \param x X value to evaluate.
     * \return Y value.
     */
    qreal operator()(qreal const x) const;

private:

    qreal mA, mX0, mY0;

};

} // namespace pdf_viewer

#endif // CONTROLLABLEPOLYNOMINAL_H
