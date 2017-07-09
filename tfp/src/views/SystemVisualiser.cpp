#include "tfp/views/SystemVisualiser.hpp"
#include "tfp/models/System.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
SystemVisualiser::SystemVisualiser(QWidget* parent) :
    SystemManipulator(parent)
{
}

// ----------------------------------------------------------------------------
SystemVisualiser::~SystemVisualiser()
{
    if (system_ != NULL)
        system_->dispatcher.removeListener(this);
}

// ----------------------------------------------------------------------------
void SystemVisualiser::setSystem(System* system)
{
    if (system_ != NULL)
    {
        system_->dispatcher.removeListener(this);
    }

    SystemManipulator::setSystem(system);

    if (system_ != NULL)
    {
        system_->dispatcher.addListener(this);
        onSystemStructureChanged();
        onSystemParametersChanged();
    }
}

}
