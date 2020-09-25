#include "mainwindow.h"
//#include "../../qtemdilib/include/qtemdilib.h"
#include "qtemdilib.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    Qtemdilib qe;

#if defined(QT_DEBUG)
    qDebug("Hi from main qt_debug");
#elif defined(QT_NO_DEBUG)
   qDebug("Hi from main qt_no_debug");
#endif


    w.show();
    return a.exec();
}
