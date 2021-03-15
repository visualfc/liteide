#ifndef GOLANGPLSOPTION_H
#define GOLANGPLSOPTION_H

#include "liteapi/liteapi.h"

class GolangPlsOption : public LiteApi::IOption
{
    Q_OBJECT

public:
    explicit GolangPlsOption(LiteApi::IApplication *app, QObject *parent = 0);

private:
    LiteApi::IApplication   *m_liteApp;

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
