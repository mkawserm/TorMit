#include "hstorutility.h"

#include <QRandomGenerator>
#include <QCryptographicHash>

HSTorUtility::HSTorUtility()
{

}

HSTorUtility::~HSTorUtility()
{

}

QByteArray HSTorUtility::torControlHashedPasswordNormal(const QByteArray &password)
{
    /**
     * Implementation standard followed from here
     * Link: https://www.ietf.org/rfc/rfc2440.txt
     * See at the section: 3.6.1.3. Iterated and Salted S2K
    **/

    QByteArray salt;
    for(quint8 i=0;i<8;++i)
    {
        salt.append(static_cast<char>(QRandomGenerator::global()->generate()%255));
    }

    if (salt.isNull())
        return QByteArray();

    QByteArray tmp = salt + password;
    int count = (static_cast<quint32>(16) + (96 & 15)) << ((96 >> 4) + 6);

    QCryptographicHash hash(QCryptographicHash::Sha1);
    while (count)
    {
        int c = qMin(count, tmp.size());
        hash.addData(tmp.constData(),c);
        count -= c;
    }
    QByteArray md = hash.result().mid(0,20);
    /* 60 is the hex-encoded value of 96, which is a constant used by Tor's algorithm. */
    return QByteArray("16:") + salt.toHex().toUpper() + QByteArray("60") +md.toHex().toUpper();
}
