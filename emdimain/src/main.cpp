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
    emdi.AddMainWindow(&w);

    std::string file1 = "somefile.txt";
    std::string file2 = "anotherfile.sch";

    TxtDocument doc1(file1);
    SchDocument doc2(file2);

    // TODO: Retrieve document userTypes
    emdi.AddDocument(&doc1);
    emdi.AddDocument(&doc2);

    emdi.ShowView(file1, "Schematic", AttachmentType::MDI);
    emdi.ShowView(file1, "Properties", AttachmentType::Dock);

    emdi.ShowView(file2, "SymView", AttachmentType::MDI);
    emdi.ShowView(file2, "Properties", AttachmentType::Dock);


#if defined(QT_DEBUG)
    qDebug("Hi from main qt_debug");
#elif defined(QT_NO_DEBUG)
   qDebug("Hi from main qt_no_debug");
#endif

    w.show();

    return a.exec();
}
