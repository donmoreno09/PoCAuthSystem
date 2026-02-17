#include "AuthManager.h"
#include "SecureTokenStorage.h"
#include "PermissionManager.h"
#include "networking/AuthApi.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

AuthManager::AuthManager(QObject* parent)
    : QObject(parent)
{
    m_refreshTimer.setSingleShot(true);
    connect(&m_refreshTimer, &QTimer::timeout, this, [this]() {
        if (m_state != AuthState::Authenticated || !m_api) return;

        qDebug() << "[AuthManager] Token refresh timer fired";

        m_api->refresh(m_tokens.refreshToken,
            [this](const LoginResult& result) {
                handleLoginResult(result);
                qDebug() << "[AuthManager] Token refreshed successfully";
            },
            [this](const ErrorResult& err) {
                qWarning() << "[AuthManager] Token refresh failed:" << err.message;
                clearSession();
                setState(AuthState::Unauthenticated);
                emit sessionExpired();
            });
    });
}

void AuthManager::initialize(AuthApi* api,
                              SecureTokenStorage* storage,
                              PermissionManager* permissions)
{
    m_api = api;
    m_storage = storage;
    m_permissions = permissions;
}

void AuthManager::login(const QString& username, const QString& password)
{
    if (m_state != AuthState::Unauthenticated && m_state != AuthState::Error) {
        qWarning() << "[AuthManager] login() called in invalid state:" << static_cast<int>(m_state);
        return;
    }

    if (!m_api) {
        handleAuthError("AuthManager not initialized");
        return;
    }

    m_errorMessage.clear();
    emit errorMessageChanged();
    setState(AuthState::LoggingIn);

    m_api->login(username, password,
        [this](const LoginResult& result) {
            handleLoginResult(result);
            emit loginSucceeded();
            qDebug() << "[AuthManager] Login succeeded for" << m_session.username;
        },
        [this](const ErrorResult& err) {
            const QByteArray raw = err.reply ? err.reply->readAll() : QByteArray{};
            const QJsonObject obj = QJsonDocument::fromJson(raw).object();
            const QString message = !obj["error"].toString().isEmpty() ? obj["error"].toString() : (!raw.isEmpty() ? QString::fromUtf8(raw) : err.message);
            qWarning() << "[AuthManager] Login failed:" << message;
            handleAuthError(message);
            emit loginFailed(message);
        });
}

void AuthManager::logout()
{
    if (m_state != AuthState::Authenticated) return;

    qDebug() << "[AuthManager] Logging out" << m_session.username;

    if (m_api) {
        m_api->logout([]() {}, [](const ErrorResult&) {});
    }

    clearSession();
    setState(AuthState::Unauthenticated);
    emit loggedOut();
}

void AuthManager::tryAutoLogin()
{
    if (!m_storage || !m_api) {
        setState(AuthState::Unauthenticated);
        return;
    }

    if (!m_storage->hasStoredTokens()) {
        setState(AuthState::Unauthenticated);
        return;
    }

    AuthTokens stored = m_storage->loadTokens();
    if (stored.refreshToken.isEmpty()) {
        setState(AuthState::Unauthenticated);
        return;
    }

    setState(AuthState::AutoLoggingIn);
    qDebug() << "[AuthManager] Attempting auto-login with stored token";

    m_api->refresh(stored.refreshToken,
        [this](const LoginResult& result) {
            handleLoginResult(result);
            qDebug() << "[AuthManager] Auto-login succeeded for" << m_session.username;
        },
        [this](const ErrorResult& err) {
            qWarning() << "[AuthManager] Auto-login failed:" << err.message;
            if (m_storage) m_storage->clearAll();
            setState(AuthState::Unauthenticated);
        });
}

void AuthManager::setState(AuthState newState)
{
    if (m_state == newState) return;
    m_state = newState;
    emit stateChanged();
}

void AuthManager::handleLoginResult(const LoginResult& result)
{
    m_tokens = result.tokens;
    m_session = result.user;

    if (m_storage) {
        m_storage->saveTokens(m_tokens);
        m_storage->saveUserSession(m_session);
    }

    if (m_permissions) {
        m_permissions->loadFromSession(m_session.roles, m_session.permissions);
    }

    emit tokenChanged(m_tokens.accessToken.toUtf8());

    scheduleTokenRefresh();

    emit userChanged();
    setState(AuthState::Authenticated);
}

void AuthManager::handleAuthError(const QString& message)
{
    m_errorMessage = message;
    emit errorMessageChanged();
    setState(AuthState::Error);
}

void AuthManager::scheduleTokenRefresh()
{
    m_refreshTimer.stop();

    if (m_tokens.expiresIn <= 0) return;

    int refreshInMs = qMax(10, m_tokens.expiresIn - 60) * 1000;

    qDebug() << "[AuthManager] Scheduling token refresh in" << refreshInMs / 1000 << "seconds";
    m_refreshTimer.start(refreshInMs);
}

void AuthManager::clearSession()
{
    m_refreshTimer.stop();
    m_tokens.clear();
    m_session.clear();
    m_errorMessage.clear();

    if (m_storage) m_storage->clearAll();
    if (m_permissions) m_permissions->clear();

    emit tokenChanged(QByteArray{});
    emit userChanged();
}

AuthState AuthManager::state() const { return m_state; }
QString AuthManager::errorMessage() const { return m_errorMessage; }
QString AuthManager::username() const { return m_session.username; }
QString AuthManager::displayName() const { return m_session.displayName; }
QString AuthManager::email() const { return m_session.email; }
QStringList AuthManager::roles() const { return m_session.roles; }
QString AuthManager::accessToken() const { return m_tokens.accessToken; }
