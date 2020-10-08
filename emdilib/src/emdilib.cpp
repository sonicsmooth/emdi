#include "emdilib.h"

#include <algorithm>
#include <exception>
#include <iostream>
#include <iterator>

#include <QDebug>
#include <QDockWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QMdiArea>
#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql>
#include <QSqlQuery>
#include <QVariant>

template<> Document * qVal<Document *>(const QSqlQuery & query, int i) {
    return reinterpret_cast<Document *>(query.value(i).toULongLong());
}
template<> QMainWindow * qVal<QMainWindow *>(const QSqlQuery & query, int i) {
    return reinterpret_cast<QMainWindow *>(query.value(i).toULongLong());
}
template<> QWidget * qVal<QWidget *>(const QSqlQuery & query, int i) {
    return reinterpret_cast<QWidget *>(query.value(i).toULongLong());
}
template<> Document * qVal<Document *>(const QSqlQuery & query, const QString & field) {
    int i = query.record().indexOf(field);
    return reinterpret_cast<Document *>(query.value(i).toULongLong());
}
template<> QMainWindow * qVal<QMainWindow *>(const QSqlQuery & query, const QString & field) {
    int i = query.record().indexOf(field);
    return reinterpret_cast<QMainWindow *>(query.value(i).toULongLong());
}
template<> QWidget * qVal<QWidget *>(const QSqlQuery & query, const QString & field) {
    int i = query.record().indexOf(field);
    return reinterpret_cast<QWidget *>(query.value(i).toULongLong());
}
QString selectStr(const QString & table, const QString & field, unsigned int i) {
    return QString("SELECT * FROM %1 WHERE \"%2\" IS %3 LIMIT 1").
            arg(table).arg(field).arg(i);
}
QString selectStr(const QString & table, const QString & field, const std::string & str) {
    return QString("SELECT * FROM %1 WHERE \"%2\" IS '%3' LIMIT 1").
            arg(table).arg(field).arg(str.c_str());
}
QString selectStr(const QString & table, const QString & field, QMainWindow *ptr) {
    return QString("SELECT * FROM %1 WHERE \"%2\" IS %3 LIMIT 1").
            arg(table).arg(field).arg(uint64_t(ptr));
}
QString selectStr(const QString & table, const QString & field, const QMainWindow *ptr) {
    return QString("SELECT * FROM %1 WHERE \"%2\" IS %3 LIMIT 1").
            arg(table).arg(field).arg(uint64_t(ptr));
}
QString selectStr(const QString & table, const QString & field, QWidget *ptr) {
    return QString("SELECT * FROM %1 WHERE \"%2\" IS %3 LIMIT 1").
            arg(table).arg(field).arg(uint64_t(ptr));
}
QString selectStr(const QString & table, const QString & field, const QWidget *ptr) {
    return QString("SELECT * FROM %1 WHERE \"%2\" IS %3 LIMIT 1").
            arg(table).arg(field).arg(uint64_t(ptr));
}
QString selectStr(const QString & table, const QString & field, Document *ptr) {
    return QString("SELECT * FROM %1 WHERE \"%2\" IS %3 LIMIT 1").
            arg(table).arg(field).arg(uint64_t(ptr));
}
QString selectStr(const QString & table, const QString & field, const Document *ptr) {
    return QString("SELECT * FROM %1 WHERE \"%2\" IS %3 LIMIT 1").
            arg(table).arg(field).arg(uint64_t(ptr));
}
QString selectStr(const QString & table, const QString & field, AttachmentType at) {
    return QString("SELECT * FROM %1 WHERE \"%2\" IS '%3' LIMIT 1").
            arg(table).arg(field).arg(attach2str<QString>(at));
}

template<> QString tableName<DocRecord>() {return "docs";}
template<> QString tableName<DocWidgetsRecord>() {return "docWidgets";}
template<> QString tableName<FramesRecord>() {return "frames";}
template<> QString tableName<MainWindowsRecord>() {return "mainWindows";}

DocRecord::DocRecord(const QSqlQuery & query) :
    ID(qVal<decltype(ID)>(query, 0)),
    ptr(qVal<decltype(ptr)>(query, 1)),
    name(qVal<QString>(query, 2).toStdString()){}

