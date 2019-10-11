#include "imageeditorfactory.h"
#include "mimetype/mimetype.h"
#include "imageeditor.h"
#include <QImageReader>
#include <QDebug>

ImageEditorFactory::ImageEditorFactory(IApplication *app, QObject *parent)
    : LiteApi::IEditorFactory(parent), m_liteApp(app)
{
    foreach (QByteArray _type, QImageReader::supportedMimeTypes()) {
        QString type = QString::fromUtf8(_type);
        MimeType *mimeType = new MimeType;
        mimeType->setType(type);
        mimeType->setComment(QString("%1 Image").arg(type));
        foreach(QByteArray fmt, QImageReader::imageFormatsForMimeType(_type)) {
            mimeType->appendGlobPatterns("*."+QString::fromUtf8(fmt));
        }
        m_liteApp->mimeTypeManager()->addMimeType(mimeType);
        m_mimeTypes.append(type);
    }
}

QStringList ImageEditorFactory::mimeTypes() const
{
    return  m_mimeTypes;
}

IEditor *ImageEditorFactory::open(const QString &fileName, const QString &mimeType)
{
    if (!m_mimeTypes.contains(mimeType)) {
        return  0;
    }
    ImageEditor *view = new ImageEditor(m_liteApp);
    if (!view->open(fileName,mimeType)) {
        delete  view;
        return  0;
    }
    return  view;
}

IEditor *ImageEditorFactory::create(const QString &contents, const QString &mimeType)
{
    return  0;
}
