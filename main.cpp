#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>

#include "config.h"
#include "networking/ApiEndpoints.h"
#include "networking/HttpClient.h"
#include "networking/AuthApi.h"
#include "auth/AuthManager.h"
#include "auth/PermissionManager.h"
#include "auth/SecureTokenStorage.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    ensureUserConfigExists();
    AppConfig appConfig = loadConfig();
    ApiEndpoints::BaseUrl = appConfig.restBaseUrl;

    QCoreApplication::setOrganizationName("IRIDESS");
    QCoreApplication::setApplicationName("PoCAuthSystem");
    QCoreApplication::setApplicationVersion("1.0.0");

    QQmlApplicationEngine engine;

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.addImportPath("qrc:/");

    auto* authHttpClient = new HttpClient(&app);
    auto* authApi        = new AuthApi(authHttpClient, &app);
    auto* tokenStorage   = new SecureTokenStorage(&app);

    auto* authManager = engine.singletonInstance<AuthManager*>("PoCAuthSystem", "AuthManager");
    auto* permManager = engine.singletonInstance<PermissionManager*>("PoCAuthSystem", "PermissionManager");

    authManager->initialize(authApi, tokenStorage, permManager);
    authManager->tryAutoLogin();

    qDebug() << "[Main] Auth service initialized";
    qDebug() << "[Main] API base URL:" << ApiEndpoints::BaseUrl;

    engine.loadFromModule("PoCAuthSystem", "Main");

    return app.exec();
}
