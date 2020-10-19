#include "emdilib.h"

#include <QDebug>
#include <QDockWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql>
#include <QSqlQuery>
#include <QVariant>

#include <algorithm>
#include <exception>
#include <iostream>
#include <iterator>
#include <optional>
#include <vector>


template<> Document    * qVal<Document    *>(const QSqlQuery & query, int i) {
    return reinterpret_cast<Document *>(query.value(i).toULongLong());
}
template<> QMainWindow * qVal<QMainWindow *>(const QSqlQuery & query, int i) {
    QMainWindow *val = reinterpret_cast<QMainWindow *>(query.value(i).toULongLong());
    return val;
}
template<> QWidget     * qVal<QWidget     *>(const QSqlQuery & query, int i) {
    QWidget *val = reinterpret_cast<QWidget *>(query.value(i).toULongLong());
    return val;
}
template<> Document    * qVal<Document    *>(const QSqlQuery & query, const QString & field) {
    int i = query.record().indexOf(field);
    Document *val = reinterpret_cast<Document *>(query.value(i).toULongLong());
    return val;
}
template<> QMainWindow * qVal<QMainWindow *>(const QSqlQuery & query, const QString & field) {
    int i = query.record().indexOf(field);
    QMainWindow *val = reinterpret_cast<QMainWindow *>(query.value(i).toULongLong());
    return val;
}
template<> QWidget     * qVal<QWidget     *>(const QSqlQuery & query, const QString & field) {
    int i = query.record().indexOf(field);
    QWidget *val = reinterpret_cast<QWidget *>(query.value(i).toULongLong());
    return val;
}
QString limitstr(int limit) {
    if (limit >= 0)
        return QString(" LIMIT %1").arg(limit);
    return "";
}
QString selectStr(const QString & table, const QString & field, unsigned int i, int limit) {
    return QString("SELECT * FROM %1 WHERE \"%2\" IS %3%4;").
            arg(table).arg(field).arg(i).arg(limitstr(limit));
}
QString selectStr(const QString & table, const QString & field, const std::string & str, int limit) {
    return QString("SELECT * FROM %1 WHERE \"%2\" IS '%3'%4;").
            arg(table).arg(field).arg(str.c_str()).arg(limitstr(limit));
}
QString selectStr(const QString & table, const QString & field, const QMainWindow *ptr, int limit) {
    return QString("SELECT * FROM %1 WHERE \"%2\" IS %3%4;").
        arg(table).arg(field).arg(uint64_t(ptr)).arg(limitstr(limit));
}
QString selectStr(const QString & table, const QString & field, const QWidget *ptr, int limit) {
    return QString("SELECT * FROM %1 WHERE \"%2\" IS %3%4;").
        arg(table).arg(field).arg(uint64_t(ptr)).arg(limitstr(limit));
}
QString selectStr(const QString & table, const QString & field, const Document *ptr, int limit) {
    return QString("SELECT * FROM %1 WHERE \"%2\" IS %3%4;").
        arg(table).arg(field).arg(uint64_t(ptr)).arg(limitstr(limit));
}
QString selectStr(const QString & table, const QString & field, AttachmentType at, int limit) {
    return QString("SELECT * FROM %1 WHERE \"%2\" IS '%3'%4;").
        arg(table).arg(field).arg(attach2str<QString>(at)).arg(limitstr(limit));
}

template<> QString tableName<DocRecord>() {return "docs";}
template<> QString tableName<DocWidgetsRecord>() {return "docWidgets";}
template<> QString tableName<FramesRecord>() {return "frames";}
template<> QString tableName<MainWindowsRecord>() {return "mainWindows";}

DocRecord::DocRecord():
    ID(0),
    ptr(nullptr),
    name("") {}
DocRecord::DocRecord(const QSqlQuery & query) :
    ID(qVal<decltype(ID)>(query, "ID")),
    ptr(qVal<decltype(ptr)>(query, "ptr")),
    name(qVal<QString>(query, "name").toStdString()){}
