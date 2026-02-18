#ifndef ITEMMODEL_H
#define ITEMMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QQmlEngine>
#include "entities/Item.h"

class ItemApi;

class ItemModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Use as singleton.")
    QML_SINGLETON

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged FINAL)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged FINAL)

public:
    explicit ItemModel(QObject* parent = nullptr);

    void initialize(ItemApi* api);

    enum Roles {
        IdRole     = Qt::UserRole + 1,
        NameRole,
        StatusRole,
    };
    Q_ENUM(Roles)

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void fetch();
    Q_INVOKABLE void create(const QString& name, const QString& status);
    Q_INVOKABLE void update(const QString& id, const QString& name);
    Q_INVOKABLE void remove(const QString& id);

    bool loading() const;
    QString error() const;

signals:
    void loadingChanged();
    void errorChanged();
    void fetched();
    void created();
    void updated();
    void removed();

private:
    void setLoading(bool value);
    void setError(const QString& message);

    ItemApi*      m_api = nullptr;
    QVector<Item> m_items;
    bool          m_loading = false;
    QString       m_error;
};

#endif // ITEMMODEL_H
