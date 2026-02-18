#include "ItemModel.h"
#include "networking/ItemApi.h"
#include <QDebug>

ItemModel::ItemModel(QObject* parent)
    : QAbstractListModel(parent) {}

void ItemModel::initialize(ItemApi* api)
{
    m_api = api;
}

int ItemModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return m_items.size();
}

QVariant ItemModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return {};
    if (index.row() < 0 || index.row() >= m_items.size()) return {};

    const auto& item = m_items[index.row()];
    switch (role) {
    case IdRole:     return item.id;
    case NameRole:   return item.name;
    case StatusRole: return item.status;
    }
    return {};
}

QHash<int, QByteArray> ItemModel::roleNames() const
{
    return {
        { IdRole,     "id"     },
        { NameRole,   "name"   },
        { StatusRole, "status" },
    };
}

void ItemModel::fetch()
{
    if (!m_api) return;
    qDebug().noquote() << "[ItemModel] fetch()";
    setLoading(true);
    setError({});

    m_api->fetchAll([this](const QList<Item>& items) {
        beginResetModel();
        m_items = QVector<Item>(items.begin(), items.end());
        endResetModel();
        setLoading(false);
        qDebug().noquote() << "[ItemModel] fetch → loaded" << m_items.size() << "items";
        emit fetched();
    }, [this](const ErrorResult& err) {
        qWarning().noquote() << "[ItemModel] fetch → error:" << err.message;
        setLoading(false);
        setError(err.message);
    });
}

void ItemModel::create(const QString& name, const QString& status)
{
    if (!m_api) return;
    qDebug().noquote() << "[ItemModel] create() name=" << name << "status=" << status;
    setLoading(true);
    setError({});

    m_api->create(name, status, [this](const Item& item) {
        const int row = m_items.size();
        beginInsertRows({}, row, row);
        m_items.append(item);
        endInsertRows();
        setLoading(false);
        qDebug().noquote() << "[ItemModel] create → success  id=" << item.id
                           << "row=" << row;
        emit created();
    }, [this](const ErrorResult& err) {
        qWarning().noquote() << "[ItemModel] create → error:" << err.message;
        setLoading(false);
        setError(err.message);
    });
}

void ItemModel::update(const QString& id, const QString& name)
{
    if (!m_api) return;
    qDebug().noquote() << "[ItemModel] update() id=" << id << "name=" << name;
    setLoading(true);
    setError({});

    m_api->update(id, name, [this, id](const Item& updated) {
        for (int i = 0; i < m_items.size(); ++i) {
            if (m_items[i].id == id) {
                m_items[i] = updated;
                const QModelIndex idx = index(i);
                emit dataChanged(idx, idx);
                qDebug().noquote() << "[ItemModel] update → success  id=" << id
                                   << "row=" << i;
                break;
            }
        }
        setLoading(false);
        emit this->updated();
    }, [this](const ErrorResult& err) {
        qWarning().noquote() << "[ItemModel] update → error:" << err.message;
        setLoading(false);
        setError(err.message);
    });
}

void ItemModel::remove(const QString& id)
{
    if (!m_api) return;
    qDebug().noquote() << "[ItemModel] remove() id=" << id;
    setLoading(true);
    setError({});

    m_api->remove(id, [this, id]() {
        for (int i = 0; i < m_items.size(); ++i) {
            if (m_items[i].id == id) {
                beginRemoveRows({}, i, i);
                m_items.removeAt(i);
                endRemoveRows();
                qDebug().noquote() << "[ItemModel] remove → success  id=" << id
                                   << "row=" << i;
                break;
            }
        }
        setLoading(false);
        emit removed();
    }, [this](const ErrorResult& err) {
        qWarning().noquote() << "[ItemModel] remove → error:" << err.message;
        setLoading(false);
        setError(err.message);
    });
}

bool ItemModel::loading() const { return m_loading; }
QString ItemModel::error() const { return m_error; }

void ItemModel::setLoading(bool value)
{
    if (m_loading == value) return;
    m_loading = value;
    emit loadingChanged();
}

void ItemModel::setError(const QString& message)
{
    if (m_error == message) return;
    m_error = message;
    emit errorChanged();
}