DocRecord & DocRecord::operator=(const DocRecord & other) {
    this->ID = other.ID;
    this->ptr = other.ptr;
    this->name = other.name;
    return *this;
}

DocWidgetsRecord::DocWidgetsRecord() :
    ID(0),
    ptr(nullptr),
    userType(""),
    docID(0){}
DocWidgetsRecord::DocWidgetsRecord(const QSqlQuery & query) :
    ID(qVal<decltype(ID)>(query, "ID")),
    ptr(qVal<decltype(ptr)>(query, "ptr")),
    userType(qVal<QString>(query, "userType").toStdString()),
    docID(qVal<decltype(docID)>(query, "docID")){}
DocWidgetsRecord & DocWidgetsRecord::operator=(const DocWidgetsRecord & other) {
    this->ID = other.ID;
    this->ptr = other.ptr;
    this->userType = other.userType;
    this->docID = other.docID;
    return *this;
}

FramesRecord::FramesRecord() :
    ID(0),
    ptr(nullptr),
    attach(AttachmentType::ERROR),
    mainWindowID(0),
    docWidgetID(0){}
FramesRecord::FramesRecord(const QSqlQuery & query) :
    ID(qVal<decltype(ID)>(query, "ID")),
    ptr(qVal<decltype(ptr)>(query, "ptr")),
    attach(str2attach(qVal<QString>(query, "attach"))),
    mainWindowID(qVal<decltype(mainWindowID)>(query, "mainWindowID")),
    docWidgetID(qVal<decltype(docWidgetID)>(query, "docWidgetID")){}
FramesRecord & FramesRecord::operator=(const FramesRecord & other) {
    this->ID = other.ID;
    this->ptr = other.ptr;
    this->attach = other.attach;
    this->mainWindowID = other.mainWindowID;
    this->docWidgetID = other.docWidgetID;
    return *this;
}

MainWindowsRecord::MainWindowsRecord() :
    ID(0),
    ptr(nullptr){}
MainWindowsRecord::MainWindowsRecord(const QSqlQuery & query) :
    ID(qVal<decltype(ID)>(query, "ID")),
    ptr(qVal<decltype(ptr)>(query, "ptr")){}
