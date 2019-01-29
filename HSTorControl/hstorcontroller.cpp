#include "hstorcontroller.h"
#include "hstorprocess.h"

#include <QDir>
#include <QUuid>
#include <QDebug>
#include <QDateTime>
#include <QRandomGenerator>
#include <QCoreApplication>


HSTorController::HSTorController(QObject *parent) : QObject(parent)
{

#if defined(Q_OS_WIN32)
    this->m_torExe = QCoreApplication::applicationDirPath()+QLatin1String("/tor.exe");
#elif defined (Q_OS_ANDROID)
    this->m_torExe = QCoreApplication::applicationDirPath()+QLatin1String("/tor");
#elif defined (Q_OS_MAC) || defined (Q_OS_LINUX)
    this->m_torExe = QCoreApplication::applicationDirPath()+QLatin1String("/tor");
#endif

    this->m_torControllerReady = false;
    this->m_torControlSocket = new QTcpSocket(this);
    this->m_torProcess = new HSTorProcess(this);
    this->m_torProcess->setTorExe(this->m_torExe);
    this->m_torProcess->setTorPort(60050);
    this->m_torProcess->setTorControlPort(60051);

    this->connect(this->m_torProcess,&HSTorProcess::torProcessStarted,this,&HSTorController::torProcessStarted);
    this->connect(this->m_torProcess,&HSTorProcess::torProcessFinished,this,&HSTorController::torProcessFinished);
    this->connect(this->m_torProcess,&HSTorProcess::torProcessFailedToStart,this,&HSTorController::torProcessFailedToStart);
    this->connect(this->m_torProcess,&HSTorProcess::torProcessBootstrapStatus,this,&HSTorController::torProcessBootstrapStatusSlot);

    this->connect(this->m_torProcess,&HSTorProcess::torHeartBeat,this,&HSTorController::torHeartBeat);
    this->connect(this->m_torProcess,&HSTorProcess::torProcessError,this,&HSTorController::torProcessError);
    this->connect(this->m_torProcess,&HSTorProcess::torProcessMessage,this,&HSTorController::torProcessMessage);



    this->connect(this->m_torControlSocket,&QTcpSocket::connected,[this](){
        QString auth = QString("AUTHENTICATE \"%1\"\r\n").arg(this->m_torProcess->getTorControlPassword());
        this->m_torControlSocket->write(auth.toLocal8Bit());
        this->m_torControlSocket->waitForBytesWritten();
        this->m_torControlSocket->waitForReadyRead();
        QString r = this->m_torControlSocket->readAll();

        if(r == QLatin1String("250 OK\r\n"))
        {
            this->m_torControllerReady = true;
            this->heartbeat();
            emit torControllerReady();
        }
        else
        {
            this->m_torControllerReady = false;
            emit error(QLatin1String("Tor controller is not ready. Authentication failure. Restart your Tor router."));
        }
    });

    this->connect(this->m_torControlSocket,&QTcpSocket::disconnected,[this](){
        this->m_torControllerReady = false;
    });
}

HSTorController::~HSTorController()
{
    qDebug() << "HSTorController::~HSTorController() begin";

    this->halt();
    delete  this->m_torControlSocket;
    delete this->m_torProcess;

    qDebug() << "HSTorController::~HSTorController() end";
}

void HSTorController::startTorProcess()
{
    if(!this->m_torProcess->isTorProcessRunning())
    {
        this->m_torProcess->setTorControlPassword(this->generateRandomPassword());
        this->m_torProcess->start();
    }
}

void HSTorController::stopTorProcess()
{
    this->m_torProcess->stop();
}

bool HSTorController::dump()
{
    if(this->m_torControllerReady)
    {
        this->m_torControlSocket->write("SIGNAL DUMP\r\n");
        this->m_torControlSocket->waitForBytesWritten();
        this->m_torControlSocket->waitForReadyRead();
        return this->m_torControlSocket->readAll() == QByteArray("250 OK\r\n");
    }
    else
    {
        return false;
    }
}

bool HSTorController::halt()
{
    if(this->m_torControllerReady)
    {
        this->m_torControlSocket->write("SIGNAL HALT\r\n");
        this->m_torControlSocket->waitForBytesWritten();
        this->m_torControlSocket->waitForReadyRead();
        return this->m_torControlSocket->readAll() == QByteArray("250 OK\r\n");
    }
    else
    {
        return false;
    }
}

bool HSTorController::reload()
{
    if(this->m_torControllerReady)
    {
        this->m_torControlSocket->write("SIGNAL RELOAD\r\n");
        this->m_torControlSocket->waitForBytesWritten();
        this->m_torControlSocket->waitForReadyRead();
        return this->m_torControlSocket->readAll() == QByteArray("250 OK\r\n");
    }
    else
    {
        return false;
    }
}

bool HSTorController::newnym()
{
    if(this->m_torControllerReady)
    {
        this->m_torControlSocket->write("SIGNAL NEWNYM\r\n");
        this->m_torControlSocket->waitForBytesWritten();
        this->m_torControlSocket->waitForReadyRead();
        return this->m_torControlSocket->readAll() == QByteArray("250 OK\r\n");
    }
    else
    {
        return false;
    }
}

bool HSTorController::heartbeat()
{
    if(this->m_torControllerReady)
    {
        this->m_torControlSocket->write("SIGNAL HEARTBEAT\r\n");
        this->m_torControlSocket->waitForBytesWritten();
        this->m_torControlSocket->waitForReadyRead();
        return this->m_torControlSocket->readAll() == QByteArray("250 OK\r\n");
    }
    else
    {
        return false;
    }
}

