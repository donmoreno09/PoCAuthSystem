#ifndef AUTHSTATE_H
#define AUTHSTATE_H

#include <QObject>
#include <QQmlEngine>

class AuthStateEnum
{
    Q_GADGET
    QML_ELEMENT
    QML_UNCREATABLE("Enum-only type")

public:
    enum Value {
        Initializing = 0,
        Unauthenticated,
        AutoLoggingIn,
        LoggingIn,
        Authenticated,
        Error
    };
    Q_ENUM(Value)
};

using AuthState = AuthStateEnum::Value;

#endif // AUTHSTATE_H
