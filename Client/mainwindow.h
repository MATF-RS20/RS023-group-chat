#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "ui_mainwindow.h"
#include <QTextStream>
#include <QDebug>
#include <iostream>
#include <string>
#include <QStackedWidget>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void broadcastAll();
    void testSignal();
private slots:
    void on_clear_clicked();
    void on_send_clicked();
    void on_connect_button_clicked();
    void fromServer();
    void connectSuccesful();
    void on_signUp_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::MainWindow *ui;
    //server socket
    QTcpSocket* mSocket;
    //user,pass,nickname klijenta
    QString mUsername;
    QString mPassword;
    QString mNickname;
};
#endif // MAINWINDOW_H
