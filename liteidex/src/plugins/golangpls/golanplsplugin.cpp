#include "golangpls.h"
#include "golangplsoptionfactory.h"
#include "golanplsplugin.h"
#include "golangpls_global.h"

GolanPlsPlugin::GolanPlsPlugin()
{
}

bool GolanPlsPlugin::load(LiteApi::IApplication *app)
{
    m_liteApp = app;
    m_pls= new GolangPls(app,this);

    app->optionManager()->addFactory(new GolangPlsOptionFactory(app,this));
    connect(app->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    //connect(app->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(app,SIGNAL(loaded()),this,SLOT(appLoaded()));

    return true;
}

QStringList GolanPlsPlugin::dependPluginList() const
{
    return {};
}

void GolanPlsPlugin::appLoaded()
{

}

void GolanPlsPlugin::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    if (editor->mimeType() != "text/x-gosrc") {
        return;
    }
    LiteApi::ILiteEditor *liteEdit = LiteApi::getLiteEditor(editor);
    if (liteEdit) {
        liteEdit->setSpellCheckZoneDontComplete(true);
    }
}

void GolanPlsPlugin::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (editor) {
        if (editor->mimeType() == "text/x-gosrc") {
            LiteApi::ICompleter *completer = LiteApi::findExtensionObject<LiteApi::ICompleter*>(editor,"LiteApi.ICompleter");
            m_pls->setCompleter(completer);
            return;
        } else if (editor->mimeType() == "browser/goplay") {
            LiteApi::IEditor* editor = LiteApi::findExtensionObject<LiteApi::IEditor*>(m_liteApp->extension(),"LiteApi.Goplay.IEditor");
            if (editor && editor->mimeType() == "text/x-gosrc") {
                LiteApi::ICompleter *completer = LiteApi::findExtensionObject<LiteApi::ICompleter*>(editor,"LiteApi.ICompleter");
                m_pls->setCompleter(completer);
                return;
            }
        }
    }
    m_pls->setCompleter(0);
}
