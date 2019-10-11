#include "imageeditorplugin.h"
#include "imageeditorfactory.h"
#include <QtPlugin>

ImageEditorPlugin::ImageEditorPlugin()
{
}

bool ImageEditorPlugin::load(LiteApi::IApplication *app)
{
    app->editorManager()->addFactory(new ImageEditorFactory(app,this));
    return true;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
