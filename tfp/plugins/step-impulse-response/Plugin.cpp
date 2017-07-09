#include "ImpulsePlot.hpp"
#include "StepPlot.hpp"
#include "tfp/util/PluginManager.hpp"

#if defined(SE_BUILDING)
#  define SE_API Q_DECL_EXPORT
#else
#  define SE_API Q_DECL_IMPORT
#endif

using namespace tfp;

extern "C" {

SE_API bool start_plugin(Plugin* plugin, DataTree* dataTree)
{
    if (plugin->registerSystemManipulator<ImpulsePlot>("Impulse Response") == false) return false;
    if (plugin->registerSystemManipulator<StepPlot>("Step Response") == false) return false;
    return true;
}

SE_API void stop_plugin(Plugin* plugin)
{
}

}
