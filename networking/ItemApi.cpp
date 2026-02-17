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

    client()->get(ApiEndpoints::Items(), [
        successCb = std::move(successCb),
        errorCb   = std::move(errorCb)
    ](QRestReply& reply) mutable {
        expectArray(reply, errorCb, [&](const QJsonArray& arr) {
            if (!successCb) return;
            QList<Item> items;
            items.reserve(arr.size());
            for (const QJsonValue& val : arr) {
                Item item;
                item.fromJson(val.toObject());
                items.append(item);
            }
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

    client()->post(ApiEndpoints::Items(),
                   QJsonDocument(body).toJson(QJsonDocument::Compact), [
        successCb = std::move(successCb),
        errorCb   = std::move(errorCb)
    ](QRestReply& reply) mutable {
        expectObject(reply, errorCb, [&](const QJsonObject& obj) {
            if (!successCb) return;
            Item item;
            item.fromJson(obj);
            successCb(item);
        });
    });
}

void ItemApi::update(const QString& id,
                     const QString& name,
                     const QString& status,
                     std::function<void(const Item&)> successCb,
                     ErrorCb errorCb)
{
    if (!ensureClient(errorCb)) return;

    QJsonObject body;
    body["name"]   = name;
    body["status"] = status;

    client()->put(ApiEndpoints::Items() + "/" + id,
                  QJsonDocument(body).toJson(QJsonDocument::Compact), [
        successCb = std::move(successCb),
        errorCb   = std::move(errorCb)
    ](QRestReply& reply) mutable {
        expectObject(reply, errorCb, [&](const QJsonObject& obj) {
            if (!successCb) return;
            Item item;
            item.fromJson(obj);
            successCb(item);
        });
    });
}

void ItemApi::remove(const QString& id,
                     std::function<void()> successCb,
                     ErrorCb errorCb)
{
    if (!ensureClient(errorCb)) return;

    client()->remove(ApiEndpoints::Items() + "/" + id, [
        successCb = std::move(successCb),
        errorCb   = std::move(errorCb)
    ](QRestReply& reply) mutable {
        if (!reply.isSuccess()) {
            emitError(errorCb, fromReply(reply));
            return;
        }
        if (successCb) successCb();
    });
}
