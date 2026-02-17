#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "config.h"
#include "networking/ApiEndpoints.h"
#include "networking/HttpClient.h"
#include "networking/AuthApi.h"
#include "networking/ItemApi.h"
#include "auth/AuthManager.h"
#include "auth/PermissionManager.h"
#include "auth/SecureTokenStorage.h"
#include "models/ItemModel.h"

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

    auto* itemHttpClient = new HttpClient(&app);
    auto* itemApi        = new ItemApi(itemHttpClient, &app);

    auto* authManager = engine.singletonInstance<AuthManager*>("PoCAuthSystem", "AuthManager");
    auto* permManager = engine.singletonInstance<PermissionManager*>("PoCAuthSystem", "PermissionManager");
    auto* itemModel   = engine.singletonInstance<ItemModel*>("PoCAuthSystem", "ItemModel");

    authManager->initialize(authApi, tokenStorage, permManager);
    itemModel->initialize(itemApi);

    QObject::connect(authManager, &AuthManager::tokenChanged, itemHttpClient, &HttpClient::setBearerToken);
    QObject::connect(authManager, &AuthManager::loginSucceeded, itemModel, &ItemModel::fetch);
    QObject::connect(authManager, &AuthManager::loggedOut, itemHttpClient, &HttpClient::clearBearerToken);

    authManager->tryAutoLogin();

    engine.loadFromModule("PoCAuthSystem", "Main");

    return app.exec();
}
