#include "mainwindow.h"
#include "emdilib.h"
#include "docworker.h"

#include <QApplication>
#include <QDebug>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QTextEdit>

#include <memory>
#include <string>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    Emdi emdi;

    std::string file1 = "somefile.txt";
    std::string file2 = "anotherfile.sch";

    auto uptw = std::unique_ptr<TxtWorker>();
    auto upsw = std::unique_ptr<SchWorker>();

    emdi.registerWorker(std::move(uptw));
    emdi.registerWorker(std::move(upsw));

    emdi.openDoc(file1);
    emdi.openDoc(file2);


    QMdiSubWindow *subw = new QMdiSubWindow();
    QMdiArea *mdi = new QMdiArea();
    QTextEdit *te = new QTextEdit();
    subw->setWidget(te);
    mdi->addSubWindow(subw);10
    w.setCentralWidget(mdi);

#if defined(QT_DEBUG)
    qDebug("Hi from main qt_debug");
#elif defined(QT_NO_DEBUG)
   qDebug("Hi from main qt_no_debug");
#endif




    w.show();
    return a.exec();
}
