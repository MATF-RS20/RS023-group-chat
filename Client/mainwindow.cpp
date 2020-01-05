#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//---pozadina aplikacije
   // QPixmap background("/home/andjela/Desktop/RS023-group-chat/images/b3.jpg");
   // background = background.scaled(this->size());
   // QPalette palette;
   // palette.setBrush(QPalette::Background, background);
   // this->setPalette(palette);
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

    if(text.startsWith("[logAccepted]")){
        QList<QString> tmpList = text.split(":");
        mNicknameLog = tmpList[1];
        ui->stackedWidget->setCurrentWidget(ui->chatPage);
        broadcastAll();
    }else if(text.startsWith("[logDeclinedUsrPas]")){
        qDebug() << "USER PASS INCORECT!";
        if(ui->username->text() == "" || ui->password->text() == ""){
            ui->error_msg_line_2->setText("Molimo popunite sva polja..");
        }
        else{
        ui->error_msg_line_2->setText("Nepostojeci nalog...");
        }
        mSocket->disconnectFromHost();
    }else if(text.startsWith("[logDeclinedInUse]")){
        ui->error_msg_line_2->setText("Korisnik je vec aktivan...");
    }
    else if(text.startsWith("[PasswdIncorrect]:")){
         ui->error_msg_line_2->setText("Pogresna sifra naloga..");
    }
    else{
        ui->textBox->append(text);
    }
}

void MainWindow::on_connect_button_clicked()
{
    //konektovanje na server i prikaz ChatBoxa..
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
{
    //Potvrda novog naloga (Treba napraviti novi nalog korisnika)
    mNickname = ui->nickname_line->text();
    mUsername = ui->username_line->text();
    mPassword = ui->password_line->text();
    if(mNickname.isEmpty() or mUsername.isEmpty() or mPassword.isEmpty()){
        qDebug() << "Nickname,user,pass: " <<  mNickname << mUsername << mPassword;
        on_buttonBox_rejected();
        ui->stackedWidget->setCurrentWidget(ui->SignUpPage);
        ui->error_msg_line->setText("Morate uneti sva tri polja...");
    }else{
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
