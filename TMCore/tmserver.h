#ifndef TMSERVER_H
#define TMSERVER_H

#include <QObject>
#include <QWebSocketServer>

class TMServer : public QObject
{
    Q_OBJECT
public:
    explicit TMServer(QObject *parent = nullptr);
    virtual ~TMServer();

signals:

public slots:
};

#endif // TMSERVER_H
