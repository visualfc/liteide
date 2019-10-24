#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include "liteapi/liteapi.h"
#include "navigate/navigate.h"

class ImageEditorFile;
class ImageEditorWidget;
class QLabel;
class QToolBar;
class ImageEditor : public LiteApi::IEditor
{
    Q_OBJECT
public:
    ImageEditor(LiteApi::IApplication *app);
    virtual ~ImageEditor();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual bool open(const QString &filePath,const QString &mimeType);
    virtual bool reload();
    virtual bool save();
    virtual bool saveAs(const QString &filePath);
    virtual void setReadOnly(bool b);
    virtual bool isReadOnly() const;
    virtual bool isModified() const;
    virtual QString filePath() const;
    virtual QString mimeType() const;
    virtual QByteArray saveState() const;
    virtual bool restoreState(const QByteArray &array);
    virtual void onActive();
public slots:
    void scaleFactorChanged(qreal factor);
    void toggledPlay(bool checked);
    void frameChanged(int frameNumber);
    void broadcast(const QString &module, const QString &id, const QVariant &param);
protected:
    LiteApi::IApplication *m_liteApp;
    ImageEditorFile *m_file;
    ImageEditorWidget *m_imageWidget;
    QWidget *m_widget;
    QToolBar *m_toolBar;
    QToolBar *m_mvToolBar;
    NavigateBar *m_navBar;
    QLabel *m_imageInfo;
    QLabel *m_scaleInfo;
    QLabel *m_frameLabel;
    QAction *m_playAct;
};


#endif // IMAGEEDITOR_H
