#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Reference.hpp"
#include <QVector>

namespace tfp {

class DataTree;
class Plugin;
class Tool;
class ToolFactory;

class PluginManager : public RefCounted
{
public:
    PluginManager(DataTree* dataTree);

    bool loadPlugin(const QString& fileName);

    QVector<ToolFactory*> getToolsList() const;

    Tool* createTool(const QString& name);

private:
    typedef QVector< Reference<Plugin> > Plugins;
    Plugins plugins_;
    // Stuff to pass to plugins in start()
    DataTree* dataTree_;
};

}
