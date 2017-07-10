#include "ImpulsePlot.hpp"
#include "StepPlot.hpp"
#include "tfp/util/PluginManager.hpp"

#if defined(PLUGIN_BUILDING)
#  define PLUGIN_API Q_DECL_EXPORT
#else
#  define PLUGIN_API Q_DECL_IMPORT
#endif

using namespace tfp;

extern "C" {

PLUGIN_API bool start_plugin(Plugin* plugin, DataTree* dataTree)
{
    if (plugin->registerSystemManipulator<ImpulsePlot>("Impulse Response") == false) return false;
    if (plugin->registerSystemManipulator<StepPlot>("Step Response") == false) return false;
    return true;
}

PLUGIN_API void stop_plugin(Plugin* plugin)
{
}

}
