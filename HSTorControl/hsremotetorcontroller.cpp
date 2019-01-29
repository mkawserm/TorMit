#include "hsremotetorcontroller.h"

HSRemoteTorController::HSRemoteTorController(QObject *parent) : QObject(parent)
{
    this->m_torControllerReady = false;
    this->m_torControlSocket = new QTcpSocket(this);
}

HSRemoteTorController::~HSRemoteTorController()
{
    if(this->m_torControlSocket->isOpen()){
        this->m_torControlSocket->close();
    }
    delete this->m_torControlSocket;
}

void HSRemoteTorController::connectToHost(const QString &ip, const quint16 &port)
{
    this->m_torControlSocket->connectToHost(ip,port,QIODevice::ReadWrite);
}

bool HSRemoteTorController::authenticate(const QString &password)
{
    Q_UNUSED(password);
    return false;
}
