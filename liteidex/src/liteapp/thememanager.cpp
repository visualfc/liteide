
#include <QApplication>
#include <QObject>
#include <QProcess>
#include <QDebug>

#include "liteapp.h"

#ifndef Q_OS_LINUX
  #define Q_OS_LINUX 11
#endif

class ThemeManager : public QObject {
    Q_OBJECT

public:
    //single instance
    static ThemeManager* instance(QObject *parent = nullptr) {
        static ThemeManager* _instance = nullptr;
        if (!_instance) {
            _instance = new ThemeManager(parent);
        }
        return _instance;
    }

    explicit ThemeManager(QObject *parent = nullptr);

    static bool system_dark_mode;
    static bool app_dark_mode;
    static QApplication *app;

    static void monit_system_theme(QApplication *app);

    static void app_theme_changed(QString qss);
    static void editor_theme_changed();
    static bool guess_app_theme_dark(QString qss);

    static void apply_to_liteApp(LiteApp *liteApp);

signals:
    void system_theme_change(bool dark_mode);
    void app_theme_change(bool dark_mode);

public slots:
    void setting_changed();
    void system_theme_onchange(bool dark_mode);


};

// Static member initialization
bool ThemeManager::system_dark_mode = false;
bool ThemeManager::app_dark_mode = false;
QApplication *ThemeManager::app;

void auto_editor_theme(bool is_dark , LiteApi::IApplication *m_liteApp);
void auto_app_theme(bool is_dark, QApplication &app, LiteApi::IApplication *m_liteApp);

ThemeManager::ThemeManager(QObject *parent) : QObject(parent) {

}

void ThemeManager::apply_to_liteApp(LiteApp *liteApp) {
    if (!liteApp) return;

    auto_editor_theme(LiteApp::s_darkMode, liteApp);
}

void ThemeManager::system_theme_onchange(bool dark_mode) {
    foreach(LiteApi::IApplication *liteApp, LiteApp::appList()) {
        // auto_app_theme(qss, *app, liteApp);
        // auto_app_theme(dark_mode, *app, liteApp);
        auto_app_theme(dark_mode, *ThemeManager::app, liteApp);
    }
}

void ThemeManager::app_theme_changed(QString qss){
    bool is_dark = guess_app_theme_dark(qss);
    if (is_dark != app_dark_mode) {
        app_dark_mode = is_dark;
        emit instance()->app_theme_change(app_dark_mode);
    }
}
void ThemeManager::editor_theme_changed(){
    qDebug()<< "=== editor_thene_changed: need reload!";
}

bool ThemeManager::guess_app_theme_dark(QString qss){
    // printf("--- guess_dark: %s\n", qss.toStdString().c_str());
    QString dark_keywords[] = { "black", "dark", "night" };
    QString dark_names[] = { "carbon", "detroit-future", "gray", "sublime" };

    QString qss_lower = qss.toLower();

    for (const QString &keyword : dark_keywords) {
        if (qss_lower.contains(keyword)) {
            // qDebug() << "= guess dark by keyword:" << keyword;
            return true;
        }
    }
    if (qss_lower.endsWith(".qss")) {
        qss_lower.chop(4);
    }
    for (const QString &theme : dark_names) {
        if (qss_lower == theme) {
            // qDebug() << "= guess dark by name:" << theme;
            return true;
        }
    }

    return false;
}

