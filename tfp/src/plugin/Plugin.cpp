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

    virtual void onPluginUnloading() OVERRIDE
    {
        if (wrapped_ != NULL)
            factory_->destroy(wrapped_);
        wrapped_ = NULL;
    }

    virtual void setSystem(System* system) OVERRIDE
    {
        if (wrapped_ != NULL)
            wrapped_->setSystem(system);
    }

    virtual void onSetSystem() OVERRIDE {}
    virtual void onSystemStructureChanged() OVERRIDE {}
    virtual void onSystemParametersChanged() OVERRIDE {}
    virtual void autoScale() OVERRIDE {}
    virtual void replot() OVERRIDE {}

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
    factory->name = name;
    factory->author = author;
    factory->blurb = blurb;
    factory->contactInfo = contactInfo;
    toolFactories_.insert(name, factory);
    return true;
}

// ----------------------------------------------------------------------------
QVector<ToolFactory*> Plugin::getToolsList() const
{
    QVector<ToolFactory*> list;
    for (ToolFactories::const_iterator it = toolFactories_.begin(); it != toolFactories_.end(); ++it)
        list.push_back(it.value());
    return list;
}

// ----------------------------------------------------------------------------
Tool* Plugin::createTool(const QString& name)
{
    ToolFactories::iterator it = toolFactories_.find(name);
    if (it == toolFactories_.end())
        return NULL;
    return new ToolDecorator(it.value(), this);
}

}
