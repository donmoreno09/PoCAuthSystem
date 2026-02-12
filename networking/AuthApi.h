#ifndef AUTHAPI_H
#define AUTHAPI_H

#include <functional>
#include "BaseApi.h"
#include "entities/UserSession.h"
#include "entities/AuthTokens.h"

struct LoginResult {
    AuthTokens tokens;
    UserSession user;
};

class AuthApi : public BaseApi
{
    Q_OBJECT

public:
    explicit AuthApi(HttpClient* client, QObject* parent = nullptr);

    void login(const QString& username,
               const QString& password,
               std::function<void(const LoginResult&)> successCb,
               ErrorCb errorCb);

    void refresh(const QString& refreshToken,
                 std::function<void(const LoginResult&)> successCb,
                 ErrorCb errorCb);

    void logout(std::function<void()> successCb,
                ErrorCb errorCb);
};

#endif // AUTHAPI_H
