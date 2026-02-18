#include "ItemApi.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include "ApiEndpoints.h"

ItemApi::ItemApi(HttpClient* client, QObject* parent)
    : BaseApi(client, parent) {}

void ItemApi::fetchAll(std::function<void(const QList<Item>&)> successCb,
                       ErrorCb errorCb)
{
    if (!ensureClient(errorCb)) return;

    const QString url = ApiEndpoints::Items();
    qDebug().noquote() << "[ItemApi] GET" << url;

    client()->get(url, [
        successCb = std::move(successCb),
        errorCb   = std::move(errorCb)
    ](QRestReply& reply) mutable {
        qDebug().noquote() << "[ItemApi] ←" << reply.httpStatus() << "GET /api/items";

        expectArray(reply, errorCb, [&](const QJsonArray& arr) {
            if (!successCb) return;
            QList<Item> items;
            items.reserve(arr.size());
            for (const QJsonValue& val : arr) {
                Item item;
                item.fromJson(val.toObject());
                items.append(item);
            }
            qDebug().noquote() << "[ItemApi] ← parsed" << items.size() << "items";
            successCb(items);
        });
    });
}

void ItemApi::create(const QString& name,
                     const QString& status,
                     std::function<void(const Item&)> successCb,
                     ErrorCb errorCb)
{
    if (!ensureClient(errorCb)) return;

    QJsonObject body;
    body["name"]   = name;
    body["status"] = status;

    const QByteArray payload = QJsonDocument(body).toJson(QJsonDocument::Compact);
    const QString url = ApiEndpoints::Items();
    qDebug().noquote() << "[ItemApi] POST" << url;
    qDebug().noquote() << "[ItemApi] → body:" << payload;

    client()->post(url, payload, [
        successCb = std::move(successCb),
        errorCb   = std::move(errorCb)
    ](QRestReply& reply) mutable {
        qDebug().noquote() << "[ItemApi] ←" << reply.httpStatus() << "POST /api/items";

        expectObject(reply, errorCb, [&](const QJsonObject& obj) {
            if (!successCb) return;
            Item item;
            item.fromJson(obj);
            qDebug().noquote() << "[ItemApi] ← created item id=" << item.id
                               << "name=" << item.name
                               << "status=" << item.status;
            successCb(item);
        });
    });
}

void ItemApi::update(const QString& id,
                     const QString& name,
                     std::function<void(const Item&)> successCb,
                     ErrorCb errorCb)
{
    if (!ensureClient(errorCb)) return;

    QJsonObject body;
    body["name"] = name;

    const QByteArray payload = QJsonDocument(body).toJson(QJsonDocument::Compact);
    const QString url = ApiEndpoints::Items() + "/" + id;
    qDebug().noquote() << "[ItemApi] PUT" << url;
    qDebug().noquote() << "[ItemApi] → body:" << payload;

    client()->put(url, payload, [
        successCb = std::move(successCb),
        errorCb   = std::move(errorCb)
    ](QRestReply& reply) mutable {
        qDebug().noquote() << "[ItemApi] ←" << reply.httpStatus() << "PUT /api/items/:id";

        expectObject(reply, errorCb, [&](const QJsonObject& obj) {
            if (!successCb) return;
            Item item;
            item.fromJson(obj);
            qDebug().noquote() << "[ItemApi] ← updated item id=" << item.id
                               << "name=" << item.name
                               << "status=" << item.status;
            successCb(item);
        });
    });
}

void ItemApi::remove(const QString& id,
                     std::function<void()> successCb,
                     ErrorCb errorCb)
{
    if (!ensureClient(errorCb)) return;

    const QString url = ApiEndpoints::Items() + "/" + id;
    qDebug().noquote() << "[ItemApi] DELETE" << url;

    client()->remove(url, [
        id,
        successCb = std::move(successCb),
        errorCb   = std::move(errorCb)
    ](QRestReply& reply) mutable {
        qDebug().noquote() << "[ItemApi] ←" << reply.httpStatus() << "DELETE /api/items/" + id;

        if (!reply.isSuccess()) {
            emitError(errorCb, fromReply(reply));
            return;
        }
        if (successCb) successCb();
    });
}
