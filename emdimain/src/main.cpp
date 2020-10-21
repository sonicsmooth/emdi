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


#include <memory>
#include <string>

using docVec_t = std::vector<std::unique_ptr<Document> >;

QWidget *buttonWindow(Emdi & emdi, docVec_t & docVec) {
    QWidget *w = new QWidget();


    QVBoxLayout *vb = new QVBoxLayout();
    QPushButton *pb = new QPushButton("New Schematic Doc");
    vb->addWidget(pb);
    QObject::connect(pb, &QPushButton::clicked, [&](){
        QString text = QInputDialog::getText(nullptr, "New Schematic Doc", "Filename");
        if (!text.isEmpty()) {
            auto p = std::make_unique<SchDocument>(text.toStdString());
            emdi.AddDocument(p.get());
            emdi.ShowView(text.toStdString(), "Main Editor", AttachmentType::MDI);
            docVec.push_back(std::move(p));
        }});

    pb = new QPushButton("New Text Doc");
    vb->addWidget(pb);
    QObject::connect(pb, &QPushButton::clicked, [&](){
        QString text = QInputDialog::getText(nullptr, "New Text Doc", "Filename");
        if (!text.isEmpty()) {
            auto p = std::make_unique<TxtDocument>(text.toStdString());
            emdi.AddDocument(p.get());
            emdi.ShowView(text.toStdString(), "Main Editor", AttachmentType::MDI);
            docVec.push_back(std::move(p));
        }});

    pb = new QPushButton("View Main Editor");
    vb->addWidget(pb);

    pb = new QPushButton("View Properties");
    vb->addWidget(pb);

    pb = new QPushButton("View Hierarchy");
    vb->addWidget(pb);

    vb->addStretch();
    w->setLayout(vb);
    return w;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.setCentralWidget(nullptr);



    Emdi emdi;
    emdi.AddMainWindow(&w);

    docVec_t docVec;
    QWidget *buttWindow = buttonWindow(emdi, docVec);

//    std::string file1 = "somefile.txt";
//    std::string file2 = "anotherfile.sch";

//    TxtDocument doc1(file1);
//    SchDocument doc2(file2);

//    // TODO: Retrieve document userTypes
//    emdi.AddDocument(&doc1);
//    emdi.AddDocument(&doc2);

//    emdi.ShowView(file1, "Properties", AttachmentType::Dock);
//    emdi.ShowView(file2, "Properties", AttachmentType::Dock);
//    emdi.ShowView(file1, "Explorer", AttachmentType::Dock);
//    emdi.ShowView(file2, "Explorer", AttachmentType::Dock);
//    emdi.ShowView(file1, "Schematic", AttachmentType::MDI);
//    emdi.ShowView(file2, "Schematic", AttachmentType::MDI);
//    emdi.ShowView(file1, "Symbol", AttachmentType::MDI);
//    emdi.ShowView(file2, "Symbol", AttachmentType::MDI);


#if defined(QT_DEBUG)
    qDebug("Hi from main qt_debug");
#elif defined(QT_NO_DEBUG)
   qDebug("Hi from main qt_no_debug");
#endif

    buttWindow->show();
    w.show();
    int x = a.exec();
    return x;
}
