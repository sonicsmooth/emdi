#include "mainwindow.h"
#include "emdilib.h"
#include "documents.h"

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
    w.setCentralWidget(nullptr);


    Emdi emdi;
    emdi.AddHostWindow(&w);

    std::string file1 = "somefile.txt";
    std::string file2 = "anotherfile.sch";

    auto doc1 = std::make_unique<TxtDocument>(file1);
    auto doc2 = std::make_unique<SchDocument>(file2);

    emdi.AddDocument(std::move(doc1));
    emdi.ShowView(file1, "MainView");
    emdi.ShowView(file1, "SideView");

    emdi.AddDocument(std::move(doc2));
    emdi.ShowView(file2, "MainView");
    emdi.ShowView(file2, "SideView");


#if defined(QT_DEBUG)
    qDebug("Hi from main qt_debug");
#elif defined(QT_NO_DEBUG)
   qDebug("Hi from main qt_no_debug");
#endif

    w.show();

    return a.exec();
}
