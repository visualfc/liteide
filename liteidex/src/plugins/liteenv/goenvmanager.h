#ifndef GOENVMANAGER_H
#define GOENVMANAGER_H

#include "liteenvapi/liteenvapi.h"

class GoEnvManager : public LiteApi::IGoEnvManger
{
    Q_OBJECT
public:
    GoEnvManager(QObject *parent);
    virtual bool initWithApp(LiteApi::IApplication *app);
    virtual QString gocmd() const;
    virtual QString gotools() const;
    virtual QString GOROOT() const;
    virtual QStringList GOPATH() const;
    virtual QProcessEnvironment environment() const;
    virtual QProcessEnvironment customEnvironment(const QString &buildFilePath, QString *pCustomBuildPath) const;
    virtual QStringList customGOPATH(const QString &buildPath, QString *pCustomBuildPath) const;
    virtual QString findRealCustomBuildPath(const QString &buildPath) const;
    virtual bool hasCustomGOPATH(const QString &buildPath) const;
    void updateGoEnv();
protected:
    QString m_gocmd;
    QString m_gotools;
    QString m_goroot;
    QStringList m_gopathList;
    LiteApi::IEnvManager *m_envManager;
    QProcessEnvironment m_goenv;
};

#endif // GOENVMANAGER_H
