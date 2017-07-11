#include "DPSFG.hpp"
#include "tfp/util/Plugin.hpp"

#if defined(PLUGIN_BUILDING)
#  define PLUGIN_API Q_DECL_EXPORT
#else
#  define PLUGIN_API Q_DECL_IMPORT
#endif

using namespace tfp;

extern "C" {

PLUGIN_API bool start_plugin(Plugin* plugin, DataTree* dataTree)
{
    return plugin->registerTool<DPSFG>(
        Plugin::GENERATOR,
        Plugin::LTI_SYSTEM_CONTINUOUS,
        "DPSFG",
        "Alex Murray",
        "Draw driving-point signal flow graphs and compute their transfer functions",
        "alex.murray@gmx.ch"
    );
}

PLUGIN_API void stop_plugin(Plugin* plugin)
{
}

}
