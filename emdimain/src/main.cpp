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

    auto doc1 = std::make_unique<TxtDocument>(file1);
    auto doc2 = std::make_unique<SchDocument>(file2);

    // TODO: Retrieve document frameTypes 
    emdi.AddDocument(std::move(doc1));
    emdi.ShowView(file1, "Schematic", WidgetType::MDI);
    emdi.ShowView(file1, "Properties", WidgetType::Dock);

    emdi.AddDocument(std::move(doc2));
    emdi.ShowView(file2, "SymView", WidgetType::MDI);
    emdi.ShowView(file2, "Properties", WidgetType::Dock);


#if defined(QT_DEBUG)
    qDebug("Hi from main qt_debug");
#elif defined(QT_NO_DEBUG)
   qDebug("Hi from main qt_no_debug");
#endif

    w.show();

    return a.exec();
}
