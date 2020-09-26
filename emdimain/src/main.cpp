#include "mainwindow.h"
#include "emdilib.h"


#include <QApplication>
#include <QDebug>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QTextEdit>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QMdiSubWindow *subw = new QMdiSubWindow();
    QMdiArea *mdi = new QMdiArea();
    QTextEdit *te = new QTextEdit();

    Emdilib qe;

    subw->setWidget(te);
    mdi->addSubWindow(subw);
    w.setCentralWidget(mdi);

#if defined(QT_DEBUG)
    qDebug("Hi from main qt_debug");
#elif defined(QT_NO_DEBUG)
   qDebug("Hi from main qt_no_debug");
#endif




    w.show();
    return a.exec();
}
