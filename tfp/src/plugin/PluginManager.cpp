#include "tfp/plugin/PluginManager.hpp"
#include "tfp/plugin/Plugin.hpp"
#include "tfp/views/DataTree.hpp"
#include <QLibrary>
#include <QDir>

namespace tfp {

// ----------------------------------------------------------------------------
PluginManager::PluginManager(DataTree* dataTree) :
    dataTree_(dataTree)
{
}

// ----------------------------------------------------------------------------
bool PluginManager::loadPlugin(const QString& name)
{
    Reference<Plugin> plugin(new Plugin(name));
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
    if ((plugin->run_tests = (Plugin::run_tests_func)plugin->library_->resolve("run_tests")) == NULL)
    {
        std::cout << "Plugin " << name.toStdString() << " has no unit tests :/" << std::endl;
    }

    std::cout << "Loaded plugin " << name.toStdString() << std::endl;
    plugins_.push_back(plugin);

    return true;
}

// ----------------------------------------------------------------------------
bool PluginManager::loadAllPlugins()
{
    QDir pluginsDir("plugins");
    QStringList allFiles = pluginsDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

    QStringList validExtensions;
    validExtensions += ".dll";
    validExtensions += ".so";
    validExtensions += ".dylib";
    validExtensions += ".dynlib";

    bool success = true;
    for (QStringList::const_iterator it = allFiles.begin(); it != allFiles.end(); ++it)
        for (QStringList::const_iterator ext = validExtensions.begin(); ext != validExtensions.end(); ++ext)
            if (it->endsWith(*ext, Qt::CaseInsensitive))
            {
                success &= loadPlugin(QDir::toNativeSeparators("plugins/" + *it));
                break;
            }

    return success;
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

// ----------------------------------------------------------------------------
QVector<QString> PluginManager::runPluginTests(int argc, char** argv) const
{
    QVector<QString> failedPlugins;
    for (Plugins::const_iterator it = plugins_.begin(); it != plugins_.end(); ++it)
        if ((*it)->runTests(argc, argv) != 0)
            failedPlugins.push_back((*it)->getName());
    return failedPlugins;
}

}
