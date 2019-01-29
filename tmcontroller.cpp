#include "tmcontroller.h"
#include "tmcentralwidget.h"

#include <QDebug>

TMController::TMController(QObject *parent) : QObject(parent)
{
    this->m_mainWindow = Q_NULLPTR;
    this->m_centralWidget = Q_NULLPTR;
    this->m_messageWindow = Q_NULLPTR;
    this->m_logWindow = Q_NULLPTR;
    this->m_typingTimer = QDateTime::currentMSecsSinceEpoch();

    this->m_server = new QWebSocketServer(QLatin1String("TorMit"),QWebSocketServer::NonSecureMode,this);
    this->m_server->setProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy,QLatin1String("127.0.0.1"),9050));
    //this->m_server->setMaxPendingConnections(1);

    this->connect(this->m_server,&QWebSocketServer::newConnection,this,&TMController::newConnection);

    this->m_currentConnection = Q_NULLPTR;

    this->m_torController.setTorPort(9050);
    this->m_torController.setTorControlPort(9051);

    this->m_torController.setTorDataDirecotry(
                QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+QLatin1String("/TorMit")
                );

    this->m_serviceFilePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
            +QLatin1String("/TorMit/TorMit.cnf");

    this->connect(&this->m_torController,&HSTorController::torProcessStarted,this,&TMController::torProcessStarted);
    this->connect(&this->m_torController,&HSTorController::torProcessFinished,this,&TMController::torProcessFinished);
    this->connect(&this->m_torController,&HSTorController::torProcessFailedToStart,this,&TMController::torProcessFailedToStart);

    this->connect(&this->m_torController,&HSTorController::torProcessBootstrapStatus,this,&TMController::torProcessBootstrapStatus);

    this->connect(&this->m_torController,&HSTorController::torProcessError,this,&TMController::torProcessError);
    this->connect(&this->m_torController,&HSTorController::torProcessMessage,this,&TMController::torProcessMessage);
    this->connect(&this->m_torController,&HSTorController::torHeartBeat,this,&TMController::torHeartBeat);
    this->connect(&this->m_torController,&HSTorController::error,this,&TMController::error);
    this->connect(&this->m_torController,&HSTorController::onionServiceId,this,&TMController::onionServiceId);
    this->connect(&this->m_torController,&HSTorController::torControllerReady,this,&TMController::torControllerReady);

    this->m_server->listen(QHostAddress("127.0.0.1"),1971);
}

TMController::~TMController()
{
    this->m_server->close();
    delete this->m_server;

    if(this->m_currentConnection)
    {
        this->m_currentConnection->close();
    }
    delete this->m_currentConnection;
}

void TMController::setMainWindow(TMMainWindow *mainWindow)
{
    this->m_mainWindow = mainWindow;
    this->m_centralWidget = dynamic_cast<TMCentralWidget*>(mainWindow->centralWidget());

    this->m_logWindow = dynamic_cast<TMCentralWidget*>(mainWindow->centralWidget())->getLogWindow();
    this->m_messageWindow = dynamic_cast<TMCentralWidget*>(mainWindow->centralWidget())->getMessageWindow();

    this->m_logWindow->clearMessage();
    this->m_logWindow->addTorStatus("Diconnected");
    this->m_logWindow->addLog("TorMit started.");

    this->m_messageWindow->clearMessage();
    this->m_messageWindow->clearServiceId();
    this->m_messageWindow->setConnectButtonText("CONNECT");

    this->m_messageWindow->disableMessageWindow();

    this->m_centralWidget->setCurrentIndex(1);

    this->connect(this->m_messageWindow,&TMMessageWindow::sendButtonClicked,this,&TMController::sendButtonClicked);
    this->connect(this->m_messageWindow,&TMMessageWindow::connectButtonClicked,this,&TMController::connectButtonClicked);
    this->connect(this->m_messageWindow,&TMMessageWindow::typingMessage,this,&TMController::typingMessage);
}

