#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//prvo je vidljiv loginPage
    ui->stackedWidget->setCurrentWidget(ui->loginPage);


//testiranje povezivanja na server
    mSocket = new QTcpSocket(this);
    mSocket->connectToHost("localhost",4567);


//    klijent dobija informaciju da je neki drugi klijent povezan na server
    connect(mSocket, &QTcpSocket::readyRead, [&]() {
        QTextStream T(mSocket);
        auto text = T.readAll();
        std::string textIspis = text.toUtf8().constData();
        std::cout << textIspis << std::endl;
    });


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

int MainWindow::Is_Active() {
    return is_active;
}

void MainWindow::on_clear_clicked()
{
    //brise sadrzaj iz hostname i port polja..
}

void MainWindow::on_connect_button_clicked()
{
    //konektovanje na server i prikaz ChatBoxa..
    ui->stackedWidget->setCurrentWidget(ui->chatPage);
}

void MainWindow::on_send_clicked(){
    //TODO
}
