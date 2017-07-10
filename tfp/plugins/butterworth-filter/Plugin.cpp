#include "ButterworthFilter.hpp"
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
    return plugin->registerSystemManipulator<ButterworthFilter>("Butterworth Filter");
}

PLUGIN_API void stop_plugin(Plugin* plugin)
{
}

}
