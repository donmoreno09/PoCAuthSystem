#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <QGuiApplication>
#include <QFile>
#include <QSettings>

struct AppConfig {
    QString restBaseUrl;
};

static void ensureUserConfigExists()
{
    const QString appDir         = QCoreApplication::applicationDirPath();
    const QString userCfgPath    = appDir + "/config.ini";
    const QString defaultCfgPath = appDir + "/config.default.ini";

    if (!QFile::exists(userCfgPath) && QFile::exists(defaultCfgPath)) {
        QFile::copy(defaultCfgPath, userCfgPath);
        QFile::setPermissions(userCfgPath,
                              QFile::permissions(userCfgPath) | QFile::WriteOwner);
    }
}

static AppConfig loadConfig()
{
    const QString appDir      = QCoreApplication::applicationDirPath();
    const QString userCfgPath = appDir + "/config.ini";

    QSettings s(userCfgPath, QSettings::IniFormat);

    AppConfig cfg;
    cfg.restBaseUrl = s.value("rest/baseUrl", "http://localhost:7000").toString();

    return cfg;
}

#endif // APP_CONFIG_H
