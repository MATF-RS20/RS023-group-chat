#include "mainwindow.h"
#include <regex>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//---pozadina aplikacije
    QPixmap background("../images/b3.jpg");
    background = background.scaled(this->size());
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);
//---

//prvo je vidljiv loginPage
    ui->stackedWidget->setCurrentWidget(ui->loginPage);

}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::broadcastAll(){
    QTextStream T(mSocket);
    T << "cao svima!!!";
    mSocket->flush();
}


void MainWindow::on_clear_clicked()
{
    //brise sadrzaj svih polja..
    ui->hostname->clear();
    ui->username->clear();
    ui->password->clear();
    ui->error_msg_line_2->clear();
    ui->port->setValue(0);
}

void MainWindow::fromServer(){
    QTextStream T(mSocket);
    auto text = T.readAll();

    if(text.startsWith("[clientDisconected]")){
            QList<QString> tmpList = text.split(":");
            QString nickLogOut = tmpList[1];

            QString disconectedMsg = "";
            disconectedMsg.append("User:[");
            disconectedMsg.append(nickLogOut);
            disconectedMsg.append("] se diskonektovao...");
            ui->textBox->append(disconectedMsg);

            if(mOnlineUsers.removeOne(nickLogOut)){
                ui->OnlineUsersBox->clear();
                for(const auto &i : mOnlineUsers){
                    ui->OnlineUsersBox->append(i);
                }
            }else{
                qDebug() << "BUG!";
            }
    }
    else if(text.startsWith("[logAccepted]")){
        QList<QString> tmpList = text.split(":");
        mNicknameLog = tmpList[1];
        qDebug() << tmpList.size();
        for(auto i=2;i<tmpList.size();i++){
            if(!tmpList[i].isEmpty()){
                mOnlineUsers.append(tmpList[i]);
            }
        }

        qDebug() << mOnlineUsers << " ***logAccepted**";
        ui->stackedWidget->setCurrentWidget(ui->chatPage);
        for(const auto &i : mOnlineUsers){
            ui->OnlineUsersBox->append(i);
        }
        broadcastAll();
    }
    else if(text.startsWith("[NewClientOnline]")){
         QList<QString> tmpList = text.split(":");
         QString newUserOnline = tmpList[1];
         mOnlineUsers.append(newUserOnline);
         qDebug() << mOnlineUsers << " ***newClientsOnline**";
         ui->OnlineUsersBox->clear();
         for(const auto &i : mOnlineUsers){
             ui->OnlineUsersBox->append(i);
         }

    }
    else if(text.startsWith("[logDeclinedUsrPas]")){
        qDebug() << "USER PASS INCORECT!";
        ui->error_msg_line_2->setText("Nepostojeci nalog...");
        mSocket->disconnectFromHost();
    }else if(text.startsWith("[logDeclinedInUse]")){
        mSocket->disconnectFromHost();
        ui->error_msg_line_2->setText("Korisnik je vec aktivan...");
    }
    else if(text.startsWith("[PasswdIncorrect]:")){
         mSocket->disconnectFromHost();
         ui->error_msg_line_2->setText("Pogresna sifra naloga..");
    }
    else{
        ui->textBox->append(text);
    }
}

void MainWindow::on_connect_button_clicked()
{
    //konektovanje na server i prikaz ChatBoxa..
    if(ui->username->text() == "" || ui->password->text() == "" || ui->hostname->text() == ""
             || ui->port->text() == ""){
        ui->error_msg_line_2->setText("Molimo popunite sva polja..");
        return;
    }

    QString host = ui->hostname->text();
    qint16 port = ui->port->value();
    mSocket = new QTcpSocket(this);
    mSocket->connectToHost(host,port);

    mUsernameLog = ui->username->text();
    mPasswordLog = ui->password->text();

    connect(mSocket,&QTcpSocket::connected,this,&MainWindow::connectSuccesful);
    connect(mSocket, &QTcpSocket::readyRead,this,&MainWindow::fromServer);

}

void MainWindow::connectSuccesful(){
    QTextStream T(mSocket);
    T << "[logCheck]:" << mUsernameLog << ":" << mPasswordLog;
    mSocket->flush();
}

void MainWindow::on_send_clicked(){
    QString msg = ui->message->text();
    if(!msg.isEmpty()){
        QTextStream T(mSocket);
        T << msg;
        mSocket->flush();
        QString tmp = "[";
        tmp.append(mNicknameLog);
        tmp.append("]:");
        tmp.append(msg);
        ui->textBox->append(tmp);
    }
    ui->message->clear();
    ui->message->setFocus();
}

void MainWindow::on_signUp_clicked()
{
    //Otvara se prozor za prijavljivanje novih clanova :)
    on_clear_clicked();
    ui->stackedWidget->setCurrentWidget(ui->SignUpPage);
}