MainWindowsRecord & MainWindowsRecord::operator=(const MainWindowsRecord & other) {
    this->ID = other.ID;
    this->ptr = other.ptr;
    return *this;
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
    const QString subq = "SELECT SUM(fail)>0 FROM                                                     \n"
                       "    (SELECT DISTINCT docWidgets.userType, mainWindowID,                       \n"
                       "                     COUNT(docWidgets.userType)>1 as fail                     \n"
                       "     FROM            frames                                                   \n"
                       "     LEFT JOIN       docWidgets                                               \n"
                       "     ON              frames.docWidgetID = docWidgets.ID                       \n"
                       "     WHERE           attach is 'Dock'                                         \n"
                       "     GROUP BY frames.mainWindowID, docWidgets.userType)                       \n";

    const
    QStringList qsl = {"DROP TABLE IF EXISTS docs;                                                      ",
                       "DROP TABLE IF EXISTS docWidgets;                                                ",
                       "DROP TABLE IF EXISTS frames;                                                    ",
                       "DROP TABLE IF EXISTS mainWindows;                                               ",
                       "CREATE TABLE docs (ID   INTEGER PRIMARY KEY AUTOINCREMENT,                    \n"
                       "                   ptr  INTEGER UNIQUE,                                       \n"
                       "                   name TEXT);                                                  ",

                       "CREATE TABLE docWidgets  (ID       INTEGER PRIMARY KEY AUTOINCREMENT,         \n"
                       "                          ptr      INTEGER UNIQUE,                            \n"
                       "                          userType TEXT,                                      \n"
                       "                          docID    REFERENCES docs(ID));                        ",

                       "CREATE TABLE frames (ID           INTEGER PRIMARY KEY AUTOINCREMENT,          \n"
                       "                     ptr          INTEGER UNIQUE,                             \n"
                       "                     attach       TEXT CHECK (attach IN ('MDI', 'Dock')),     \n"
                       "                     docWidgetID  INTEGER UNIQUE,                             \n"
                       "                     mainWindowID INTEGER,                                    \n"
                       "                     FOREIGN KEY(docWidgetID) REFERENCES docWidgets(ID),      \n"
                       "                     FOREIGN KEY(mainWindowID) REFERENCES mainWindows(ID));   \n",

                       "CREATE TABLE mainWindows (ID          INTEGER PRIMARY KEY AUTOINCREMENT,      \n"
                       "                          ptr         INTEGER UNIQUE);                        \n",

                       "CREATE TRIGGER multidocks_insert_fail                                         \n"
                       "    AFTER INSERT ON frames                                                    \n"
                       "WHEN                                                                          \n"
                       "    (" + subq + ")                                                            \n"
                       "BEGIN                                                                         \n"
                       "    SELECT RAISE(ABORT, 'Cannot allow more than one userType in mainWindow'); \n"
                       "END;                                                                            ",

                       "CREATE TRIGGER multidocks_update_fail                                         \n"
                       "    AFTER UPDATE ON frames                                                    \n"
                       "WHEN                                                                          \n"
                       "    (" + subq + ")                                                            \n"
                       "BEGIN                                                                         \n"
                       "    SELECT RAISE(ABORT, 'Cannot allow more than one userType in mainWindow'); \n"
                       "END;                                                                            "};
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
std::optional<MainWindowsRecord> Emdi::_dbFindLatestMainWindow() const {
    QString s = "SELECT * FROM mainWindows ORDER BY ID DESC LIMIT 1;";
    return getRecord<MainWindowsRecord>(s);
}
void Emdi::_dbAddDocWidget(const QWidget *ptr, const std::string & userType, unsigned int ID) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString::asprintf("INSERT INTO docWidgets (ptr,userType,docID) VALUES (%llu,'%s',%u);",
                                  uint64_t(ptr), userType.c_str(),ID);
    if (!query.exec(s))
        fatalStr(querr("Could not execute add docWidget", query), __LINE__);
}
std::optional<DocWidgetsRecord> Emdi::_dbFindDockWigetsRecordByUserTypeDocID(const std::string & userType, unsigned int docID) {
    QString s = QString("SELECT * from DocWidgets WHERE \"userType\" IS '%1' AND \n"
                        "                               \"docID\" is %2").
                        arg(userType.c_str()).arg(docID);
    return getRecord<DocWidgetsRecord>(s);
}
void Emdi::_dbAddFrame(const QWidget *ptr, AttachmentType at, unsigned int mwID, unsigned int dwID) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString::asprintf("INSERT INTO frames (ptr,attach,docWidgetID,mainWindowID) "
                                  "VALUES (%llu,'%s',%u,%u);", uint64_t(ptr),
                                  at == AttachmentType::Dock ? "Dock" : "MDI", dwID, mwID);
    if (!query.exec(s))
        fatalStr(querr("Could not execute add Frame", query), __LINE__);
}
std::optional<FramesRecord> Emdi::_dbFindExistingDockFrame(const std::string & userType, unsigned int mainWindowID){
    QString s = QString("SELECT frames.* FROM frames \n"
                        "JOIN   docWidgets\n"
                        "ON     frames.docWidgetID = docWidgets.ID\n"
                        "WHERE  docWidgets.userType = '%1' AND\n"
                        "       mainWindowID IS %2\n"
                        "LIMIT  1").arg(userType.c_str()).arg(mainWindowID);
    return getRecord<FramesRecord>(s);
}
void Emdi::_dbUpdateFrameDocWidgetID(unsigned int ID, unsigned int docWidgetID) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString("UPDATE frames               \n"
                        "SET    \"docWidgetID\" = %1 \n"
                        "WHERE  \"ID\" is %2;").arg(docWidgetID).arg(ID);
    if (!query.exec(s))
        fatalStr(querr("Could not update frames table", query), __LINE__);
}
void Emdi::_onMdiActivated(QMdiSubWindow *sw) {
    // Figure out the docWidgets, the frames, and the userTypes from sw
    if (!sw)
        return;
    QSqlQuery query(QSqlDatabase::database("connviews"));
    auto mwropt = getRecord<MainWindowsRecord>("ptr", sw->window());
    assert(mwropt);
    unsigned int mwid = mwropt->ID;
    QString s = QString(
        "SELECT docWidgets.ptr as \"docWidgetPtr\",            \n"
        "       eut.frptr as \"framePtr\", docWidgets.userType \n"
        "FROM   docWidgets                                     \n"
        "JOIN   (SELECT userType, frames.ptr as \"frptr\"      \n"
        "        FROM   docWidgets                             \n"
        "        JOIN   frames                                 \n"
        "        ON     docWidgets.ID = frames.docWidgetID     \n"
        "        WHERE  frames.attach = 'Dock' AND             \n"
        "               frames.mainWindowID = %1) as eut       \n"
        "ON     eut.userType = docWidgets.userType             \n"
        "JOIN  (SELECT docID, frames.ptr as \"framePtr\"       \n"
        "       FROM   docWidgets                              \n"
        "       JOIN   frames                                  \n"
        "       ON     docWidgets.ID = frames.docWidgetID      \n"
        "       WHERE  frames.ptr = %2) AS selectedDoc         \n"
        "ON     docWidgets.docID = selectedDoc.docID;          \n").
        arg(mwid).arg(uint64_t(sw));
   if (!query.exec(s))
       fatalStr(querr("Could not run _onMdiActivated", query), __LINE__);
   while (query.next()) {
       auto userType = qVal<QString>(query, "userType");
       auto *docWidget = qVal<QWidget *>(query, "docWidgetPtr");
       auto *wframe = qVal<QWidget *>(query, "framePtr");
       auto *frame = dynamic_cast<QDockWidget *>(wframe);
       if (!frame)
           throw(std::logic_error("Could not retrieve pointer"));
       frame->setWidget(docWidget);
       frame->setWindowTitle(userType);
    }
}
void Emdi::_onMdiClosed(QObject *sw) {
    // dynamic_cast and qobject_cast don't work
    QMdiSubWindow *mdiSubWindow = static_cast<QMdiSubWindow *>(sw);
    FramesRecord fr = *getRecord<FramesRecord>("ptr", mdiSubWindow);
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString docIDsToDelete = "SELECT ID from docs                       \n"
                             "EXCEPT                                    \n"
                             "SELECT DISTINCT docID                     \n"
                             "FROM   docWidgets                         \n"
                             "JOIN   frames                             \n"
                             "ON     docWidgets.ID = frames.docWidgetID \n"
                             "WHERE  frames.attach != 'Dock'";
    QStringList qsl = {QString("BEGIN TRANSACTION;"),
                       QString("SAVEPOINT DEL1"),
                       QString("DELETE FROM frames WHERE ID == %1;").arg(fr.ID),
                       QString("DELETE FROM docWidgets WHERE ID == %1;").arg(fr.docWidgetID),
                       QString("RELEASE DEL1")};

    // Do the first part of the transaction
    for (QString qs: qsl) {
        if (!query.exec(qs)) {
            fatalStr(querr("Could not delete frame and docWidget", query), __LINE__); }}

    // Read the docs to delete, close the doc
    QString condemnedDocsStr = QString("SELECT * FROM docs WHERE ID = (%1);").arg(docIDsToDelete);
    auto condemnedDocs = getRecords<DocRecord>(condemnedDocsStr);
    assert(condemnedDocs.size() <= 1);
    if (condemnedDocs.size())
        condemnedDocs[0].ptr->done();

    // Read the docWidgets to delete, delete them
    // Should all be associated with QDockWidgets
    // If smart, then roll back everything if this delete fails
    QString dwToDelete = QString("SELECT * FROM docWidgets WHERE docID = (%1)").arg(docIDsToDelete);
    for (auto dw : getRecords<DocWidgetsRecord>(dwToDelete))
        delete dw.ptr;

    // Finish the transaction
    qsl = QStringList({QString("DELETE FROM docWidgets WHERE docID = (%1);").arg(docIDsToDelete),
                       QString("DELETE FROM docs WHERE ID = (%1);").arg(docIDsToDelete),
                       QString("COMMIT;")});
    for (QString qs: qsl) {
       if (!query.exec(qs)) {
           fatalStr(querr("Could not delete frame and docWidget", query), __LINE__);}}
}
void Emdi::AddMainWindow(QMainWindow *mainWindow) {
    // Make sure mainwindow has MDI area
    _dbAddMainWindow(mainWindow);
    QMdiArea *mdi = dynamic_cast<QMdiArea *>(mainWindow->centralWidget());
    if (mdi)
        return;
    mdi = new QMdiArea();
    mainWindow->setCentralWidget(mdi);
    QObject::connect(mdi, &QMdiArea::subWindowActivated, this, &Emdi::_onMdiActivated);
}
void Emdi::AddDocument(const Document *doc) {
    _dbAddDocument(doc);
}
void Emdi::ShowView(const std::string & docName, const std::string & userType,
                    AttachmentType at, QMainWindow *mainWindow) {
    // docId is the unique string identifier for the document
    // frameType is specific to the document, eg SchView, SymView, etc.
    // AttachmentType is either MDI or Dock
    // mw is the mainWindow to put the view in, otherwise nullptr for latest main window

    // Retrieve the Document and create or retrieve view
    auto dropt = getRecord<DocRecord>("name", docName);
    Document *doc = dropt->ptr;
    QWidget *docWidget = nullptr;
    DocWidgetsRecord dwr;
    auto dwropt = _dbFindDockWigetsRecordByUserTypeDocID(userType, dropt->ID);
    if (dwropt) {
        docWidget = dwropt->ptr;
        dwr = *dwropt;
    }
    else {
        doc->init(); // generic version of "open"
        docWidget = doc->newView(userType);// assert docWidget
        _dbAddDocWidget(docWidget, userType, dropt->ID);
        dwr = *getRecord<DocWidgetsRecord>("ptr",docWidget);
    }

    // Find mainWindow record
    MainWindowsRecord mwr;
    if (mainWindow) {
        mwr = *getRecord<MainWindowsRecord>("ptr", mainWindow);
    } else {
        mwr = *_dbFindLatestMainWindow();
        mainWindow = mwr.ptr;
    }

    // Create new or reuse MDI view
    if (at == AttachmentType::MDI) {
        QMdiArea *mdi = static_cast<QMdiArea *>(mainWindow->centralWidget());
        QMdiSubWindow *frame = new QMdiSubWindow();
        frame->setAttribute(Qt::WA_DeleteOnClose);
        QObject::connect(frame, &QObject::destroyed, this, &Emdi::_onMdiClosed);
        frame->setWidget(docWidget);
        frame->setWindowTitle(QString::fromStdString(userType));
        mdi->addSubWindow(frame);
        _dbAddFrame(frame, at, mwr.ID, dwr.ID);
    }

    // Create new or reuse DockWidget
    else if (at == AttachmentType::Dock) {
        QDockWidget *frame = nullptr;
        // Find frame with this userType and Dock and mainWindowID
        auto fropt = _dbFindExistingDockFrame(userType, mwr.ID); // See about removing userType from frames
        if (fropt) { // reuse
            frame = static_cast<QDockWidget *>(fropt->ptr);
            _dbUpdateFrameDocWidgetID(fropt->ID, dwr.ID);
        } else { // new
            frame = new QDockWidget();
            mainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, frame);
            _dbAddFrame(frame, at, mwr.ID, dwr.ID);
        }
        frame->setWidget(docWidget);
        frame->setWindowTitle(QString::fromStdString(userType));

    }
}
