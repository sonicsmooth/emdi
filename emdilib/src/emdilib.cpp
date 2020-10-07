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


template<typename T>
T *_ptr(const QVariant & qv) {
    return reinterpret_cast<T *>(qv.toULongLong());
}
unsigned int _uint(const QVariant & qv) {
    return qv.toUInt();
}
std::string _str(const QVariant & qv) {
    return qv.toString().toStdString();
}
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
bool fatalExec (QSqlQuery & query, int line) {
    if (!query.exec()) {
        QString qs = query.lastError().text();
        qDebug(qs.toLatin1());
        QMessageBox mb;
        mb.setIcon(QMessageBox::Critical);
        mb.setWindowTitle("DB Error");
        if (line)
            mb.setText("Line #" + QString::number(line));
        mb.exec();
        throw(std::logic_error(qs.toLatin1()));
    }
    return true;
}
bool fatalExec (QSqlQuery & query, const QString & inftxt, int line) {
    if (!query.exec()) {
        QString qs = query.lastError().text();
        qDebug(qs.toLatin1());
        QMessageBox mb;
        mb.setIcon(QMessageBox::Critical);
        mb.setWindowTitle("DB Error");
        mb.setInformativeText(inftxt);
        if (line)
            mb.setText("Line #" + QString::number(line));
        mb.exec();
        throw(std::logic_error(qs.toLatin1()));
    }
    return true;
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
            fatalStr(qs, __LINE__);
}

void Emdi::_dbAddDocument(const Document *doc) {
    QSqlDatabase db = QSqlDatabase::database("connviews");
    QSqlQuery query(db);
    QString s;
    s.sprintf("INSERT INTO docs (ptr,name) VALUES (%u, '%s')", uint64_t(doc), doc->name().c_str());
    if (!query.exec(s))
        fatalStr(query.lastError().text() + "\n\"" + s + "\"", __LINE__);
}
const Document *Emdi::_dbFindDocPtr(const std::string & arg) const {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    query.prepare("SELECT ptr FROM docs WHERE name IS " + QString::fromStdString(arg));
    fatalExec(query, query.lastQuery(), __LINE__);
    if(query.next())
        return qVal<Document *>(query);
        //return _ptr<Document>(query.value(0));
    else
        fatalStr("Can't find document", __LINE__);
}
const Document *Emdi::_dbFindDocPtr(unsigned int arg) const {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s;
    s.sprintf("SELECT ptr FROM docs WHERE ID IS %d", arg);
    query.prepare(s);
    fatalExec(query, query.lastQuery() + "\n\"" + s + "\"", __LINE__);
    if(query.next())
        return qVal<Document *>(query);
        //return _ptr<Document>(query.value(0));
    else
        fatalStr("Can't find document", __LINE__);
}
unsigned int Emdi::_dbFindDocID(const std::string & arg) const {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s;
    s.sprintf("SELECT ID FROM docs WHERE name IS '%s'", arg.c_str());
    query.prepare(s);
    fatalExec(query, s, __LINE__);
    if(query.next())
        return qVal<unsigned int>(query);
        //return _uint(query.value(0));
    else
        fatalStr("Can't find document", __LINE__);
}
unsigned int Emdi::_dbFindDocID(const Document *arg) const {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s;
    s.sprintf("SELECT ID FROM docs WHERE ptr IS %u;", uint64_t(arg));
    query.prepare(s);
    fatalExec(query, s, __LINE__);
    if(query.next())
        return qVal<unsigned int>(query);
        //return query.value(0).toInt();
    else
        fatalStr("Can't find document", __LINE__);
}

void Emdi::_dbAddMainWindow(const QMainWindow *arg) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s;
    s.sprintf("INSERT INTO mainWindows (ptr) VALUES (%u);", uint64_t(arg));
    if (!query.exec(s))
        fatalStr(s, __LINE__);
}
void Emdi::_dbAddDocWidget(const QWidget *arg1, int arg2) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s;
    s.sprintf("INSERT INTO docWidgets (ptr,docID) VALUES (%u,%d);",
        uint64_t(arg1), arg2);
    if (!query.exec(s))
        fatalStr(s, __LINE__);
}
void Emdi::_dbAddFrame(const QWidget *frame, const std::string & userType, 
                       AttachmentType at, int mainWindowID, int docWidgetID) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s;
    QString atstr = at == AttachmentType::Dock ? "Dock" : "MDI";
    s.sprintf("INSERT INTO frames (ptr,userType,attach,mainWindowID,docWidgetID) "
              "VALUES (%u,%s,%s,%d,%d);", uint64_t(frame), userType.c_str(), atstr.toLatin1().data(),
              mainWindowID, docWidgetID);
    if (!query.exec(s))
        fatalStr(s, __LINE__);
}
QMainWindow *Emdi::_dbFindMainWindow() const {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = "SELECT ptr FROM mainWindows ORDER BY ID DESC;";
    if (!query.exec(s))
        fatalStr(s, __LINE__);
    if (query.next())
        return qVal<QMainWindow *>(query);
        //return _ptr<QMainWindow>(query.value(0));
    else
        fatalStr("Can't find MainWindow");
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
    QMainWindow *mw = _dbFindMainWindow();
    QMdiArea *mdi = static_cast<QMdiArea *>(mw->centralWidget());
    if (!mdi) {
        mdi = new QMdiArea();
        mw->setCentralWidget(mdi);
    }

    // Retrieve the Document based on its name
    const Document *doc   = _dbFindDocPtr(docName);
    unsigned int            docID  = _dbFindDocID(docName);
    const Document *doc2  = _dbFindDocPtr(docID);
    unsigned int            docID2 = _dbFindDocID(doc);

    // Get a new view from the doc
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
        _dbAddDocWidget(docWidget, _dbFindDocID(doc));
        _dbAddFrame(frame, userType, at, 0, 0);
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
            mw->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, frame);
  //          _addConnView({0, docName, const_cast<Document *>(doc), docWidget, frameType, "Dock", subWidget, mw});
//        }
    }
}
