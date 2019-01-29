#ifndef HSTORHIDDENSERVICE_H
#define HSTORHIDDENSERVICE_H

#include <QtCore>
#include <QString>
#include <QStringList>

class HSTorHiddenService
{
private:
    QString m_hiddenServiceDir;
    QList<quint16> m_portList;
    QList<QString> m_serviceIpList;
    QList<quint16> m_servicePortList;

public:
    explicit HSTorHiddenService();
    virtual ~HSTorHiddenService();

    void setHiddenServiceDir(const QString &dir);
    void addHiddenService(const quint16 &port, const QString &serviceIp, const quint16 &servicePort);

    QString toString() const;
};

#endif // HSTORHIDDENSERVICE_H
