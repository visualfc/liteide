#ifndef GOPLSOPTION_H
#define GOPLSOPTION_H

#include "liteapi/liteapi.h"

namespace Ui { class GoplsOption; }

class GoplsOption : public LiteApi::IOption
{
    Q_OBJECT
public:
    explicit GoplsOption(LiteApi::IApplication *app, QObject *parent = 0);
    ~GoplsOption();
    QWidget *widget();
    QString name() const;
    QString mimeType() const;
    void load();
    void save();
private:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_widget;
    Ui::GoplsOption *ui;
};

#endif // GOPLSOPTION_H
