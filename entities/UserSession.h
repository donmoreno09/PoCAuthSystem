#ifndef USERSESSION_H
#define USERSESSION_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include "IPersistable.h"

class UserSession : public IPersistable
{
public:
    QString userId;
    QString username;
    QString displayName;
    QString email;
    QStringList roles;
    QStringList permissions;

    bool isValid() const { return !userId.isEmpty() && !username.isEmpty(); }

    void clear()
    {
        userId.clear();
        username.clear();
        displayName.clear();
        email.clear();
        roles.clear();
        permissions.clear();
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["id"] = userId;
        obj["username"] = username;
        obj["displayName"] = displayName;
        obj["email"] = email;
        obj["roles"] = QJsonArray::fromStringList(roles);
        obj["permissions"] = QJsonArray::fromStringList(permissions);
        return obj;
    }

    void fromJson(const QJsonObject& obj) override
    {
        userId = obj["id"].toString();
        username = obj["username"].toString();
        displayName = obj["displayName"].toString();
        email = obj["email"].toString();

        roles.clear();
        for (const auto& v : obj["roles"].toArray())
            if (v.isString()) roles.append(v.toString());

        permissions.clear();
        for (const auto& v : obj["permissions"].toArray())
            if (v.isString()) permissions.append(v.toString());
    }
};

#endif // USERSESSION_H
