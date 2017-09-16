#include "bookmarksplugin.h"
#include "bookmarkmanager.h"
#include <QtPlugin>

BookmarksPlugin::BookmarksPlugin()
{
}

bool BookmarksPlugin::load(LiteApi::IApplication *app)
{
    BookmarkManager *manager = new BookmarkManager(app);
    if (!manager->initWithApp(app)) {
        return false;
    }
    return true;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
