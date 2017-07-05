#pragma once

#include "tfp/config.hpp"
#include "tfp/listeners/SystemListener.hpp"
#include "tfp/math/TransferFunction.hpp"
#include "tfp/models/RefCounted.hpp"
#include "tfp/models/ListenerDispatcher.hpp"
#include <QVector>

class QTreeWidgetItem;

namespace tfp {

class System : public RefCounted,
               public TransferFunction<double>
{
public:
    System(QTreeWidgetItem* dataTree);
    ~System();

    void notifyParametersChanged();
    void notifyStructureChanged();
    void getInterestingFrequencyInterval(double* xStart, double* xEnd) const;
    void getInterestingTimeInterval(double* xStart, double* xEnd) const;

    QTreeWidgetItem* dataTree();

    ListenerDispatcher<SystemListener> dispatcher;

private:
    QTreeWidgetItem* dataTree_;
    QTreeWidgetItem* tfItem_;
    QTreeWidgetItem* factorItem_;
    QTreeWidgetItem* polesItem_;
    QTreeWidgetItem* zerosItem_;
    QTreeWidgetItem* coeffBItem_;
    QTreeWidgetItem* coeffAItem_;
};

}
