#include "SocketServer.h"
#include "SocketClient.h"
#include <QTextStream>
#include <QDebug>

SocketServer::SocketServer(QObject *parent)
    : QTcpServer(parent)
{
}

bool SocketServer::startServer(quint16 port)
{
    return listen(QHostAddress::Any, port);
}

void SocketServer::incomingConnection(qintptr socketfd)
{
    qDebug() << "Povezan klijent sa fd:" << socketfd;
    auto socket = new SocketClient(socketfd, this);
    mSockets << socket;

    for (const auto &i :  mSockets) {
        if(i != socket){
            QTextStream T(i);
            T << "[server]:Povezan je klijent: " << socket->socketDescriptor();
            i->flush();
        }
    }


    //kada god klijent napise nesto , server reaguje na signal i poziva fju broadcastAll
    connect(socket, &SocketClient::ReadyReadSig,this,&SocketServer::broadcastAll);
    connect(socket, &SocketClient::StateChangedSig,this,&SocketServer::clientDisconected);
}

void SocketServer::broadcastAll(SocketClient *client){
    qDebug() << "ReadyRead client:" << client;
    QTextStream S(client);
    auto text = S.readAll();

    for (const auto &i : mSockets) {
        if(i != client){
            QTextStream K(i);
            K << text;
            i->flush();
        }
    }
}

void SocketServer::clientDisconected(SocketClient *S, int ST){

    qDebug() << "Klijent je promenio stanje fd:"
             << S->sockedfd();
    if (ST == QTcpSocket::UnconnectedState) {
        qDebug() << "Klijent je diskonektovan:"
                 << S->sockedfd();
        mSockets.removeOne(S);
        for (const auto &i : mSockets) {
            QTextStream K(i);
            K << "Server: klijent -> "
              << S->sockedfd()
              << " se diskonektovao!";
            i->flush();
        }
    }
}

