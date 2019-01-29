#include "hstorhiddenservice.h"

HSTorHiddenService::HSTorHiddenService()
{

}

void HSTorHiddenService::setHiddenServiceDir(const QString &dir)
{
    this->m_hiddenServiceDir = dir;
}

void HSTorHiddenService::addHiddenService(const quint16 &port, const QString &serviceIp, const quint16 &servicePort)
{
    this->m_portList<<port;
    this->m_serviceIpList<<serviceIp;
    this->m_servicePortList<<servicePort;
}

QString HSTorHiddenService::toString() const
{
    QString rdata;
    if(this->m_hiddenServiceDir.isEmpty()) return rdata;

    rdata.append("HiddenServiceDir ");
    rdata.append(this->m_hiddenServiceDir);
    rdata.append('\n');

    if(this->m_portList.size() == this->m_servicePortList.size()){
        for(int i=0;i<this->m_portList.length();++i)
        {
            rdata.append("HiddenServicePort ");
            rdata.append(this->m_portList.at(i));
            rdata.append(' ');
            rdata.append(this->m_serviceIpList.at(i));
            rdata.append(':');
            rdata.append(this->m_servicePortList.at(i));
            rdata.append('\n');
        }
    }
    return rdata;
}

HSTorHiddenService::~HSTorHiddenService()
{
    this->m_portList.clear();
    this->m_serviceIpList.clear();
    this->m_servicePortList.clear();
}
