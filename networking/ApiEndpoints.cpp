#include "ApiEndpoints.h"

namespace ApiEndpoints {
    QString BaseUrl = "http://localhost:7000";

    QString AuthLogin()   { return BaseUrl + "/auth/login"; }
    QString AuthRefresh() { return BaseUrl + "/auth/refresh"; }
    QString AuthLogout()  { return BaseUrl + "/auth/logout"; }

    QString Items()       { return BaseUrl + "/api/items"; }
}
