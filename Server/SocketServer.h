#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QTcpServer>
#include <QMap>
#include <QFile>

struct clientData{
    QTcpSocket *clientSocket;
    QString clintNickname;
    QString clientUsername;
    QString clientPassword;
};

class SocketClient;

class SocketServer : public QTcpServer
{
public:
    SocketServer(QObject *parent = nullptr);
    bool startServer(quint16 port);
    void readClientsData();

private slots:
    void broadcastAll(SocketClient *client);
    void clientDisconected(SocketClient *S, int ST);
protected:
    void incomingConnection(qintptr handle);
private:
    QList<SocketClient *> mSockets;
    QList<SocketClient *> mAccSockets;
    QList<SocketClient *> mAccSocketsDeclined;
    QList<clientData> mClientsData;
};

#endif // SOCKETSERVER_H