void ThemeManager::monit_system_theme(QApplication *app) {
// void ThemeManager::monit_system_theme(QObject *app) {
    // monitor system theme and emit signal if changed

    ThemeManager::app = app;

#if defined(Q_OS_LINUX)

    qDebug()<< "=== monit_system_theme_change...";

    // 启动 gsettings monitor 进程
	// gsettings monitor org.gnome.desktop.interface color-scheme
    QProcess *proc = new QProcess(app);
    proc->setProcessChannelMode(QProcess::MergedChannels);
    proc->start("gsettings", {"monitor", "org.gnome.desktop.interface", "color-scheme"});

    // 初始读取一次
    {
        QProcess getProc;
        getProc.start("gsettings", {"get", "org.gnome.desktop.interface", "color-scheme"});
        getProc.waitForFinished();
        QString theme = getProc.readAllStandardOutput().trimmed();
		theme = theme.remove("'");
        qDebug() << "== system theme current:" << theme;

        //----- 0. init theme
        bool is_dark = theme.contains("dark", Qt::CaseInsensitive);
        system_dark_mode = is_dark;
        // emit instance()->system_theme_change(system_dark_mode);
        instance()->system_theme_onchange(system_dark_mode);
    }

    // 监听变化
    QObject::connect(proc, &QProcess::readyReadStandardOutput, app, [proc, app]() {
        while (proc->canReadLine()) {
            QString line = QString::fromUtf8(proc->readLine()).trimmed();
            if (line.contains("color-scheme")) {
                QString theme = line.section(' ', -1); // 取最后一个单词
				theme = theme.remove("'");
                qDebug() << "== system theme changed:" << theme;

                bool is_dark = theme.contains("dark", Qt::CaseInsensitive);

                bool new_mode = is_dark;
                if (new_mode != system_dark_mode) {
                    system_dark_mode = new_mode;
                    // emit instance()->system_theme_change(system_dark_mode);

                    instance()->system_theme_onchange(system_dark_mode);
                }
            }
        }
    });

#elif defined(Q_OS_WIN)

#elif defined(Q_OS_MAC)

#else

#endif
}


bool guess_dark(QString qss){
    // printf("--- guess_dark: %s\n", qss.toStdString().c_str());
    QString dark_keywords[] = { "black", "dark", "night" };
    QString dark_names[] = { "carbon", "detroit-future", "gray", "sublime" };

    QString qss_lower = qss.toLower();

    for (const QString &keyword : dark_keywords) {
        if (qss_lower.contains(keyword)) {
            // qDebug() << "= guess dark by keyword:" << keyword;
            return true;
        }
    }
    if (qss_lower.endsWith(".qss")) {
        qss_lower.chop(4);
    }
    for (const QString &theme : dark_names) {
        if (qss_lower == theme) {
            // qDebug() << "= guess dark by name:" << theme;
            return true;
        }
    }

    return false;
}

void auto_editor_theme(bool is_dark , LiteApi::IApplication *m_liteApp){
    //----1.check if dark
    // bool is_dark = guess_dark(qss);
    // qDebug() << "--- auto_editor_theme guess_dark:" << (is_dark ? "dark" : "light") << " //app:" << m_liteApp;

    //----2.get editor settings
    #define EDITOR_STYLE "editor/style"
    #define EDITOR_STYLE_DARK "editor/style_dark"
    QString styleName = m_liteApp->settings()->value(EDITOR_STYLE,"default.xml").toString();
    QString styleName_dark = m_liteApp->settings()->value(EDITOR_STYLE_DARK,"NOT SET").toString();

    //----3.apply editor theme
    if (is_dark && styleName_dark != "NOT SET") {
        // styleName = styleName_dark;
        qDebug() << "=== auto_editor_theme dark:" << styleName_dark;
        QString style = styleName_dark;
        QString styleFile = m_liteApp->resourcePath()+"/liteeditor/color/"+style;
        m_liteApp->editorManager()->loadColorStyleScheme(styleFile);
    }else{
        qDebug() << "=== auto_editor_theme light:" << styleName;
        QString style = styleName;
        QString styleFile = m_liteApp->resourcePath()+"/liteeditor/color/"+style;
        m_liteApp->editorManager()->loadColorStyleScheme(styleFile);
    }
}

void auto_app_theme(QString qss, QApplication &app, LiteApi::IApplication *m_liteApp){
    // QFile f(resPath+"/liteapp/qss/"+qss);
    QFile f(m_liteApp->resourcePath()+"/liteapp/qss/"+qss);
    if (f.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(f.readAll());
        app.setStyleSheet(styleSheet);
    }

    LiteApp::s_darkMode = guess_dark(qss);
    auto_editor_theme(LiteApp::s_darkMode, m_liteApp);

    // auto_editor_theme(qss, m_liteApp);
    // foreach(LiteApi::IApplication *liteApp, m_liteApp->appList()) {
    // foreach(LiteApi::IApplication *liteApp, m_liteApp->instanceList()) {
    //     qDebug() << " >>> auto_app_theme: - instance:" << liteApp;
    //     auto_editor_theme(qss, liteApp);
    // }
}

void auto_app_theme(bool is_dark, QApplication &app, LiteApi::IApplication *m_liteApp){
    QString qss = is_dark ? "gray.qss" : "default.qss";
    auto_app_theme(qss, app, m_liteApp);
}
