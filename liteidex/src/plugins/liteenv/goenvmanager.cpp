#include "goenvmanager.h"
#include "fileutil/fileutil.h"
#include <QDir>

using namespace LiteApi;

GoEnvManager::GoEnvManager(QObject *parent)
    : IGoEnvManger(parent)
{

}

bool GoEnvManager::initWithApp(IApplication *app)
{
    if (!IGoEnvManger::initWithApp(app)) {
        return false;
    }
    m_liteApp->extension()->addObject("LiteApi.GoEnvManager",this);
    m_envManager = LiteApi::getEnvManager(app);
    return true;
}

QString GoEnvManager::gocmd() const
{
    return m_gocmd;
}

QString GoEnvManager::gotools() const
{
    return m_gotools;
}

QString GoEnvManager::GOROOT() const
{
    return m_goroot;
}

QStringList GoEnvManager::GOPATH() const
{
    return m_gopathList;
}

QProcessEnvironment GoEnvManager::environment() const
{
    return LiteApi::getGoEnvironment(m_liteApp);
}

QProcessEnvironment GoEnvManager::customEnvironment(const QString &buildFilePath, QString *pCustomBuildPath) const
{
    return LiteApi::getCustomGoEnvironment(m_liteApp,buildFilePath,pCustomBuildPath);
}

QStringList GoEnvManager::customGOPATH(const QString &buildPath, QString *pCustomBuildPath) const
{
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif
    QProcessEnvironment env = this->customEnvironment(buildPath,pCustomBuildPath);
    return env.value("GOPATH").split(sep);
}

QString GoEnvManager::findRealCustomBuildPath(const QString &buildPath) const
{
    if (buildPath.isEmpty()) {
        return QString();
    }
    QString customKey = "litebuild-custom/"+buildPath;
    QString customBuildPath = buildPath;
    bool use_custom_gopath = m_liteApp->settings()->value(customKey+"#use_custom_gopath",false).toBool();
    if (!use_custom_gopath) {
        QString srcRoot = LiteApi::lookupSrcRoot(buildPath);
        if (!srcRoot.isEmpty()) {
            customKey = LiteApi::lookupParentHasCustom(m_liteApp,buildPath,srcRoot, &customBuildPath);
            if (!customKey.isEmpty()) {
                use_custom_gopath = true;
                return customBuildPath;
            }
        }
    }
    return QString();
}

bool GoEnvManager::hasCustomGOPATH(const QString &buildPath) const
{
    return !findRealCustomBuildPath(buildPath).isEmpty();
}

void GoEnvManager::updateGoEnv()
{
    m_gocmd = FileUtil::lookupGoBin("go",m_liteApp,false);
    m_gotools = LiteApi::getGotools(m_liteApp);
    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
    m_goroot = env.value("GOROOT");
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif
    QStringList pathList;
    QString goroot = QDir::toNativeSeparators(env.value("GOROOT"));
    foreach (QString path, env.value("GOPATH").split(sep,QString::SkipEmptyParts)) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.removeAll(goroot);
    pathList.removeDuplicates();

    if (m_gopathList != pathList) {
        m_gopathList = pathList;
        emit globalGOPATHChanged();
    }
}
