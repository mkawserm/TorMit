#include "hstorprocess.h"
#include "hstorhiddenservice.h"

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

HSTorProcess::HSTorProcess(QObject *parent) : QObject(parent)
{
    this->m_torProcessReady = false;
    this->m_torPort = 9050;
    this->m_torControlPort = 9051;


    this->connect(&this->m_proc,
                  &QProcess::started,
                  this,
                  &HSTorProcess::torProcessStarted);

    this->connect(&this->m_proc,
                  static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                  this,
                  [this](int exitCode, QProcess::ExitStatus exitStatus){
        Q_UNUSED(exitStatus);
        Q_UNUSED(exitCode);
        emit torProcessFinished();
    });

    this->connect(&this->m_proc,
                  &QProcess::readyReadStandardError,
                  this,
                  &HSTorProcess::readyReadStandardError);

    this->connect(&this->m_proc,
                  &QProcess::readyReadStandardOutput,
                  this,
                  &HSTorProcess::readyReadStandardOutput);
}

void HSTorProcess::setTorExe(const QString &torExe)
{
#if defined(Q_OS_WIN32)
    this->m_torExe = "\"" + torExe + "\"";
#else
    this->m_torExe = torExe;
#endif
}

void HSTorProcess::setTorPort(const quint16 &torPort)
{
    this->m_torPort = torPort;
}

void HSTorProcess::setTorControlPort(const quint16 &torControlPort)
{
    this->m_torControlPort = torControlPort;
}

void HSTorProcess::setTorControlPassword(const QString &torControlPassword)
{
    this->m_torControlPassword = torControlPassword;
}

void HSTorProcess::setTorDataDirectory(const QString &torDataDirectory)
{
    this->m_torDataDirectory = torDataDirectory;
}

QString HSTorProcess::getTorExe() const
{
    return this->m_torExe;
}

quint16 HSTorProcess::getTorPort() const
{
    return this->m_torPort;
}

quint16 HSTorProcess::getTorControlPort() const
{
    return this->m_torControlPort;
}

QString HSTorProcess::getTorControlPassword() const
{
    return this->m_torControlPassword;
}

QString HSTorProcess::getTorDataDirectory() const
{
    return this->m_torDataDirectory;
}

void HSTorProcess::addHiddenService(HSTorHiddenService *service)
{
    this->m_hiddenServiceList<<service;
}

void HSTorProcess::setTorrcFilePath(const QString &path)
{
    this->m_torrcFilePath = path;
}

QString HSTorProcess::getTorrcFilePath() const
{
    return this->m_torrcFilePath;
}

void HSTorProcess::generateTorrcFile(const QString &filePath) const
{
    QFile f(filePath);
    if(f.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        f.write("SOCKSPort ");
        f.write(QString::number(this->m_torPort).toLocal8Bit());
        f.write("\n");

        if(!this->m_torControlPassword.isEmpty())
        {
            //qDebug()<<"Pass:" << this->m_torControlPassword;

            QProcess p;
            p.setProgram(this->m_torExe);
            p.setArguments(QStringList() << "--hash-password"<<this->m_torControlPassword);
            p.start(QIODevice::ReadOnly);
            p.waitForReadyRead();
            QByteArray hp = p.readAllStandardOutput();
            p.close();
            p.waitForFinished();

            f.write("ControlPort ");
            f.write(QString::number(this->m_torControlPort).toLocal8Bit());
            f.write("\n");

            f.write("HashedControlPassword ");
            f.write(hp);
        }

        if(!this->m_torDataDirectory.isEmpty()){
            f.write("DataDirectory ");
            f.write(this->m_torDataDirectory.toLocal8Bit());
            f.write("\n");
        }

        for(const HSTorHiddenService *s:this->m_hiddenServiceList)
        {
            f.write(s->toString().toLocal8Bit());
        }

        f.close();
    }
}

void HSTorProcess::start()
{
    if(this->m_torDataDirectory.isEmpty())
    {
        QString tl = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+QLatin1String("/HSTorControl");
        {
            QDir d(tl);
            if(!d.exists())
            {
                d.mkpath(tl);
            }
        }
        this->m_torDataDirectory = tl;
    }

    if(this->m_torrcFilePath.isEmpty())
    {
        QString tl = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+QLatin1String("/HSTorControl");
        {
            QDir d(tl);
            if(!d.exists())
            {
                d.mkpath(tl);
            }
        }
        this->m_torrcFilePath = tl + QLatin1String("/torrc");
    }

    this->generateTorrcFile(this->m_torrcFilePath);

    if(QFile::exists(this->m_torrcFilePath))
    {
        this->m_proc.setProgram(this->m_torExe);
        this->m_proc.setArguments(QStringList()<<"-f"<<this->m_torrcFilePath);
        this->m_proc.start(QIODevice::ReadWrite);
    }
    else
    {
        emit torProcessFailedToStart();
    }
}

void HSTorProcess::stop()
{
    qDebug() << "HSTorProcess::stop() begin";

    if(this->m_proc.state() == QProcess::Running)
    {

#if defined(Q_OS_WIN32)
        this->m_proc.close();
        this->m_proc.kill();
        this->m_torProcessReady = false;
        emit torProcessBootstrapStatus(0);
        emit torHeartBeat(QLatin1String("0:00"),QLatin1String("0"),QLatin1String("0 kB"),QLatin1String("0 kB"));
#else
        //this->m_proc.close();
        this->m_proc.terminate();
        this->m_proc.waitForFinished(5000);
        this->m_torProcessReady = false;
        emit torProcessBootstrapStatus(0);
        emit torHeartBeat(QLatin1String("0:00"),QLatin1String("0"),QLatin1String("0 kB"),QLatin1String("0 kB"));
#endif
    }

    qDebug() << "HSTorProcess::stop() end";
}

bool HSTorProcess::isTorProcessReady() const
{
    return this->m_torProcessReady;
}

bool HSTorProcess::isTorProcessRunning() const
{
    return this->m_proc.state() == QProcess::Running;
}

void HSTorProcess::readyReadStandardOutput()
{
    QString message = this->m_proc.readAllStandardOutput();

    QRegularExpression re("Bootstrapped (\\d+)%");
    QRegularExpressionMatch match = re.match(message);
    if (match.hasMatch())
    {
        if(match.captured(1) == QLatin1String("100"))
        {
            this->m_torProcessReady = true;
        }
        emit torProcessBootstrapStatus(match.captured(1).toInt());
    }

    {
        QRegularExpression hb("is (.*?) hours.*?with(.*?)circuits.*?sent(.*?)and received(.*?B)");
        QRegularExpressionMatch match = hb.match(message);
        if(match.hasMatch()){
            QString uptime = match.captured(1);
            QString circuits = match.captured(2);
            QString dataSent = match.captured(3);
            QString dataReceived = match.captured(4);

            emit torHeartBeat(uptime.trimmed(),circuits.trimmed(),dataSent.trimmed(),dataReceived.trimmed());
        }
    }


    //qDebug() << "OUTPUT:"<<message;
    emit torProcessMessage(message);
}

void HSTorProcess::readyReadStandardError()
{
    QString error = this->m_proc.readAllStandardError();

    //qDebug() << "ERROR:"<<error;
    emit torProcessError(error);
}

HSTorProcess::~HSTorProcess()
{
    qDebug() << "HSTorProcess::~HSTorProcess() begin";

    while(!this->m_hiddenServiceList.isEmpty())
    {
        HSTorHiddenService *s = this->m_hiddenServiceList.takeLast();
        if(s) delete s;
    }
    this->stop();

    qDebug() << "HSTorProcess::~HSTorProcess() end";
}
