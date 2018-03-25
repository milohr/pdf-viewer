#ifndef CONTROLLABLEPOLYNOMINAL_H
#define CONTROLLABLEPOLYNOMINAL_H

#include <QObject>

namespace pdf_viewer {

/*!
 * \class Polynomial
 * \brief Controllable 3rd order polynomial used for animation.
 *
 * The polynomial is constructed from three parameters, m, n and g.
 * It guarentees to fulfill these requirements:
 * 1. f(0) = 0
 * 2. f'(0) = g
 * 3. f(m) = n
 * 4. f'(m) = 0
 *
 * In words, the function has the given gradient g at x = 0
 * and is flat at (m|n).
 *
 * If g is too steep, the function will exceed n before getting to m,
 * although the function still reaches (m|n) and is still flat at that point.
 */
class Polynomial
{

public:

    /*!
     * \brief Polynomial Construct a new invariant polynomial.
     * \param m M-Parameter.
     * \param n N-Parameter.
     * \param g Gradient at (0|0).
     * \attention n must not be 0!
     * \note m, n and g can also be negative. m and g can also be 0.
     */
    void set(qreal const m, qreal const n, qreal const g);

    /*!
     * \brief operator () Evaluates the polynomial at x.
     * \param x X value to evaluate.
     * \return Y value.
     */
    qreal operator()(qreal x);

    qreal m() const;
    qreal n() const;
    qreal g() const;

private:

    qreal mM, mN, mG;
    qreal mA, mB, mC;

};

} // namespace pdf_viewer

#endif // CONTROLLABLEPOLYNOMINAL_H
