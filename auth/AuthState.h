#ifndef AUTHSTATE_H
#define AUTHSTATE_H

#include <QObject>
#include <QQmlEngine>

namespace AuthStateEnum
{
    Q_NAMESPACE
    QML_ELEMENT

    enum Value {
        Initializing = 0,
        Unauthenticated,
        AutoLoggingIn,
        LoggingIn,
        Authenticated,
        Error
    };
    Q_ENUM_NS(Value)
}

using AuthState = AuthStateEnum::Value;

#endif // AUTHSTATE_H
