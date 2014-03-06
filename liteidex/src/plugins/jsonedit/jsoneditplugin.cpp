#include "jsoneditplugin.h"
#include "jsonedit.h"

#include <QtPlugin>

JsonEditPlugin::JsonEditPlugin()
{
}

bool JsonEditPlugin::load(LiteApi::IApplication *app)
{
    m_liteApp = app;
    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    return true;
}

void JsonEditPlugin::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor || editor->mimeType() != "application/json") {
        return;
    }
    new JsonEdit(m_liteApp,editor,this);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
