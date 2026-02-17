#ifndef ITEMAPI_H
#define ITEMAPI_H

#include <functional>
#include <QList>
#include "BaseApi.h"
#include "entities/Item.h"

class ItemApi : public BaseApi
{
    Q_OBJECT

public:
    explicit ItemApi(HttpClient* client, QObject* parent = nullptr);

    void fetchAll(std::function<void(const QList<Item>&)> successCb,
                  ErrorCb errorCb);

    void create(const QString& name,
                const QString& status,
                std::function<void(const Item&)> successCb,
                ErrorCb errorCb);

    void update(const QString& id,
                const QString& name,
                const QString& status,
                std::function<void(const Item&)> successCb,
                ErrorCb errorCb);

    void remove(const QString& id,
                std::function<void()> successCb,
                ErrorCb errorCb);
};

#endif // ITEMAPI_H
