#ifndef HSTORCONTROLLER_H
#define HSTORCONTROLLER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QTcpSocket>
#include <QNetworkProxy>

class HSTorProcess;
class HSTorController;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief The HSTorController class
///
/// HSTorController is used to manage Tor process and
/// setup communication between Tor process and HSTorController
///////////////////////////////////////////////////////////////////////////////////////////////////
class HSTorController : public QObject
{
    Q_OBJECT
public:
    explicit HSTorController(QObject *parent = nullptr);
    virtual ~HSTorController();

signals:
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

    void torControllerReady();

public slots:
    void startTorProcess();
    void stopTorProcess();


    bool dump();
    bool halt();
    bool reload();
    bool newnym();
    bool heartbeat();

    QMap<QString,QString> addNewOnionService(const quint16 &virtualPort,
                                             const quint16 &targetPort);
    QMap<QString,QString> addNewOnionService(const quint16 &virtualPort,
                                             const QString &ip,
                                             const quint16 &targetPort);

    bool startOnionService(const QString &privateKey,
                           const quint16 &virtualPort,
                           const quint16 &targetPort);
    bool startOnionService(const QString &privateKey,
                           const quint16 &virtualPort,
                           const QString &ip,
                           const quint16 &targetPort);

    bool stopOnionService(const QString &serviceId);

    void setTorPort(const quint16 &port);
    void setTorExe(const QString &torExe);
    void setTorControlPort(const quint16 &controlport);
    void setTorDataDirecotry(const QString &dataDir);

    QNetworkProxy onionProxy() const;

private slots:
    void torProcessBootstrapStatusSlot(const int &status);

private:
    QString m_torExe;
    bool m_torControllerReady;
    HSTorProcess *m_torProcess;
    QTcpSocket *m_torControlSocket;

private:
    QString generateRandomPassword();
};

#endif // HSTORCONTROLLER_H
