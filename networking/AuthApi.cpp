#include "AuthApi.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include "ApiEndpoints.h"

AuthApi::AuthApi(HttpClient* client, QObject* parent)
    : BaseApi(client, parent) {}

void AuthApi::login(const QString& username,
                    const QString& password,
                    std::function<void(const LoginResult&)> successCb,
                    ErrorCb errorCb)
{
    if (!ensureClient(errorCb)) return;

    QJsonObject body;
    body["username"] = username;
    body["password"] = password;

    client()->post(ApiEndpoints::AuthLogin(), QJsonDocument(body).toJson(QJsonDocument::Compact), [
        successCb = std::move(successCb),
        errorCb = std::move(errorCb)
    ](QRestReply& reply) mutable {
        expectObject(reply, errorCb, [&](const QJsonObject& obj) {
            if (!successCb) return;

            LoginResult result;
            result.tokens.accessToken = obj["accessToken"].toString();
            result.tokens.refreshToken = obj["refreshToken"].toString();
            result.tokens.expiresIn = obj["expiresIn"].toInt();
            result.user.fromJson(obj["user"].toObject());

            successCb(result);
        });
    });
}

void AuthApi::refresh(const QString& refreshToken,
                      std::function<void(const LoginResult&)> successCb,
                      ErrorCb errorCb)
{
    if (!ensureClient(errorCb)) return;

    QJsonObject body;
    body["refreshToken"] = refreshToken;

    client()->post(ApiEndpoints::AuthRefresh(), QJsonDocument(body).toJson(QJsonDocument::Compact), [
        successCb = std::move(successCb),
        errorCb = std::move(errorCb)
    ](QRestReply& reply) mutable {
        expectObject(reply, errorCb, [&](const QJsonObject& obj) {
            if (!successCb) return;

            LoginResult result;
            result.tokens.accessToken = obj["accessToken"].toString();
            result.tokens.refreshToken = obj["refreshToken"].toString();
            result.tokens.expiresIn = obj["expiresIn"].toInt();
            result.user.fromJson(obj["user"].toObject());

            successCb(result);
        });
    });
}

void AuthApi::logout(std::function<void()> successCb, ErrorCb errorCb)
{
    if (!ensureClient(errorCb)) return;

    client()->post(ApiEndpoints::AuthLogout(), QByteArray{}, [
        successCb = std::move(successCb),
        errorCb = std::move(errorCb)
    ](QRestReply& reply) mutable {
        if (!reply.isSuccess()) {
            emitError(errorCb, fromReply(reply));
            return;
        }

        if (successCb) successCb();
    });
}
