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

void SocketServer::readClientsData(){
    QFile clientsData("clients.txt");
    clientsData.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&clientsData);
    QString line;
    while(true){
        line = in.readLine();
        if(line.isEmpty()){
            break;
        }
        QList<QString> splitedLine = line.split(":");
        QString nickname = splitedLine[0];
        QString username = splitedLine[1];
        QString password = splitedLine[2];

        clientData tmpData = {
            nullptr,
            nickname,
            username,
            password
        };

        mClientsData.append(tmpData);
    }

    for(auto &i : mClientsData){
        qDebug() << i.clientSocket << i.clintNickname << i.clientUsername << i.clientPassword;
    }
}

void SocketServer::incomingConnection(qintptr socketfd)
{
    qDebug() << "Povezan klijent sa fd:" << socketfd;
    auto socket = new SocketClient(socketfd, this);
    mSockets << socket;

    connect(socket, &SocketClient::ReadyReadSig,this,&SocketServer::broadcastAll);
    connect(socket, &SocketClient::StateChangedSig,this,&SocketServer::clientDisconected);
}

void SocketServer::broadcastAll(SocketClient *client){
    qDebug() << "ReadyRead client:" << client;
    QTextStream S(client);
    auto text = S.readAll();

    if(text.startsWith("[accCheck]")){
        auto socketTmp = client;
        mSockets.removeOne(client);
        mAccSockets << socketTmp;
        qDebug() << text;
        QList<QString> splited = text.split(":");
        QString clientNickname = splited[1];
        QString clientUsername = splited[2];
        QString clientPassword = splited[3];

        qDebug() << clientNickname << clientUsername << clientPassword;

        for(auto &i : mClientsData){
            if(clientNickname.compare(i.clintNickname) == 0 or clientPassword.compare(i.clientPassword) == 0
                     or clientUsername.compare(i.clientUsername) == 0){
                S << "INCORECT";
                client->flush();
                return;
            }
        }

        QFile clientDataWrite("clients.txt");
        clientDataWrite.open(QFile::ReadWrite | QFile::Text | QFile::Append);
        QTextStream writeStream(&clientDataWrite);
        writeStream << clientNickname;
        writeStream << ":";
        writeStream << clientUsername;
        writeStream << ":";
        writeStream << clientPassword << "\n";
        writeStream.flush();
        clientDataWrite.close();

        clientData tmpData = {
            nullptr,
            clientNickname,
            clientUsername,
            clientPassword
        };

        mClientsData.append(tmpData);
        S << "OK";
        client->flush();
    }else if(text.startsWith("[logCheck]")){
        qDebug() << "LOG CHECK!";
        QList<QString> splited = text.split(":");
        QString user = splited[1];
        QString pass = splited[2];

        for(auto &i : mClientsData){
            if(i.clientUsername.compare(user) == 0 and i.clientPassword.compare(pass) == 0){
                if(i.clientSocket != nullptr){
                    qDebug() << "Nije se poklopio user jer je vec ulogovan na servis!";
                    mSockets.removeOne(client);
                    mAccSocketsDeclined << client;
                    S << "[logDeclinedInUse]";
                    client->flush();
                    return;
                }
                qDebug() << "Poklopio se USER!";
                i.clientSocket = client;
                S << "[logAccepted]:" << i.clintNickname;
                client->flush();
                return;
            }
        }

        qDebug() << "Nije se poklopio user zbog ne postojeceg usra i pass!";
        mSockets.removeOne(client);
        mAccSocketsDeclined << client;
        S << "[logDeclinedUsrPas]";
        client->flush();
        return;
    }
    else{
        QString tmpNickname;
        for(const auto &i : mClientsData){
            if(i.clientSocket == client){
                tmpNickname = i.clintNickname;
            }
        }

        for (const auto &i : mClientsData) {
            if(i.clientSocket != client and i.clientSocket != nullptr){
                QTextStream K(i.clientSocket);
                K << "[" << tmpNickname << "]:" << text;
                i.clientSocket->flush();
            }
        }
    }
}


void SocketServer::clientDisconected(SocketClient *S, int ST){

    qDebug() << "Klijent je promenio stanje fd:"
             << S->sockedfd();
    if (ST == QTcpSocket::UnconnectedState) {
        qDebug() << "Klijent je diskonektovan:"
                 << S->sockedfd();
        if(mSockets.removeOne(S)){
            qDebug() << "Obrisan je clientSocket!";
            clientData pom;
            for(auto &i : mClientsData){
                if(i.clientSocket == S){
                    i.clientSocket = nullptr;
                    pom = i;
                    break;
                }
            }
            for (const auto &i : mSockets) {
                QTextStream K(i);
                K << "[server]:klijent <"
                  << pom.clintNickname
                  << "> se diskonektovao!";
                i->flush();
            }
        }else{
            if(mAccSockets.removeOne(S)){
                qDebug() << "Obrisan je accMake socket!";
            }
            if(mAccSocketsDeclined.removeOne(S)){
                qDebug() << "Obrisan je accMakeDeclined socket!";
            }
        }
    }
}

