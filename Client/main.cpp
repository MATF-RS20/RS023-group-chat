#include "mainwindow.h"
#include "ChatBox.h"

#include <QApplication>

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);

   ChatBox cb;
   cb.show();
   MainWindow w;
   w.show();






    //svaki klijent poziva broadcastAll
    w.broadcastAll();

    return a.exec();
}
