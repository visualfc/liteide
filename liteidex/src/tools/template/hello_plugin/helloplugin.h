#ifndef HELLOPLUGIN_H
#define HELLOPLUGIN_H

#include "hello_global.h"
#include "liteapi/liteapi.h"
#include <QtPlugin>

class HelloPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPlugin)
public:
    HelloPlugin();
    virtual bool initWithApp(LiteApi::IApplication *app);
};

#endif // HELLOPLUGIN_H
