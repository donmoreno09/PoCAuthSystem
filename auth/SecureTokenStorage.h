#ifndef SECURETOKENSTORAGE_H
#define SECURETOKENSTORAGE_H

#include <QObject>
#include <QSettings>
#include "entities/AuthTokens.h"
#include "entities/UserSession.h"

class SecureTokenStorage : public QObject
{
    Q_OBJECT

public:
    explicit SecureTokenStorage(QObject* parent = nullptr);

    void saveTokens(const AuthTokens& tokens);
    AuthTokens loadTokens() const;

    void saveUserSession(const UserSession& session);
    UserSession loadUserSession() const;

    void clearAll();
    bool hasStoredTokens() const;

    qint64 loadExpiresAt() const;

private:
    QSettings m_settings;
};

#endif // SECURETOKENSTORAGE_H
