#ifndef HSTORPROCESS_H
#define HSTORPROCESS_H

#include <QFile>
#include <QObject>
#include <QProcess>
#include <QVariantMap>
#include <QStandardPaths>

class HSTorProcess;
class HSTorHiddenService;

class HSTorProcess : public QObject
{
    Q_OBJECT

public:
    explicit HSTorProcess(QObject *parent = nullptr);
    virtual ~HSTorProcess();

    void setTorExe(const QString &torExe);
    void setTorPort(const quint16 &torPort);
    void setTorControlPort(const quint16 &torControlPort);
    void setTorControlPassword(const QString &torControlPassword);
    void setTorDataDirectory(const QString &torDataDirectory);

    QString getTorExe() const;
    quint16 getTorPort() const;
    quint16 getTorControlPort() const;
    QString getTorControlPassword() const;
    QString getTorDataDirectory() const;

    void addHiddenService(HSTorHiddenService *service);

    void setTorrcFilePath(const QString &path);
    QString getTorrcFilePath() const;

private:
    void generateTorrcFile(const QString &filePath) const;

signals:
    void torProcessStarted();
    void torProcessFinished();
    void torProcessFailedToStart();
    void torProcessError(const QString &errorText);
    void torProcessMessage(const QString &messageText);
    void torProcessBootstrapStatus(const int &status);

    void torHeartBeat(const QString &uptime, const QString &circuits, const QString &dataSent, const QString &dataReceived);

public slots:
    void start();
    void stop();
    bool isTorProcessReady() const;
    bool isTorProcessRunning() const;

private slots:
    void readyReadStandardOutput();
    void readyReadStandardError();

private:
    bool m_torProcessReady;

    QProcess m_proc;

    QString m_torExe;
    quint16 m_torPort;
    quint16 m_torControlPort;
    QString m_torControlPassword;
    QString m_torDataDirectory;
    QList<HSTorHiddenService*> m_hiddenServiceList;

    QString m_torrcFilePath;
};

#endif // HSTORPROCESS_H
