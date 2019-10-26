#ifndef IMAGEEDITORFACTORY_H
#define IMAGEEDITORFACTORY_H

#include "liteapi/liteapi.h"

using namespace LiteApi;

class ImageEditorFactory : public LiteApi::IEditorFactory
{
    Q_OBJECT
public:
    ImageEditorFactory(LiteApi::IApplication *app, QObject *parent);
    virtual QStringList mimeTypes() const;
    virtual IEditor *open(const QString &fileName, const QString &mimeType);
    virtual IEditor *create(const QString &contents, const QString &mimeType);
    virtual QString id() const;
    virtual QString displayName() const;
    virtual bool testMimeType(const QString &mimeType);
protected:
    LiteApi::IApplication *m_liteApp;
    QStringList m_mimeTypes;
};


#endif // IMAGEEDITORFACTORY_H
