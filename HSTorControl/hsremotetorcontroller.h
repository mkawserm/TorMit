#ifndef HSREMOTETORCONTROLLER_H
#define HSREMOTETORCONTROLLER_H

#include <QObject>
#include <QTcpSocket>

class HSRemoteTorController : public QObject
{
    Q_OBJECT
public:
    explicit HSRemoteTorController(QObject *parent = nullptr);
    virtual ~HSRemoteTorController();

signals:
    void controllerConnected();
    void controllerDisconnected();

public slots:
    void connectToHost(const QString &ip, const quint16 &port = 9051);
    bool authenticate(const QString &password);

private:
    bool m_torControllerReady;
    QTcpSocket *m_torControlSocket;
};

#endif // HSREMOTETORCONTROLLER_H
