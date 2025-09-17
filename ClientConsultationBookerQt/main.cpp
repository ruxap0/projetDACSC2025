#include "mainwindowclientconsultationbooker.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowClientConsultationBooker w;
    w.show();
    return a.exec();
}
