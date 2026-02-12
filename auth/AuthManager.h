#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QObject>
#include <QTimer>
#include <QQmlEngine>
#include "AuthState.h"
#include "entities/UserSession.h"
#include "entities/AuthTokens.h"

class AuthApi;
class SecureTokenStorage;
class PermissionManager;
struct LoginResult;

class AuthManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    // App State
    Q_PROPERTY(AuthState state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY stateChanged)

    // User State
    Q_PROPERTY(QString username READ username NOTIFY userChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY userChanged)
    Q_PROPERTY(QString email READ email NOTIFY userChanged)
    Q_PROPERTY(QStringList roles READ roles NOTIFY userChanged)

public:
    explicit AuthManager(QObject* parent = nullptr);

    void initialize(AuthApi* api,
                    SecureTokenStorage* storage,
                    PermissionManager* permissions);

    Q_INVOKABLE void login(const QString& username, const QString& password);
    Q_INVOKABLE void logout();
    Q_INVOKABLE void tryAutoLogin();

    // App State getters
    AuthState state() const;
    QString errorMessage() const;

    // User State getters
    QString username() const;
    QString displayName() const;
    QString email() const;
    QStringList roles() const;

    // For other C++ services
    QString accessToken() const;

signals:
    void stateChanged();
    void userChanged();
    void loginSucceeded();
    void loginFailed(const QString& error);
    void loggedOut();
    void sessionExpired();
    void tokenChanged(const QByteArray& token);

private:
    void setState(AuthState newState);
    void handleLoginResult(const LoginResult& result);
    void handleAuthError(const QString& message);
    void scheduleTokenRefresh();
    void clearSession();

    AuthApi* m_api = nullptr;
    SecureTokenStorage* m_storage = nullptr;
    PermissionManager* m_permissions = nullptr;

    // App State
    AuthState m_state = AuthState::Initializing;
    QString m_errorMessage;
    QTimer m_refreshTimer;

    // User State
    UserSession m_session;
    AuthTokens m_tokens;
};

#endif // AUTHMANAGER_H
