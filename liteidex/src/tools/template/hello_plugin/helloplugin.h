#ifndef HELLOPLUGIN_H
#define HELLOPLUGIN_H

#include "hello_global.h"
#include "liteapi/liteapi.h"

class HelloPlugin : public LiteApi::IPlugin
{
public:
    HelloPlugin();
    virtual bool initWithApp(LiteApi::IApplication *app);
};

class PluginFactory : public LiteApi::PluginFactory<HelloPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
};


#endif // HELLOPLUGIN_H