void MainWindow::on_buttonBox_accepted()
{   //Potvrda novog naloga (Treba napraviti novi nalog korisnika)

    //regex za sifru: min 8 karaktera, bar 1 veliko i jedno malo slovo i bar jedan broj, bez specijalnih karaktera:
    //^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)[a-zA-Z\d]{8,}$

    //regex za username: izmedju 5 i 12 karaktera, mogu velika, mala slova i brojevi
    //[a-zA-Z\d]{5,12}

   // QRegExp userNameRegex("[a-zA-Z0-9]{5,12}");
    //QRegExp passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*0-9)[a-zA-Z0-9]{8,}$");


    //KOM
    mNickname = ui->nickname_line->text();
    mUsername = ui->username_line->text();
    mPassword = ui->password_line->text();

//    QRegularExpression userNameRegex("[a-zA-Z0-9]{5,12}");
//    QRegularExpression passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*0-9)[a-zA-Z0-9]{8,}$");

//    QRegularExpressionMatch matchNickName = userNameRegex.match(mNickname);
//    QRegularExpressionMatch matchUserName = userNameRegex.match(mUsername);
//    QRegularExpressionMatch matchPassword = passwordRegex.match(mPassword);
    //KOM

    if(mNickname.isEmpty() or mUsername.isEmpty() or mPassword.isEmpty()){
        qDebug() << "Nickname,user,pass: " <<  mNickname << mUsername << mPassword;
      //  on_buttonBox_rejected();
       // ui->stackedWidget->setCurrentWidget(ui->SignUpPage);
        ui->error_msg_line->setText("Morate uneti sva tri polja...");
    }

    //KOM
//    else if(!matchNickName.hasMatch() or !matchUserName.hasMatch()){
//        qDebug() << "Nickname,username:" <<  mNickname << mUsername;
//        on_buttonBox_rejected();
//        ui->stackedWidget->setCurrentWidget(ui->SignUpPage);
//        ui->error_msg_line->setText("Nickname i username moraju imati izmedju 5 i 12 karaktera!");
//    }
//    else if(!matchPassword.hasMatch()){
//        qDebug() << "Password:" <<  mPassword;
//        on_buttonBox_rejected();
//        ui->stackedWidget->setCurrentWidget(ui->SignUpPage);
//        ui->error_msg_line->setText("Sifra mora imati min. 8 karaktera,veliko slovo i cifru!");
//    }
    //KOM
    else{
        qDebug() << "Nickname,user,pass: " <<  mNickname << mUsername << mPassword;
        //TEST
        mSocketTmp = new QTcpSocket(this);
        mSocketTmp->connectToHost("localhost",4567);
        connect(mSocketTmp, &QTcpSocket::readyRead,this,&MainWindow::fromServerAccCheck);
        QTextStream T(mSocketTmp);
        T << "[accCheck]:" << mNickname << ":" << mUsername << ":" << mPassword;
        mSocketTmp->flush();
        //TEST
        ui->stackedWidget->setCurrentWidget(ui->loginPage);
    }
}

void MainWindow::fromServerAccCheck(){
    QTextStream T(mSocketTmp);
    auto text = T.readAll();
    if(text.startsWith("OK")){
        qDebug() << "OK!!!";
    }else{
        on_buttonBox_rejected();
        ui->stackedWidget->setCurrentWidget(ui->SignUpPage);
        ui->error_msg_line->setText("Acc vec postoji...");
    }
    mSocketTmp->disconnectFromHost();
}

void MainWindow::on_buttonBox_rejected()
{
    //Korisnik je odustao od pravljenja naloga
    //Vracamo se na glavni prozor aplikacije
    ui->password_line->clear();
    ui->username_line->clear();
    ui->nickname_line->clear();
    ui->error_msg_line->clear();
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
}
void MainWindow::on_buttonBox_2_accepted()
{
    //BRISE SE NALOG
    QString username = ui->deleteUser_line->text();
    QString password = ui->deletePasswd_line->text();

    mSocketTmp = new QTcpSocket(this);
    mSocketTmp->connectToHost("localhost",4567);
    connect(mSocketTmp, &QTcpSocket::readyRead,this,&MainWindow::fromServerDeleteAcc);

    QTextStream T(mSocketTmp);
    T << "[DeleteAcc]:" << username << ":" << password;
    mSocketTmp->flush();
     ui->stackedWidget->setCurrentWidget(ui->loginPage);
}

void MainWindow::fromServerDeleteAcc(){
   //Ovde cekam odgovor servera
    QTextStream T(mSocketTmp);
    auto text = T.readAll();
    if(text.startsWith("DELETED")){
        qDebug() << "DELETED!!!";
         ui->stackedWidget->setCurrentWidget(ui->loginPage);
    }
    else{
        qDebug() << "FAILED!";
        ui->error_msg_line_3->setText("Neuspesno brisanje naloga..");
    }
}

void MainWindow::on_buttonBox_2_rejected()
{
    //Korisnik odustao od brisanja naloga
    //Vracamo se na glavni prozor
    ui->deleteUser_line->clear();
    ui->deletePasswd_line->clear();
    ui->error_msg_line_3->clear();
    ui->stackedWidget->setCurrentWidget(ui->ModifyAccountPage);
}

void MainWindow::on_ChangeNickname_button_clicked()
{
    //Treba promeniti nickname korisnika
}

void MainWindow::on_ChangePasswd_button_clicked()
{
    //Treba promeniti sifru korisnika
}

void MainWindow::on_DeteleAccount_button_clicked()
{
    //Otvara se prozor za brisanje naloga
    ui->stackedWidget->setCurrentWidget(ui->DeleteAccountPage);
}

void MainWindow::on_ModifyAcc_button_clicked()
{   //Otvara se prozor za izmenu naloga
     ui->stackedWidget->setCurrentWidget(ui->ModifyAccountPage);
}
void MainWindow::on_Back_button_clicked()
{
    //Vracamo korisnika na loginPage
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
}
