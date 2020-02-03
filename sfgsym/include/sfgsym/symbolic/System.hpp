#pragma once

#include "sfgsym/config.hpp"
#include "sfgsym/util/RefCounted.hpp"

namespace sfgsym {

class SymbolicTransferFunction;

/*!
 * @brief The interface for building and interacting with dynamic systems.
 * This is what all of the plugins primarily interact with.
 *
 * All systems in TFP are stored in a symbolic state space representation (see
 * SymbolicStateSpace). However, System also caches intermediate results such
 * as symbolic transfer functions, numberic transfer functions (poles & zeros,
 * or polynomial coefficients), partial fraction expansion results, and some
 * metadata such as interesting time and frequency intervals or recommended
 * minimum number of datapoints.
 *
 * The computations depend on each other as follows:
 *                           _______                        -------------
 *                          | DPSFG |                                   ^
 *                          |_______|                                   |
 *                              |                                       |
 *                    __________v___________                            |
 *                   | Symbolic State Space |                        Symbolic
 *                   |______________________|                         Stage
 *                     /        |         \                             |
 *          __________v_______  |   _______v___________                 |
 *        |  Symbolic Matrix  | |  |  Symbolic State   |                |
 *        | Transfer Function | |  | Transition Matrix |                |
 *        |___________________| |  |___________________|                v
 *          |                   |                 |         -------------
 *  ________v________     ______v______    _______v__________           ^
 * | Matrix Transfer |   | State Space |  | State Transition |          |
 * |    Function     |   |_____________|  |      Matrix      |          |
 * |_________________|          |         |__________________|      Numerical
 *                  \    _______v_______      /                       Stage
 *                   \ /| Poles & Zeros |    /                  (Frequency Domain)
 *                    x |_______________|   /                           |
 *                   / \         |         /                            |
 *     _____________v   v________v________v                             |
 *    |  Interesting | | Partial Fraction |                             v
 *    |   Intervals  | |     Expansion    |                 -------------
 *    |______________| |__________________|                             ^
 *                         |          |                                 |
 *            _____________v___     __v_______________              Numerical
 *           | Impulse & Step  |   | Inverse Laplace  |               Stage
 *           | Response Matrix |   | State Transition |           (Time Domain)
 *           |_________________|   |      Matrix      |                 |
 *                                 |__________________|                 v
 *                                                          -------------
 *
 * Why symbolic? Why not make everything numerical? Two major reasons:
 *   1) It's cool
 *   2) When the user starts tweaking parameters, we don't have to recompute
 *      everything. We instead have a list of precomputed expressions that
 *      simply need to be re-evaluated as the parameters are changed. This
 *      is an effort to keep the UI responsive, among other things.
 *
 * When System::update() is called, any computations that were "dirtied" are
 * recomputed, causing all dependencies to be recomputed as well.
 *
 * Plugins can register as listeners to the results they are interested in,
 * and will get notified immediately when a new result is computed.
 */
class SFGSYM_PUBLIC_API System : public RefCounted
{
public:
    System(const std::string& name);
    ~System();

    void notifyParametersChanged();
    void notifyStructureChanged();
    void interestingFrequencyInterval(double* xStart, double* xEnd) const;
    void interestingTimeInterval(double* xStart, double* xEnd) const;
/*
    const SymbolicStateSpace& SymbolicStateSpace() const;
    const SymbolicMatrixTransferFunction& symbolicMatrixTransferFunction() const;
    const MatrixTransferFunction& matrixTransferFunction() const;
    const StateSpace& stateSpace() const;

    SymbolicStateSpace& symbolicStateSpace();
    SymbolicMatrixTransferFunction& symbolicMatrixTransferFunction();
    MatrixTransferFunction& matrixTransferFunction();
    StateSpace& stateSpace();

    ListenerDispatcher<SystemListener> dispatcher;*/

private:/*
    Graph                          graph_;
    SymbolicStateSpace             symbolicStateSpace_;
    SymbolicMatrixTransferFunction symbolicMatrixTF_;
    MatrixTransferFunction         matrixTF_;
    StateSpace                     stateSpace_;*/

    enum {
        GRAPH                = 0x01,
        SYMBOLIC_STATE_SPACE = 0x02,
        SYMBOLIC_MATRIX_TF   = 0x04,
        MATRIX_TF            = 0x08,
        STATE_SPACE          = 0x10,

        ALL                  = 0x1F
    } dirtyFlags_;
};

}