void TMController::start()
{
    this->m_torController.startTorProcess();
}

void TMController::torProcessStarted()
{
    this->m_logWindow->addLog("Tor process started.");
}

void TMController::torProcessFinished()
{
    this->m_logWindow->addLog("Tor process finished.");
    this->m_logWindow->addTorStatus("Diconnected");
}

void TMController::torProcessFailedToStart()
{
    this->m_logWindow->addTorStatus("Diconnected");
    this->m_logWindow->setMessage("Restart TorMit.");
}

void TMController::torProcessBootstrapStatus(const int &status)
{
    if(status == 100)
    {
    }
}

void TMController::torProcessError(const QString &errorText)
{
    this->m_logWindow->addLog(errorText);
}

void TMController::torProcessMessage(const QString &messageText)
{
    this->m_logWindow->addLog(messageText);
}

void TMController::torHeartBeat(const QString &uptime,
                                const QString &circuits,
                                const QString &dataSent,
                                const QString &dataReceived)
{
    Q_UNUSED(uptime);
    Q_UNUSED(circuits);
    Q_UNUSED(dataSent);
    Q_UNUSED(dataReceived);

}

void TMController::error(const QString &errorText)
{
    this->m_logWindow->addLog(errorText);
}

void TMController::onionServiceId(const QString &serviceId)
{
    this->m_messageWindow->setMyServiceId(serviceId);
}

void TMController::sendButtonClicked()
{
    QString message = this->m_messageWindow->getInputMessage();
    this->m_messageWindow->clearInputMessage();
    this->m_messageWindow->addMessage(QString("1st: %1").arg(message));

    if(this->m_currentConnection)
    {
        this->m_typingTimer = QDateTime::currentMSecsSinceEpoch();
        this->m_currentConnection->sendTextMessage(message);
    }
}

void TMController::connectButtonClicked()
{
    //qDebug() << "void TMController::connectButtonClicked()";
    if(this->m_currentConnection)
    {
        this->m_currentConnection->close();
    }
    else
    {
        if(!this->m_messageWindow->getServiceId().isEmpty())
        {
            this->m_messageWindow->disableConnectButton();
            this->m_messageWindow->disableServiceId();
            this->m_messageWindow->clearMessage();
            this->m_currentConnection = new QWebSocket();
            this->m_currentConnection->setProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy,QLatin1String("127.0.0.1"),9050));
            this->connect(this->m_currentConnection,&QWebSocket::connected,this,&TMController::socketConnected);
            this->connect(this->m_currentConnection,&QWebSocket::disconnected,this,&TMController::socketDisconnected);
            this->connect(this->m_currentConnection,&QWebSocket::textMessageReceived,this,&TMController::textMessageReceived);
            this->connect(this->m_currentConnection,&QWebSocket::binaryMessageReceived,this,&TMController::binaryMessageReceived);
            this->connect(this->m_currentConnection,&QWebSocket::pong,this,&TMController::pong);

            this->m_mainWindow->setStatusBarText("Connecting...",0);
            this->m_currentConnection->open(
                        QUrl(QString("ws://%1.onion:1971").arg(this->m_messageWindow->getServiceId()))
                        );
        }
    }
}

