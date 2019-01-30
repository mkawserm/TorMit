#ifndef HSTORUTILITY_H
#define HSTORUTILITY_H

#include <QByteArray>

class HSTorUtility
{
public:
    explicit HSTorUtility();
    virtual ~HSTorUtility();

    static QByteArray torControlHashedPasswordNormal(const QByteArray &password);

};

#endif // HSTORUTILITY_H