QMap<QString, QString> HSTorController::addNewOnionService(const quint16 &virtualPort,
                                                           const quint16 &targetPort)
{
    return this->addNewOnionService(virtualPort,QLatin1String("127.0.0.1"),targetPort);
}

QMap<QString, QString> HSTorController::addNewOnionService(const quint16 &virtualPort,
                                                           const QString &ip,
                                                           const quint16 &targetPort)
{
    QMap<QString,QString> rval;

    if(this->m_torControllerReady){
        QString c = QString("ADD_ONION NEW:ED25519-V3 Port=%1,%2:%3\r\n").arg(virtualPort).arg(ip).arg(targetPort);
        this->m_torControlSocket->write(c.toLocal8Bit());
        this->m_torControlSocket->waitForBytesWritten();
        this->m_torControlSocket->waitForReadyRead();
        QString rdata = this->m_torControlSocket->readAll();
        if(rdata.contains(QLatin1String("250 OK")))
        {
            QStringList rlist = rdata.split(QLatin1String("\r\n"));
            for (const QString &rs: rlist)
            {
                if(rs.contains(QLatin1String("250-ServiceID=")))
                {
                    QString sid = rs.mid(QString("250-ServiceID=").length());
                    rval.insert("ServiceID",sid);
                    emit onionServiceId(sid);
                }
                else if(rs.contains(QLatin1String("250-PrivateKey=")))
                {
                    rval.insert("PrivateKey",rs.mid(QString("250-PrivateKey=").length()));
                }
            }
        }
        else
        {
            emit error(rdata);
        }
    }
    else{
        emit error("Controller is not ready.");
    }

    return rval;
}

bool HSTorController::startOnionService(const QString &privateKey,
                                        const quint16 &virtualPort,
                                        const quint16 &targetPort)
{
    return this->startOnionService(privateKey,virtualPort,QLatin1String("127.0.0.1"),targetPort);
}

bool HSTorController::startOnionService(const QString &privateKey,
                                        const quint16 &virtualPort,
                                        const QString &ip,
                                        const quint16 &targetPort)
{
    if(this->m_torControllerReady){
        QString c = QString("ADD_ONION %1 Port=%2,%3:%4\r\n")
                .arg(privateKey)
                .arg(virtualPort)
                .arg(ip)
                .arg(targetPort);
        //qDebug() << c;
        this->m_torControlSocket->write(c.toLocal8Bit());
        this->m_torControlSocket->waitForBytesWritten();
        this->m_torControlSocket->waitForReadyRead();
        QString rdata = this->m_torControlSocket->readAll();
        if(rdata.contains(QLatin1String("250 OK")))
        {
            QStringList rlist = rdata.split(QLatin1String("\r\n"));
            for (const QString &rs: rlist)
            {
                if(rs.contains(QLatin1String("250-ServiceID=")))
                {
                    QString sid = rs.mid(QString("250-ServiceID=").length());
                    emit onionServiceId(sid);
                }
            }

            return true;
        }
        else
        {
            emit error(rdata);
        }
    }

    return false;
}

bool HSTorController::stopOnionService(const QString &serviceId)
{
    if(this->m_torControllerReady){
        QString c = QString("DEL_ONION %1\r\n")
                .arg(serviceId);
        this->m_torControlSocket->write(c.toLocal8Bit());
        this->m_torControlSocket->waitForBytesWritten();
        this->m_torControlSocket->waitForReadyRead();
        QString rdata = this->m_torControlSocket->readAll();
        if(rdata.contains(QLatin1String("250 OK")))
        {
            return true;
        }
        else
        {
            emit error(rdata);
        }
    }

    return false;
}

void HSTorController::setTorPort(const quint16 &port)
{
    if(!this->m_torProcess->isTorProcessRunning())
    {
        this->m_torProcess->setTorPort(port);
    }
}

void HSTorController::setTorControlPort(const quint16 &controlport)
{
    if(!this->m_torProcess->isTorProcessRunning())
    {
        this->m_torProcess->setTorControlPort(controlport);
    }
}

void HSTorController::setTorDataDirecotry(const QString &dataDir)
{
    if(!this->m_torProcess->isTorProcessRunning())
    {
        {
            QDir d(dataDir);
            if(!d.exists())
            {
                d.mkpath(dataDir);
            }
        }
        this->m_torProcess->setTorDataDirectory(dataDir);
        this->m_torProcess->setTorrcFilePath(dataDir+QLatin1String("/torrc"));
    }
}

QNetworkProxy HSTorController::onionProxy() const
{
    QNetworkProxy proxy(QNetworkProxy::Socks5Proxy,QLatin1String("127.0.0.1"),this->m_torProcess->getTorPort());
    return proxy;
}

void HSTorController::setTorExe(const QString &torExe)
{
    if(!this->m_torProcess->isTorProcessRunning())
    {
        this->m_torExe = torExe;
        this->m_torProcess->setTorExe(torExe);
    }
}

void HSTorController::torProcessBootstrapStatusSlot(const int &status)
{
    if(status == 100)
    {
        this->m_torControlSocket->connectToHost(QLatin1String("127.0.0.1"),this->m_torProcess->getTorControlPort());
        emit torProcessBootstrapStatus(status);
    }
}

QString HSTorController::generateRandomPassword()
{
    QString p;
    p = QUuid::createUuid().toByteArray().toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
    return p;
}