void TMController::torControllerReady()
{
    QString torServicePrivateKey;
    bool onionStarted = false;
    if(QFile::exists(this->m_serviceFilePath))
    {
        QFile f(this->m_serviceFilePath);
        if(f.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            torServicePrivateKey = f.readAll();
            f.close();
        }
    }

    if(torServicePrivateKey.isEmpty())
    {
        QMap<QString,QString> r = this->m_torController.addNewOnionService(1971,1971);

        if(r.contains(QLatin1String("PrivateKey")))
        {
            torServicePrivateKey = r.value(QLatin1String("PrivateKey"));
            QFile f(this->m_serviceFilePath);
            if(f.open(QIODevice::WriteOnly|QIODevice::Text))
            {
                f.write(torServicePrivateKey.toLocal8Bit());
                f.close();
            }
            onionStarted = true;
        }
    }
    else
    {
        onionStarted = this->m_torController.startOnionService(torServicePrivateKey,1971,1971);
    }


    if(onionStarted)
    {
        this->m_logWindow->addTorStatus("Connected");
        this->m_messageWindow->enableOnlyConnectWindow();
        this->m_centralWidget->setCurrentIndex(0);
    }
    else
    {
        this->m_logWindow->addTorStatus("Diconnected");
        this->m_messageWindow->disableMessageWindow();
        this->m_centralWidget->setCurrentIndex(1);
        this->m_logWindow->setMessage("Restart TorMit.");
    }

}

void TMController::newConnection()
{
    qDebug() << "newConnection";

    if(this->m_currentConnection)
    {
        if(this->m_server->hasPendingConnections())
        {
            QWebSocket *nc = this->m_server->nextPendingConnection();
            nc->setProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy,QLatin1String("127.0.0.1"),9050));
            nc->sendTextMessage("Try later.");
            nc->close();
            nc->deleteLater();
        }
    }
    else
    {
        if(this->m_server->hasPendingConnections())
        {
            this->m_currentConnection = this->m_server->nextPendingConnection();
            this->m_currentConnection->setProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy,QLatin1String("127.0.0.1"),9050));
            this->connect(this->m_currentConnection,&QWebSocket::connected,this,&TMController::socketConnected);
            this->connect(this->m_currentConnection,&QWebSocket::disconnected,this,&TMController::socketDisconnected);
            this->connect(this->m_currentConnection,&QWebSocket::textMessageReceived,this,&TMController::textMessageReceived);
            this->connect(this->m_currentConnection,&QWebSocket::binaryMessageReceived,this,&TMController::binaryMessageReceived);
            this->connect(this->m_currentConnection,&QWebSocket::pong,this,&TMController::pong);
            this->m_messageWindow->setConnectedState();
            this->m_currentConnection->sendBinaryMessage(QByteArray("2"));
            this->m_mainWindow->setStatusBarText("Connected.",0);
            this->m_messageWindow->clearMessage();

            qDebug() << "PEER ADDRESS:"<<this->m_currentConnection->peerAddress().toString();
        }
    }
}

void TMController::textMessageReceived(const QString &message)
{
    //qDebug() << "Message Received:"<<message;
    this->m_messageWindow->addMessage(QString("2nd: %1").arg(message));
}

void TMController::binaryMessageReceived(const QByteArray &message)
{
    if(message == QByteArray("1"))
    {
        this->m_mainWindow->setStatusBarText("Typing...",3000);
    }
    else if(message == QByteArray("2"))
    {
        this->m_mainWindow->setStatusBarText("Connected.",0);
    }
}

void TMController::socketConnected()
{
    this->m_messageWindow->setConnectedState();
}

void TMController::socketDisconnected()
{
    this->m_messageWindow->setDisconnectedState();
    if(this->m_currentConnection)
    {
        this->m_currentConnection->deleteLater();
    }
    this->m_currentConnection = Q_NULLPTR;
}

void TMController::typingMessage()
{
    if(this->m_messageWindow->messageLength()>2)
    {
        if((QDateTime::currentMSecsSinceEpoch() - this->m_typingTimer)>=3000)
        {
            this->m_typingTimer = QDateTime::currentMSecsSinceEpoch();
            if(this->m_currentConnection)
            {
                this->m_currentConnection->sendBinaryMessage(QByteArray("1"));;
            }
        }
    }
    else
    {
        this->m_typingTimer = QDateTime::currentMSecsSinceEpoch();
    }
}

void TMController::pong(quint64 elapsedTime, const QByteArray &payload)
{
    Q_UNUSED(elapsedTime);
    Q_UNUSED(payload);
}

