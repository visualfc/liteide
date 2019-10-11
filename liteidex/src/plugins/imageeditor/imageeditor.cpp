#include "imageeditor.h"
#include "imageeditorfile.h"
#include "imageeditorwidget.h"
#include <QFileInfo>
#include <QDebug>

ImageEditor::ImageEditor(LiteApi::IApplication *app)
    : m_liteApp(app)
{
    m_file = new ImageEditorFile(m_liteApp,this);
    m_widget = new ImageEditorWidget;
}

ImageEditor::~ImageEditor()
{
    delete m_widget;
    delete m_file;
}

QWidget *ImageEditor::widget()
{
    return  m_widget;
}

QString ImageEditor::name() const
{
    return  QFileInfo(m_file->filePath()).fileName();
}

bool ImageEditor::open(const QString &filePath, const QString &mimeType)
{
    bool b = m_file->open(filePath,mimeType);
    m_widget->setImageItem(m_file->graphicsItem());
    return  b;
}

bool ImageEditor::reload()
{
    if (!m_file->isValid()) {
        return  false;
    }
    QString filePath = m_file->filePath();
    QString mimeType = m_file->mimeType();
    m_file->clear();
    bool b = m_file->open(filePath,mimeType);
    m_widget->setImageItem(m_file->graphicsItem());
    return b;
}

bool ImageEditor::save()
{
    return  false;
}

bool ImageEditor::saveAs(const QString &filePath)
{
    return  false;
}

void ImageEditor::setReadOnly(bool b)
{

}

bool ImageEditor::isReadOnly() const
{
    return  true;
}

bool ImageEditor::isModified() const
{
    return  false;
}

QString ImageEditor::filePath() const
{
    return  m_file->filePath();
}

QString ImageEditor::mimeType() const
{
    return  m_file->mimeType();
}

QByteArray ImageEditor::saveState() const
{
    return  QByteArray();
}

bool ImageEditor::restoreState(const QByteArray &array)
{
    return false;
}

void ImageEditor::onActive()
{

}
