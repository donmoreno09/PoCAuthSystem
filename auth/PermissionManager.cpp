#include "PermissionManager.h"

PermissionManager::PermissionManager(QObject* parent)
    : QObject(parent) {}

bool PermissionManager::hasPermission(const QString& permission) const
{
    return m_permissions.contains(permission);
}

bool PermissionManager::hasRole(const QString& role) const
{
    return m_roles.contains(role);
}

bool PermissionManager::hasAnyPermission(const QStringList& permissions) const
{
    for (const auto& p : permissions) {
        if (m_permissions.contains(p))
            return true;
    }
    return false;
}

void PermissionManager::loadFromSession(const QStringList& roles, const QStringList& permissions)
{
    m_roles = QSet<QString>(roles.begin(), roles.end());
    m_permissions = QSet<QString>(permissions.begin(), permissions.end());
    emit permissionsChanged();
}

void PermissionManager::clear()
{
    m_roles.clear();
    m_permissions.clear();
    emit permissionsChanged();
}
