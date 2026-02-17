#ifndef ITEM_H
#define ITEM_H

#include <QString>
#include <QJsonObject>
#include "IPersistable.h"

class Item : public IPersistable
{
public:
    QString id;
    QString name;
    QString status;

    QJsonObject toJson() const override {
        QJsonObject obj;
        obj["id"] = id;
        obj["name"] = name;
        obj["status"] = status;
        return obj;
    }

    void fromJson(const QJsonObject &obj) override {
        // id arrives as int from PoCServer, convert to string
        id = obj["id"].toVariant().toString();
        name = obj["name"].toString();
        status = obj["status"].toString();
    }
};

#endif // ITEM_H
