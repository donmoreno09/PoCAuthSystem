#ifndef AUTHTOKENS_H
#define AUTHTOKENS_H

#include <QString>

struct AuthTokens {
    QString accessToken;
    QString refreshToken;
    int expiresIn = 0;

    bool isValid() const { return !accessToken.isEmpty(); }

    void clear()
    {
        accessToken.clear();
        refreshToken.clear();
        expiresIn = 0;
    }
};

#endif // AUTHTOKENS_H
