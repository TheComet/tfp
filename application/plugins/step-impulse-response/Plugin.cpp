#include "ImpulsePlot.hpp"
#include "StepPlot.hpp"
#include "tfp/plugin/Plugin.hpp"

#if defined(PLUGIN_BUILDING)
#  define PLUGIN_API Q_DECL_EXPORT
#else
#  define PLUGIN_API Q_DECL_IMPORT
#endif

using namespace tfp;

extern "C" {

PLUGIN_API bool start_plugin(Plugin* plugin, DataTree* dataTree)
{
    if (plugin->registerTool<ImpulsePlot>(
        Plugin::VISUALISER,
        Plugin::LTI_SYSTEM_CONTINUOUS,
        "Impulse Response",
        "Alex Murray",
        "Calculate the impulse response of a transfer function.",
        "alex.murray@gmx.ch"

    ) == false) return false;

    if (plugin->registerTool<StepPlot>(
        Plugin::VISUALISER,
        Plugin::LTI_SYSTEM_CONTINUOUS,
        "Step Response",
        "Alex Murray",
        "Calculate the step response of a transfer function.",
        "alex.murray@gmx.ch"
    ) == false) return false;
    return true;
}

PLUGIN_API void stop_plugin(Plugin* plugin)
{
}

}
