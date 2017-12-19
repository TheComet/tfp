#pragma once

#include "tfp/config.hpp"
#include "tfp/listeners/SystemListener.hpp"
#include "tfp/util/Reference.hpp"
#include "tfp/util/ListenerDispatcher.hpp"
#include "tfp/math/Graph.hpp"
#include "tfp/math/TransferFunction.hpp"
#include <QVector>

class QTreeWidgetItem;

namespace tfp {

class SymbolicTransferFunction;

/*!
 * @brief The interface for building and interacting with dynamic systems.
 * This is what all of the plugins primarily interact with.
 *
 * All systems in TFP are stored in a symbolic state space representation (see
 * SymbolicStateSpace). However, System also caches intermediate results such
 * as symbolic transfer functions, numberic transfer functions (poles & zeros,
 * or polynomial coefficients), partial fraction expansion results, and some
 * metadata such as interesting time and frequency intervals. The computations
 * depend on each other in the following order:
 *
 *
 *                                    _______
 *                                   | DPSFG |
 *                                   |_______|
 *                                       |
 *                             __________v___________
 *                            | Symbolic State Space |
 *                            |______________________|
 *                              /        |         \
 *                   __________v_______  |    _______v___________
 *                 |  Symbolic Matrix  | |   |  Symbolic State   |
 *                 | Transfer Function | |   | Transition Matrix |
 *                 |___________________| |   |___________________|
 *                   /             ______v______
 *     _____________v___          | State Space |
 *    | Matrix Transfer |         |_____________|
 *    |    Function     |                |
 *    |_________________|                v
 *                            ____________________________
 *                           | Partial Fraction Expansion |
 *
 *
 *
 * Often times, though, plugins will want to
 * manipulate the system through a different representation.
 */
class TFP_PUBLIC_API System : public RefCounted
{
public:
    System(const QString& name);
    ~System();

    void notifyParametersChanged();
    void notifyStructureChanged();
    void interestingFrequencyInterval(double* xStart, double* xEnd) const;
    void interestingTimeInterval(double* xStart, double* xEnd) const;

    const Graph& graph() const;
    const SymbolicTransferFunction& stf() const;
    const TransferFunction& tf() const;

    const QString& name() const;

    void setDataTree(QTreeWidgetItem* dataTree);
    QTreeWidgetItem* dataTree();

    ListenerDispatcher<SystemListener> dispatcher;

private:
    Graph graph_;
    TransferFunction tf_;

    QTreeWidgetItem* dataTree_;
    QTreeWidgetItem* tfItem_;
    QTreeWidgetItem* factorItem_;
    QTreeWidgetItem* polesItem_;
    QTreeWidgetItem* zerosItem_;
    QTreeWidgetItem* coeffBItem_;
    QTreeWidgetItem* coeffAItem_;
    QString name_;
};

}
