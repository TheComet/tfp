#include "tfp/plugin/PluginManager.hpp"
#include "tfp/plugin/Plugin.hpp"
#include "tfp/views/DataTree.hpp"
#include <QLibrary>

namespace tfp {

// ----------------------------------------------------------------------------
PluginManager::PluginManager(DataTree* dataTree) :
    dataTree_(dataTree)
{
}

// ----------------------------------------------------------------------------
bool PluginManager::loadPlugin(const QString& name)
{
    Reference<Plugin> plugin(new Plugin);
    plugin->library_->setFileName(name);
    if (plugin->library_->load() == false)
    {
        std::cout << "Failed to load plugin: " << plugin->library_->errorString().toStdString() << std::endl;
        return false;
    }

    if ((plugin->start = (Plugin::start_plugin_func)plugin->library_->resolve("start_plugin")) == NULL)
    {
        std::cout << "Failed to load plugin: " << "symbol start_plugin() not found" << std::endl;
        return false;
    }
    if ((plugin->stop = (Plugin::stop_plugin_func)plugin->library_->resolve("stop_plugin")) == NULL)
    {
        std::cout << "Failed to load plugin: " << "symbol start_plugin() not found" << std::endl;
        return false;
    }
    if (plugin->start(plugin, dataTree_) == false)
    {
        std::cout << "Failed to load plugin: " << "start_plugin() returned an error" << std::endl;
        return false;
    }

    std::cout << "Loaded plugin " << name.toStdString() << std::endl;
    plugins_.push_back(plugin);

    return true;
}

// ----------------------------------------------------------------------------
QVector<ToolFactory*> PluginManager::getToolsList() const
{
    QVector<ToolFactory*> list;
    for (Plugins::const_iterator it = plugins_.begin(); it != plugins_.end(); ++it)
        list.append(it->get()->getToolsList());
    return list;
}

// ----------------------------------------------------------------------------
Tool* PluginManager::createTool(const QString& name)
{
    Tool* ret = NULL;
    for (Plugins::iterator it = plugins_.begin(); it != plugins_.end(); ++it)
        if ((ret = (*it)->createTool(name)) != NULL)
            break;
    return ret;
}

}
