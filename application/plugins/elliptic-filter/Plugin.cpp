#include "EllipticFilter.hpp"
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
    return plugin->registerTool<EllipticFilter>(
        Plugin::GENERATOR,
        Plugin::LTI_SYSTEM_CONTINUOUS,
        "Elliptic Filter",
        "Alex Murray",
        "Design an elliptic filter and compute its transfer function",
        "alex.murray@gmx.ch"
    );
}

PLUGIN_API void stop_plugin(Plugin* plugin)
{
}

}
