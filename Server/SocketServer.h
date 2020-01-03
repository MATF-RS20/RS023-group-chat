#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QTcpServer>
#include <QMap>

class SocketClient;

class SocketServer : public QTcpServer
{
public:
    SocketServer(QObject *parent = nullptr);
    bool startServer(quint16 port);

private slots:
    void broadcastAll(SocketClient *client);
    void clientDisconected(SocketClient *S, int ST);
protected:
    void incomingConnection(qintptr handle);
private:
    QList<SocketClient *> mSockets;
};

#endif // SOCKETSERVER_H
