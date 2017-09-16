#ifndef BOOKMARKSPLUGIN_H
#define BOOKMARKSPLUGIN_H

#include "bookmarks_global.h"
#include "liteapi/liteapi.h"

class BookmarksPlugin : public LiteApi::IPlugin
{
public:
    BookmarksPlugin();
    virtual bool load(LiteApi::IApplication *app);
};

class PluginFactory : public LiteApi::PluginFactoryT<BookmarksPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.BookmarksPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/Bookmarks");
        m_info->setVer("X33");
        m_info->setName("Bookmarks");
        m_info->setAuthor("visualfc");
        m_info->setInfo("Bookmarks");
        //m_info->setMustLoad(true);
        m_info->appendDepend("plugin/liteeditor");
    }
};


#endif // BOOKMARKSPLUGIN_H
