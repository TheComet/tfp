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
