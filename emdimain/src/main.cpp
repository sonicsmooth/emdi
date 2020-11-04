#include "mainwindow.h"
#include "emdilib.h"
#include "documents.h"

#include <QApplication>
#include <QDebug>
#include <QInputDialog>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <iomanip>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <sstream>




using docVec_t = std::list<std::unique_ptr<Document> >;

template <typename T> std::string docString() {return "undefined";}
template <> std::string docString<SchDocument>() {return "SchDocument_";}
template <> std::string docString<TxtDocument>() {return "TxtDocument_";}

template<typename T> std::string docName() {
    static int idx = 0;
    std::stringstream ss;
    ss << docString<T>() << std::setfill('0') <<std::setw(2) << idx++;
    return ss.str();
}    


template<typename T>
void newDoc(std::string userType, Emdi & emdi, docVec_t & docVec) {
    std::string docname = docName<T>();
    auto p = std::make_unique<T>(docname);
    emdi.addDocument(p.get());
    emdi.newMdiFrame(docname, userType);
    docVec.push_back(std::move(p));
}

QWidget *buttonWindow(Emdi &, docVec_t &);
QWidget *buttonWindow(Emdi & emdi, docVec_t & docVec) {
    QWidget *w = new QWidget();
    QVBoxLayout *vb = new QVBoxLayout();
    QPushButton *pb;

    pb = new QPushButton("New Schematic Doc");
    vb->addWidget(pb);
    QObject::connect(pb, &QPushButton::clicked, [&](){
        newDoc<SchDocument>("Main Editor", emdi, docVec);});

    pb = new QPushButton("New Text Doc");
    vb->addWidget(pb);
    QObject::connect(pb, &QPushButton::clicked, [&](){
        newDoc<TxtDocument>("Main Editor", emdi, docVec);});

    pb = new QPushButton("Close Current Doc");
    vb->addWidget(pb);
    QObject::connect(pb, &QPushButton::clicked, [&](){
        emdi.closeDocument();});

    pb = new QPushButton("Duplicate Current MDI");
    vb->addWidget(pb);
    QObject::connect(pb, &QPushButton::clicked, [&](){
        emdi.duplicateMdiFrame();});

    pb = new QPushButton("Properties Dock");
    vb->addWidget(pb);
    QObject::connect(pb, &QPushButton::clicked, [&](){
        emdi.showDockFrame("Properties");});

    pb = new QPushButton("Hierarchy Dock");
    vb->addWidget(pb);
    QObject::connect(pb, &QPushButton::clicked, [&](){
        emdi.showDockFrame("Hierarchy");});

    vb->addStretch();
    w->setLayout(vb);
    return w;
}


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.setCentralWidget(nullptr);

    Emdi emdi;
    emdi.addMainWindow(&w);
    docVec_t docVec;

    QObject::connect(&emdi, &Emdi::destroy, [&docVec](void *p) {
        docVec.remove_if([&](const std::unique_ptr<Document> & up) {
            return up.get() == static_cast<Document *>(p);});});
    QWidget *buttWindow = buttonWindow(emdi, docVec);


#if defined(QT_DEBUG)
    qDebug("Hi from main qt_debug");
#elif defined(QT_NO_DEBUG)
   qDebug("Hi from main qt_no_debug");
#endif

    buttWindow->show();
    w.show();
    return a.exec();
}
