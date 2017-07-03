#include "tfp/views/SystemManipulator.hpp"
#include "tfp/models/System.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
SystemManipulator::SystemManipulator(QWidget* parent) :
    QWidget(parent)
{
}

// ----------------------------------------------------------------------------
void SystemManipulator::setSystem(System* system)
{
    system_ = system;
    onSetSystem();
}

// ----------------------------------------------------------------------------
System* SystemManipulator::createSystem()
{
    system_ = new System;
    return system_;
}

}