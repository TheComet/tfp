#pragma once

#include "tfp/config.hpp"
#include "tfp/models/System.hpp"
#include "tfp/util/Reference.hpp"
#include "tfp/util/ListenerDispatcher.hpp"
#include "tfp/views/SystemManipulator.hpp"
#include <QMap>
#include <QVector>

class QLibrary;

namespace tfp {

class DataTree;
class PluginManager;
class PluginListener;

struct SystemManipulatorFactory : public RefCounted
{
    virtual SystemManipulator* create() = 0;
    virtual void destroy(SystemManipulator* o) = 0;
};

template <class T>
struct SystemManipulatorFactoryImpl : public SystemManipulatorFactory
{
    virtual SystemManipulator* create() { return new T; }
    virtual void destroy(SystemManipulator* o) { delete o; }
};

class Plugin : public RefCounted
{
public:
    Plugin();
    ~Plugin();

    bool registerSystemManipulator(SystemManipulatorFactory* factory, const QString& name);

    template <class T>
    bool registerSystemManipulator(const QString& name)
    {
        return registerSystemManipulator(new SystemManipulatorFactoryImpl<T>(), name);
    }

    SystemManipulator* createSystemManipulator(const QString& name);

    ListenerDispatcher<PluginListener> dispatcher;

    friend class PluginManager;
private:
    typedef bool (*start_plugin_func)(Plugin*, DataTree*);
    typedef void (*stop_plugin_func)(Plugin*);
    typedef QMap< QString, Reference<SystemManipulatorFactory> > ManipulatorFactories;

    start_plugin_func start;
    stop_plugin_func stop;
    QLibrary* library_;
    ManipulatorFactories manipulatorFactories_;
};

class PluginManager : public RefCounted
{
public:
    PluginManager(DataTree* dataTree);

    bool loadPlugin(const QString& name);

    SystemManipulator* createSystemManipulator(const QString& name);

private:
    typedef QVector< Reference<Plugin> > Plugins;
    Plugins plugins_;
    // Stuff to pass to plugins in start()
    DataTree* dataTree_;
};

}
