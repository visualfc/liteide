#ifndef GOLANGPLSOPTION_H
#define GOLANGPLSOPTION_H

#include "liteapi/liteapi.h"

namespace Ui {
class GolangPlsOption;
}

class GolangPlsOption : public LiteApi::IOption
{
    Q_OBJECT

public:
    explicit GolangPlsOption(LiteApi::IApplication *app,QObject *parent = nullptr);
    ~GolangPlsOption();

private:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_widget;
    Ui::GolangPlsOption *ui;

    // IView interface
public:
    QWidget *widget();
    QString name() const;

    // IOption interface
public:
    QString mimeType() const;
    void save();
    void load();
};

#endif // GOLANGPLSOPTION_H
