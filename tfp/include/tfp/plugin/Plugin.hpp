#pragma once

#include "tfp/config.hpp"
#include "tfp/models/System.hpp"
#include "tfp/util/Reference.hpp"
#include "tfp/util/ListenerDispatcher.hpp"
#include <typeinfo>
#include <QMap>
#include <QVector>

class QLibrary;

namespace tfp {

class DataTree;
class PluginManager;
class PluginListener;
class Tool;
class ToolFactory;
template <class T> class ToolFactoryImpl;

class TFP_PUBLIC_API Plugin : public RefCounted
{
public:
    enum Category
    {
        GENERATOR,
        MANIPULATOR,
        VISUALISER,

        CATEGORY_COUNT
    };

    enum SubCategory
    {
        LTI_SYSTEM_CONTINUOUS,
        LTI_SYSTEM_DISCRETE,

        SUB_CATEGORY_COUNT
    };

    Plugin();
    ~Plugin();

    template <class T>
    bool registerTool(Category category,
                      SubCategory subCategory,
                      const QString& name,
                      const QString& author,
                      const QString& blurb,
                      const QString& contactInfo)
    {
        return registerTool(new ToolFactoryImpl<T>(), category, subCategory, name, author, blurb, contactInfo);
    }

    QVector<ToolFactory*> getToolsList() const;

    Tool* createTool(const QString& name);

    ListenerDispatcher<PluginListener> dispatcher;

    friend class PluginManager;

private:
    bool registerTool(ToolFactory* factory,
                      Category category,
                      SubCategory subCategory,
                      const QString& name,
                      const QString& author,
                      const QString& blurb,
                      const QString& contactInfo);
    void unregisterTool(const QString& typeName);

private:
    typedef bool (*start_plugin_func)(Plugin*, DataTree*);
    typedef void (*stop_plugin_func)(Plugin*);
    typedef QMap< QString, Reference<ToolFactory> > ToolFactories;

    start_plugin_func start;
    stop_plugin_func stop;
    QLibrary* library_;
    ToolFactories toolFactories_;
};

class ToolFactory : public RefCounted
{
public:
    virtual Tool* create() = 0;
    virtual void destroy(Tool* o) = 0;

    Plugin::Category category;
    Plugin::SubCategory subCategory;
    QString name;
    QString author;
    QString blurb;
    QString contactInfo;
    QString typeName;
};

template <class T>
class ToolFactoryImpl : public ToolFactory
{
public:
    ToolFactoryImpl() { typeName = typeid(T).name(); }
    virtual Tool* create() OVERRIDE { return new T; }
    virtual void destroy(Tool* o) OVERRIDE { delete o; }
};

} // namespace tfp

#if defined(PLUGIN_BUILDING)
extern "C" {
	Q_DECL_EXPORT bool start_plugin(tfp::Plugin* plugin, tfp::DataTree* dataTree);
	Q_DECL_EXPORT void stop_plugin(tfp::Plugin* plugin);
}
#endif
