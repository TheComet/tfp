#pragma once

#include "tfp/config.hpp"

#include "tfp/util/Reference.hpp"
#include "tfp/math/StateSpace.hpp"
#include "tfp/math/SymbolicMatrix.hpp"
#include "tfp/math/SymbolicTransferFunction.hpp"

namespace tfp {

class VariableTable;

/*!
 * @brief Provides methods to construct and manipulate dynamical systems using
 * the state space description.
 *
 * http://lpsa.swarthmore.edu/Representations/SysRepSS.html
 * http://web.mit.edu/2.14/www/Handouts/StateSpace.pdf
 * http://lpsa.swarthmore.edu/Representations/SysRepTransformations/TF2SS.html#Transfer_Function
 * http://lpsa.swarthmore.edu/Transient/TransMethSS.html
 * https://en.wikipedia.org/wiki/Jordan_matrix
 * https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-241j-dynamic-systems-and-control-spring-2011/readings/MIT6_241JS11_chap27.pdf
 *
 */
class SymbolicStateSpace
{
public:
    struct ABCD
    {
        ABCD() {}
        ABCD(const SymbolicMatrix& A, const SymbolicMatrix& B, const SymbolicMatrix& C, const SymbolicMatrix& D) :
            A(A), B(B), C(C), D(D) {}
        SymbolicMatrix A;
        SymbolicMatrix B;
        SymbolicMatrix C;
        SymbolicMatrix D;
    };

    SymbolicStateSpace();
    SymbolicStateSpace(const ABCD& abcd);

    /*!
     * @brief Fetches the matrices A, B, C and D in canonical state space form.
     *
     * For a general nth order transfer function
     *
     *          b0 + b1*s^1 + b2*s^2 + ... + bn*s^n
     *   T(s) = -----------------------------------
     *          a0 + a1*s^1 + a2*s^2 + ... + an*s^n
     *
     * The controllable conanical state space model is
     *
     *   q' = Aq + Bu
     *   y  = Cq + Du
     *
     * where
     *
     *       /  0       1       0      ...  0         \         / 0    \
     *       |  0       0       1      ...  0         |         | 0    |
     *   A = |  :       :       :      `.   :         |     B = | 0    |
     *       |  0       0       0      ...  1         |         | 0    |
     *       \ -a0/an  -a1/an  -a2/an  ... -a(n-1)/an /         \ 1/an /
     *
     *       / b0     - bn*a0/an     \ '  <--- This thing means transpose     >
     *       | b1     - bn*a1/an     |
     *   C = | b2     - bn*a2/an     |                      D = bn/an
     *       | :                     |
     *       \ b(n-1) - bn*a(n-1)/an /
     *
     * @return Returns the A, B, C and D matrices in controllable conanical form.
     */
    SymbolicStateSpace ccf() const;

    /*!
     * @brief Fetches the matrices A, B, C and D in canonical state space form.
     *
     * For a general nth order transfer function
     *
     *          b0 + b1*s^1 + b2*s^2 + ... + bn*s^n
     *   T(s) = -----------------------------------
     *          a0 + a1*s^1 + a2*s^2 + ... + an*s^n
     *
     * The controllable conanical state space model is
     *
     *   q' = Aq + Bu
     *   y  = Cq + Du
     *
     * where
     *
     *       / -a(n-1)/an  1      0   ...   0 \       / b(n-1) - bn*a(n-1)/an \
     *       | -a(n-1)/an  0      1   ...   0 |       | b(n-2) - bn*a(n-2)/an |
     *   A = | -   :       :      0   `.    : |   B = |  :                    |
     *       | -a1/an      0      :   ...   1 |       | b1 - bn*a1/an         |
     *       \ -a0/an     -a2/an  0   ...   0 /       \ b0 - bn*a0/an         /
     *
     *       / 1/an \ '  <--- This thing means transpose                      >
     *       | 0    |
     *   C = | 0    |                             D = bn/an
     *       | 0    |
     *       \ 0    /
     *
     * @return Returns the A, B, C and D matrices in controllable conanical form.
     */
    SymbolicStateSpace ocf() const;

    StateSpace ss(const VariableTable* vt=NULL) const;

    SymbolicTransferFunction tf() const;

private:
    ABCD abcd_;
};

}
