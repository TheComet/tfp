#include "tfp/plugin/Plugin.hpp"
#include "tfp/plugin/Tool.hpp"
#include "tfp/listeners/PluginListener.hpp"
#include <QLibrary>
#include <QHBoxLayout>
#include <iostream>

namespace tfp {

struct ToolDecorator : public Tool, public PluginListener
{
    ToolDecorator(ToolFactory* factory, Plugin* plugin) :
        factory_(factory),
        plugin_(plugin)
    {
        plugin_->dispatcher.addListener(this);

        wrapped_ = factory_->create();
        QHBoxLayout* layout = new QHBoxLayout;
        layout->addWidget(wrapped_);
        setLayout(layout);
    }

    ~ToolDecorator()
    {
        if (plugin_ != NULL)
            plugin_->dispatcher.removeListener(this);
        if (wrapped_ != NULL)
            factory_->destroy(wrapped_);
    }

    virtual void onPluginUnloading() override
    {
        if (wrapped_ != NULL)
            factory_->destroy(wrapped_);
        wrapped_ = NULL;
    }

    virtual void setSystem(System* system) override
    {
        if (wrapped_ != NULL)
            wrapped_->setSystem(system);
    }

    virtual void onSetSystem() override {}
    virtual void onSystemStructureChanged() override {}
    virtual void onSystemParametersChanged() override {}
    virtual void autoScale() override {}
    virtual void replot() override {}

private:
    Tool* wrapped_;
    ToolFactory* factory_;
    WeakReference<Plugin> plugin_;
};

// ----------------------------------------------------------------------------
Plugin::Plugin() :
    start(NULL),
    stop(NULL),
    library_(new QLibrary)
{
}

// ----------------------------------------------------------------------------
Plugin::~Plugin()
{
    /*
     * All memory that was allocated by the plugin needs to be deallocated
     * right now, before unloading the library.
     *
     * The widget decorator will clean up the tree of widgets that
     * were allocated by the plugin (we notify it by dispatching an event to it)
     *
     * The factory objects were also allocated by the library, which we destroy
     * by clearing the list of factories.
     */
    dispatcher.dispatch(&PluginListener::onPluginUnloading);
    toolFactories_.clear();

    if (stop != NULL)
        stop(this);

    std::cout << "Unloading plugin " << library_->fileName().toStdString() << std::endl;
    library_->unload();
    delete library_;

}

// ----------------------------------------------------------------------------
bool Plugin::registerTool(ToolFactory* factory,
                          Category category,
                          SubCategory subCategory,
                          const QString& name,
                          const QString& author,
                          const QString& blurb,
                          const QString& contactInfo)
{
    if (toolFactories_.find(name) != toolFactories_.end())
        return false;
    toolFactories_.insert(name, factory);
    return true;
}

// ----------------------------------------------------------------------------
Tool* Plugin::createTool(const QString& name)
{
    ToolFactories::iterator it = toolFactories_.find(name);
    if (it == toolFactories_.end())
        return NULL;
    return new ToolDecorator(it.value(), this);
}

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
Tool* PluginManager::createTool(const QString& name)
{
    Tool* ret = NULL;
    for (Plugins::iterator it = plugins_.begin(); it != plugins_.end(); ++it)
        if ((ret = (*it)->createTool(name)) != NULL)
            break;
    return ret;
}

}