DocWidgetsRecord::DocWidgetsRecord(const QSqlQuery & query) :
    ID(qVal<decltype(ID)>(query, 0)),
    ptr(qVal<decltype(ptr)>(query, 1)),
    docID(qVal<decltype(docID)>(query, 2)){}

FramesRecord::FramesRecord(const QSqlQuery & query) :
    ID(qVal<decltype(ID)>(query, 0)),
    ptr(qVal<decltype(ptr)>(query, 1)),
    userType(qVal<QString>(query, 2).toStdString()),
    attach(str2attach(qVal<QString>(query, 3))),
    mainWindowID(qVal<decltype(mainWindowID)>(query, 4)),
    docWidgetID(qVal<decltype(docWidgetID)>(query, 5)){}

MainWindowsRecord::MainWindowsRecord(const QSqlQuery & query) :
    ID(qVal<decltype(ID)>(query, 0)),
    ptr(qVal<decltype(ptr)>(query, 1)){}

void fatalStr(const QString & inftxt, int line) {
    qDebug(inftxt.toLatin1());
    QMessageBox mb;
    mb.setIcon(QMessageBox::Critical);
    mb.setWindowTitle("DB Error");
    if (line)
        mb.setText("Line #" + QString::number(line));
    mb.setInformativeText(inftxt);
    mb.exec();
    throw(std::logic_error(inftxt.toLatin1()));
}
QString querr(const QString & comment, const QSqlQuery & query) {
    // Concatenates comment, last error, and query text with \n
    return QString("%1\n%2\n%3").
                   arg(comment).
                   arg(query.lastError().text()).
                   arg(query.lastQuery());
}

Emdi::Emdi() {
#if defined(QT_DEBUG)
    qDebug("Hi from lib qt_debug");
#elif defined(QT_NO_DEBUG)
    qDebug("Hi from lib qt_no_debug");
#endif
    _dbInitDb();

}
Emdi::~Emdi() {
    {
        QSqlDatabase db = QSqlDatabase::database("connviews");
        db.close();
    }
    QSqlDatabase::removeDatabase("connviews");
}

void Emdi::_dbInitDb() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "connviews");
    db.setDatabaseName("TheFile.db");
    db.open();
    QSqlQuery query(db);
    QStringList qsl({"DROP TABLE IF EXISTS docs;                                                   ",
                     "DROP TABLE IF EXISTS docWidgets;                                             ",
                     "DROP TABLE IF EXISTS frames;                                                 ",
                     "DROP TABLE IF EXISTS mainWindows;                                            ",
                     "CREATE TABLE docs (ID   INTEGER PRIMARY KEY AUTOINCREMENT,                 \n"
                     "                   ptr  INTEGER,                                           \n"
                     "                   name TEXT);                                               ",
                                
                     "CREATE TABLE docWidgets  (ID    INTEGER PRIMARY KEY AUTOINCREMENT,          \n"
                     "                          ptr   INTEGER,                                    \n"
                     "                          docId REFERENCES docs(ID));                         ",
                 
                     "CREATE TABLE frames (ID           INTEGER PRIMARY KEY AUTOINCREMENT,        \n"
                     "                     ptr          INTEGER,                                  \n"
                     "                     userType     TEXT,                                     \n"
                     "                     attach       TEXT CHECK (attach IN ('MDI', 'Dock')),   \n"
                     "                     mainWindowID INTEGER REFERENCES mainWindows(ID),       \n"
                     "                     docWidgetID  UNIQUE REFERENCES docWidgets(ID));          ",
                 
                     "CREATE TABLE mainWindows (ID          INTEGER PRIMARY KEY AUTOINCREMENT,   \n"
                     "                          ptr         INTEGER);                            \n"});
    for (QString qs: qsl)
        if (!query.exec(qs))
            fatalStr(querr("Could not init", query), __LINE__);
}

