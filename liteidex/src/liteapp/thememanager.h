
// Module: thememanager.cpp
// Creator: yurenchen

#pragma once

#include <QObject>
#include <QString>
#include <QApplication>

#include "liteapp.h"

class ThemeManager : public QObject {
    Q_OBJECT

public:
    static ThemeManager* instance(QObject *parent = nullptr);

    explicit ThemeManager(QObject *parent = nullptr);

    static bool system_dark_mode;
    static bool app_dark_mode;

    //init
    static void monit_system_theme(QApplication *app);
    
    //call when change
    static void app_theme_changed(QString qss);
    static void editor_theme_changed();

    //call when liteApp created
    static void apply_to_liteApp(LiteApp *liteApp);
private:
    static bool guess_app_theme_dark(QString qss);

signals:
    void system_theme_change(bool dark_mode);
    void app_theme_change(bool dark_mode);

public slots:
    void setting_changed();
};