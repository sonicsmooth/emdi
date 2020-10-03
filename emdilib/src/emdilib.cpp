#include "emdilib.h"
#include "vdocworker.h"

#include <iostream>

#include <QDebug>
#include <QDockWidget>
#include <QMainWindow>
#include <QMdiArea>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql>

Emdi::Emdi() {
#if defined(QT_DEBUG)
    qDebug("Hi from lib qt_debug");
#elif defined(QT_NO_DEBUG)
    qDebug("Hi from lib qt_no_debug");
#endif
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "emdiviews");
        //db.setDatabaseName("file:TheFile.db?mode=memory&cache=shared");
        db.setDatabaseName("TheFile.db");
        db.open();
        QSqlQuery query(db);
        query.exec("DROP TABLE IF EXISTS views");
        query.exec("CREATE TABLE views (ID                INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "                    DocID             TEXT,                             "
                   "                    ContentType       TEXT,                             "
                   "                    ParentWidget      INTEGER,                          "
                   "                    ParentWidgetType  TEXT,                             "
                   "                    HostWindow        INTEGER)                          ");


}
Emdi::~Emdi() {
    qDebug("Emdi::~Emdi");
    {
        QSqlDatabase db = QSqlDatabase::database("connname");
        db.close();
    }
    QSqlDatabase::removeDatabase("connname");
    qDebug("Removed database");
}

void Emdi::AddHostWindow(QMainWindow *mw) {
    m_hostWindows.push_back(mw);
}
void Emdi::AddDocument(std::unique_ptr<Document> doc) {
    (void) doc;
    qDebug("Emdi::AddDocument");

    QWidget *mv = doc->OpenView("MainView");
    QWidget *sv = doc->OpenView("SideView");
    m_docs.push_back(std::move(doc));

    // Use most recent host window for now
    QMainWindow *mw = m_hostWindows.back();
    QMdiArea *mdi = static_cast<QMdiArea *>(mw->centralWidget());
    if (!mdi) {
        mdi = new QMdiArea();
        mw->setCentralWidget(mdi);
    }
    QMdiSubWindow *mdisw = new QMdiSubWindow();
    mdisw->setWidget(mv);
    mdi->addSubWindow(mdisw);

    QDockWidget *dw = new QDockWidget();
    dw->setWidget(sv);
    mw->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dw);







}
