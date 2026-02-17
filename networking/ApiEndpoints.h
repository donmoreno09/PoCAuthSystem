#ifndef APIENDPOINTS_H
#define APIENDPOINTS_H

#include <QString>

namespace ApiEndpoints {
    extern QString BaseUrl;

    QString AuthLogin();
    QString AuthRefresh();
    QString AuthLogout();

    QString Items();
}

#endif // APIENDPOINTS_H
