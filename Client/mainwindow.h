#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>


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

    int Is_Active();
private slots:

    void on_clear_clicked();

    void on_connect_button_clicked();

private:
    Ui::MainWindow *ui;
    //socket klijenta
    QTcpSocket* mSocket;
    QString name;
    int is_active;
};
#endif // MAINWINDOW_H
