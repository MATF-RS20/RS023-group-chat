#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
    ui->nickname->clear();
    ui->port->setValue(0);
}

void MainWindow::fromServer(){
    QTextStream T(mSocket);
    auto text = T.readAll();
    ui->textBox->append(text);
}

void MainWindow::on_connect_button_clicked()
{
    //konektovanje na server i prikaz ChatBoxa..
    QString host = ui->hostname->text();
    qint16 port = ui->port->value();
    mSocket = new QTcpSocket(this);
    mSocket->connectToHost(host,port);

    connect(mSocket, &QTcpSocket::readyRead,this,&MainWindow::fromServer);
    broadcastAll();
    ui->stackedWidget->setCurrentWidget(ui->chatPage);
}

void MainWindow::on_send_clicked(){
    QString msg = ui->message->text();
    if(!msg.isEmpty()){
        QTextStream T(mSocket);
        T << msg;
        mSocket->flush();
        ui->textBox->append(msg);
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
}

void MainWindow::on_buttonBox_rejected()
{
    //Korisnik je odustao od pravljenja naloga
    //Vracamo se na glavni prozor aplikacije
    ui->newPassword->clear();
    ui->newUsername->clear();
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
}
