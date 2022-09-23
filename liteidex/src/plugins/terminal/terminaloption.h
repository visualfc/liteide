#ifndef TERMINALOPTION_H
#define TERMINALOPTION_H

#include "liteapi/liteapi.h"

namespace Ui {
    class TermianlOption;
}

class TerminalOption : public LiteApi::IOption
{
    Q_OBJECT

public:
    explicit TerminalOption(LiteApi::IApplication *app, QObject *parent = 0);
    ~TerminalOption();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QString mimeType() const;
    virtual void load();
    virtual void save();
    void updatePointSizes();
    QList<int> pointSizesForSelectedFont() const;
private:
    LiteApi::IApplication   *m_liteApp;
    QWidget           *m_widget;
    Ui::TermianlOption *ui;
    QStringList m_familyList;
    QString m_fontFamily;
    int     m_fontSize;
};

#endif // TERMINALOPTION_H
