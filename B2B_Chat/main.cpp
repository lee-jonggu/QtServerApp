#include "clientchat.h"
#include "chatmain.h"
#include "mainwindow.h"

#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ClientChat c;
    c.show();

    return a.exec();
}