void Emdi::_dbAddDocument(const Document *ptr) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString::asprintf("INSERT INTO docs (ptr,name) VALUES (%llu, '%s')", uint64_t(ptr), ptr->name().c_str());
    if (!query.exec(s))
        fatalStr(querr("Could not execute add Document", query), __LINE__);
}
void Emdi::_dbAddMainWindow(const QMainWindow *ptr) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString::asprintf("INSERT INTO mainWindows (ptr) VALUES (%llu);", uint64_t(ptr));
    if (!query.exec(s))
        fatalStr(querr("Could not execute add mainWindow", query), __LINE__);
}
MainWindowsRecord Emdi::_dbFindLatestMainWindow() const {
    QString s = "SELECT * FROM mainWindows ORDER BY ID DESC LIMIT 1;";
    MainWindowsRecord r = getRecord<MainWindowsRecord>(s);
    return r;
}
void Emdi::_dbAddDocWidget(const QWidget *ptr, unsigned int ID) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString::asprintf("INSERT INTO docWidgets (ptr,docID) VALUES (%llu,%u);", uint64_t(ptr), ID);
    if (!query.exec(s))
        fatalStr(querr("Could not execute add docWidget", query), __LINE__);
}
void Emdi::_dbAddFrame(const QWidget *ptr, const std::string & userType,
                       AttachmentType at, unsigned int mwID, unsigned int dwID) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString::asprintf("INSERT INTO frames (ptr,userType,attach,mainWindowID,docWidgetID) "
                                  "VALUES (%llu,'%s','%s',%u,%u);", uint64_t(ptr), userType.c_str(),
                                  at == AttachmentType::Dock ? "Dock" : "MDI", mwID, dwID);
    if (!query.exec(s))
        fatalStr(querr("Could not execute add Frame", query), __LINE__);
}

void Emdi::AddMainWindow(const QMainWindow *mw) {
    _dbAddMainWindow(mw);
}
void Emdi::AddDocument(const Document *doc) {
    _dbAddDocument(doc);
}
void Emdi::ShowView(const std::string & docName, const std::string & userType, AttachmentType at) {
    // docId is the unique string identifier for the document
    // frameType is specific to the document, eg SchView, SymView, etc.
    // AttachmentType is either MDI or Dock

    // Use most recent host window
    MainWindowsRecord lmwr = _dbFindLatestMainWindow();
    QMainWindow *mainWindow = lmwr.ptr;
    QMdiArea *mdi = static_cast<QMdiArea *>(mainWindow->centralWidget());
    if (!mdi) {
        mdi = new QMdiArea();
        mainWindow->setCentralWidget(mdi);
    }

    // Retrieve the Document and get view
    DocRecord dr = getRecord<DocRecord>("name", docName);
    Document *doc   = dr.ptr;
    QWidget *docWidget = doc->OpenView(userType);

    if (!docWidget) {
        qDebug("Cannot open document frameType");
        throw(std::logic_error("Cannot open document frameType"));
    }

    // Create new MDI view
    // Make this templated for MDI vs. DockWidget types
    // Pass in factory as template argument
    if (at == AttachmentType::MDI) {
        QMdiSubWindow *frame = new QMdiSubWindow();
        frame->setWidget(docWidget);
        frame->setWindowTitle(QString::fromStdString(userType));
        mdi->addSubWindow(frame);
        _dbAddDocWidget(docWidget, dr.ID);
        auto dwr = getRecord<DocWidgetsRecord>("ptr",docWidget);
        _dbAddFrame(frame, userType, at, lmwr.ID, dwr.ID);
    }

    // Create new, or reuse DockWidget
    else if (at == AttachmentType::Dock) {
        // TODO: add WHERE MainWindow...
        // ConnView cv = _findRecord("frameType", frameType);
        // if (cv.ID) {
        //     // Attach docWidget to existing record
        //     QDockWidget *subWidget = static_cast<QDockWidget *>(cv.subWidget);
        //     subWidget->setWidget(docWidget);
        //     // TODO: We have new doc info, so either:
        //     // 1. replace old doc info with new doc info, keeping frame info (forgetting old doc)
        //     // 2. replace old frame info with nulls, create new frame info with new doc info
        //     // (1) Cannot forget old doc info because it has docWidget
        //     // (2) Implies we can have a doc-only record, which means we have to redo how we add a document initially
        // }
        // else {
            QDockWidget *frame = new QDockWidget();
            frame->setWidget(docWidget);
            frame->setWindowTitle(QString::fromStdString(userType));
            mainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, frame);
  //          _addConnView({0, docName, const_cast<Document *>(doc), docWidget, frameType, "Dock", subWidget, mw});
//        }
    }
}
