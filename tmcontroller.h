#ifndef TMCONTROLLER_H
#define TMCONTROLLER_H

#include <QDir>
#include <QFile>
#include <QTimer>
#include <QObject>
#include <QWebSocket>
#include <QNetworkProxy>
#include <QStandardPaths>
#include <QWebSocketServer>
#include <QDateTime>

#include <HSTorControl>

#include "tmmainwindow.h"
#include "tmlogwindow.h"
#include "tmmessagewindow.h"
#include "tmcentralwidget.h"

class TMController : public QObject
{
    Q_OBJECT
public:
    explicit TMController(QObject *parent = nullptr);
    virtual ~TMController();

    void setMainWindow(TMMainWindow *mainWindow);

signals:

public slots:
    void start();

private slots:
    void torProcessStarted();
    void torProcessFinished();
    void torProcessFailedToStart();

    void torProcessBootstrapStatus(const int &status);

    void torProcessError(const QString &errorText);
    void torProcessMessage(const QString &messageText);
    void torHeartBeat(const QString &uptime,
                      const QString &circuits,
                      const QString &dataSent,
                      const QString &dataReceived);


    void error(const QString &errorText);

    void onionServiceId(const QString &serviceId);

    void sendButtonClicked();
    void connectButtonClicked();

    void torControllerReady();

    void newConnection();

    void textMessageReceived(const QString &message);
    void binaryMessageReceived(const QByteArray &message);

    void socketConnected();
    void socketDisconnected();

    void typingMessage();
    void pong(quint64 elapsedTime, const QByteArray &payload);

private:
    QString m_serviceFilePath;
    qint64 m_typingTimer;

    TMMainWindow *m_mainWindow;
    TMLogWindow *m_logWindow;
    TMMessageWindow *m_messageWindow;
    TMCentralWidget *m_centralWidget;
    HSTorController m_torController;

    QWebSocketServer *m_server;
    QWebSocket *m_currentConnection;
};

#endif // TMCONTROLLER_H
