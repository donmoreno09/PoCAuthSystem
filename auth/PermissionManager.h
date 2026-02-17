#ifndef PERMISSIONMANAGER_H
#define PERMISSIONMANAGER_H

#include <QObject>
#include <QSet>
#include <QStringList>
#include <QQmlEngine>

class PermissionManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(int revision READ revision NOTIFY permissionsChanged)

public:
    explicit PermissionManager(QObject* parent = nullptr);

    Q_INVOKABLE bool hasPermission(const QString& permission) const;
    Q_INVOKABLE bool hasRole(const QString& role) const;
    Q_INVOKABLE bool hasAnyPermission(const QStringList& permissions) const;

    void loadFromSession(const QStringList& roles, const QStringList& permissions);
    void clear();

    int revision() const { return m_revision; }

signals:
    void permissionsChanged();

private:
    QSet<QString> m_permissions;
    QSet<QString> m_roles;
    int m_revision = 0;
};

#endif // PERMISSIONMANAGER_H
