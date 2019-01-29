#ifndef TMCLIENT_H
#define TMCLIENT_H

#include <QObject>
#include <QWebSocket>

class TMClient : public QObject
{
    Q_OBJECT
public:
    explicit TMClient(QObject *parent = nullptr);
    virtual ~TMClient();

signals:

public slots:
};

#endif // TMCLIENT_H
