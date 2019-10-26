#ifndef IMAGEEDITORPLUGIN_H
#define IMAGEEDITORPLUGIN_H

#include "imageeditor_global.h"
#include "liteapi/liteapi.h"

class ImageEditorPlugin : public LiteApi::IPlugin
{
public:
    ImageEditorPlugin();
    virtual bool load(LiteApi::IApplication *app);
};

class PluginFactory : public LiteApi::PluginFactoryT<ImageEditorPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.ImageEditorPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/ImageViewer");
        m_info->setVer("X36.2");
        m_info->setName("ImageEditor");
        m_info->setAuthor("visualfc");
        m_info->setInfo("Image Viewer Editor");
        //m_info->appendDepend("plugin/liteenv");
    }
};


#endif // IMAGEEDITORPLUGIN_H
