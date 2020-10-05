#include "emdilib.h"

#include <algorithm>
#include <exception>
#include <iostream>
#include <iterator>

#include <QDebug>
#include <QDockWidget>
#include <QMainWindow>
#include <QMdiArea>
#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql>
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





Emdi::Emdi() {
#if defined(QT_DEBUG)
    qDebug("Hi from lib qt_debug");
#elif defined(QT_NO_DEBUG)
    qDebug("Hi from lib qt_no_debug");
#endif
    _initDb();

}
Emdi::~Emdi() {
    //qDebug("Emdi::~Emdi");
    {
        QSqlDatabase db = QSqlDatabase::database("connviews");
        db.close();
    }
    QSqlDatabase::removeDatabase("connviews");
    //qDebug("Removed database");
}

const Document *Emdi::_findDocument(const std::string & docName) const {
    auto pred = [& docName](const std::unique_ptr<Document> & ptr) {
        const std::string each_docName = ptr->docName();
        return each_docName == docName;
        };
    auto docit = std::find_if(m_docs.begin(), m_docs.end(), pred);
    if (docit == m_docs.end())
        return nullptr;
    const Document *doc = docit->get();
    return doc;
}

void Emdi::_initDb() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "connviews");
    db.setDatabaseName("TheOtherFile.db");
    db.open();
    QSqlQuery query(db);
    query.exec("DROP TABLE IF EXISTS views;                                                  ");
    query.exec("DROP TABLE IF EXISTS docs;                                                   ");
    query.exec("DROP TABLE IF EXISTS docWidgets;                                             ");
    query.exec("DROP TABLE IF EXISTS frames;                                                 ");
    query.exec("DROP TABLE IF EXISTS mainWindows;                                            ");
    query.exec("CREATE TABLE docs        (ID          INTEGER PRIMARY KEY AUTOINCREMENT,       "
               "                          name        TEXT,                                    "
               "                          ptr         INTEGER);                                ");
               
    query.exec("CREATE TABLE docWidgets  (ID          INTEGER PRIMARY KEY AUTOINCREMENT,       "
               "                          ptr         INTEGER,                                 "
               "                          docId       REFERENCES docs(ID));                    ");

    query.exec("CREATE TABLE frames      (ID          INTEGER PRIMARY KEY AUTOINCREMENT,       "
               "                          userType    TEXT,                                    "
               "                          ptr         INTEGER,                                 "
               "                          attach      TEXT CHECK (attach IN ('MDI', 'Dock'))); "
               "                          parent      INTEGER REFERENCES mainWindows(ID),      "
               "                          docWidgetID UNIQUE REFERENCES docWidgets(ID),        ");

    query.exec("CREATE TABLE mainWindows (ID          INTEGER PRIMARY KEY AUTOINCREMENT,       "
               "                          ptr         INTEGER,                                 "
               "                          frameID     INTEGER REFERENCES frames(ID));          ");
}

void Emdi::_addMainWindow(const QMainWindow *mw) {
    QSqlDatabase db = QSqlDatabase::database("connviews");
    QSqlQuery query(db);
    query.prepare("INSERT INTO mainWindows (ptr) VALUES (:mw);");
    query.bindValue(":mw", reinterpret_cast<uint64_t>(mw));
    if (!query.exec()) {
        qDebug("Insert MainWindow failed");
        qDebug(query.lastError().text().toLatin1());
        throw(std::logic_error("Insert MainWindow failed"));
    }
}

void Emdi::_addConnView(const ConnView & cv) {
    QSqlDatabase db = QSqlDatabase::database("connviews");
    QSqlQuery query(db);
    query.prepare("INSERT INTO views" 
        "(docName, docPtr, docWidgetPtr, frameType, framePtr, frameAttach, mainWindowPtr)"
        "VALUES (:dn, :dp, :dwp, :swn, :swp, :swt, :mwp);");
    query.bindValue(":dn", cv.docName.c_str());
    query.bindValue(":dp", reinterpret_cast<uint64_t>(cv.document));
    query.bindValue(":dwp", reinterpret_cast<uint64_t>(cv.docWidget));
    query.bindValue(":swn", cv.frameType.c_str());
    query.bindValue(":swp", reinterpret_cast<uint64_t>(cv.subWidget));
    query.bindValue(":swt", cv.frameAttach.c_str());
    query.bindValue(":mwp", reinterpret_cast<uint64_t>(cv.mainWindow));
    if (!query.exec()) {
        qDebug("Insert ConnView failed");
        qDebug(query.lastError().text().toLatin1());
        throw(std::logic_error("Insert ConnView failed"));
    }
}

QMainWindow *Emdi::_latestMainWindow() const {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    query.exec("SELECT ptr FROM mainWindows ORDER BY ID DESC");
    if (query.next()) {
        return reinterpret_cast<QMainWindow *>(query.value(0).toULongLong());
    }
    else {
        qDebug("Can't find MainWindow");
        throw(std::logic_error("Can't find MainWindow"));
    }
}

ConnView Emdi::_findRecord(const std::string & field, const std::string & value) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    query.prepare("SELECT * FROM views WHERE :field LIKE :value LIMIT 1");
    query.bindValue(":field", QVariant(field.c_str()));
    query.bindValue(":value", QVariant(value.c_str()));
    query.exec();
    if (query.next()) {
        ConnView cv({_uint(query.value(0)),            // ID
                     _str(query.value(1)),             // docName
                     _ptr<Document>(query.value(2)),   // document
                     _ptr<QWidget>(query.value(3)),    // docWidget
                     _str(query.value(4)),             // frameType
                     _str(query.value(5)),             // frameAttach
                     _ptr<QWidget>(query.value(6)),    // subWidget
                     _ptr<QMainWindow>(query.value(7)) // mainWindow
                    });
        return cv;
    }
    else {
        ConnView cv{0};
        return cv;
    }

}

void Emdi::AddMainWindow(const QMainWindow *mw) {
    _addMainWindow(mw);
}
void Emdi::AddDocument(std::unique_ptr<Document> doc) {
    (void) doc;
    qDebug("Emdi::AddDocument");
    m_docs.push_back(std::move(doc));
}

void Emdi::ShowView(const std::string & docName, const std::string & frameType, WidgetType wt) {
    // docId is the unique string identifier for the document
    // frameType is specific to the document, eg SchView, SymView, etc.
    // WidgetType is either MDI or Dock

    // Use most recent host window
    QMainWindow *mw = _latestMainWindow();
    QMdiArea *mdi = static_cast<QMdiArea *>(mw->centralWidget());
    if (!mdi) {
        mdi = new QMdiArea();
        mw->setCentralWidget(mdi);
    }

    // Retrieve the Document based on its name
    const Document *doc = _findDocument(docName);

    // Get a new view from the doc
    QWidget *docWidget = doc->OpenView(frameType);

    if (!docWidget) {
        qDebug("Cannot open document frameType");
        throw(std::logic_error("Cannot open document frameType"));
    }

    // Create new MDI view
    // Make this templated for MDI vs. DockWidget types
    // Pass in factory as template argument
    if (wt == WidgetType::MDI) {
        QMdiSubWindow *subWidget = new QMdiSubWindow();
        subWidget->setWidget(docWidget);
        subWidget->setWindowTitle(QString::fromStdString(frameType));
        mdi->addSubWindow(subWidget);
        _addConnView({0, docName, const_cast<Document *>(doc), docWidget, frameType, "MDI", subWidget, mw});
    }

    // Create new, or reuse DockWidget
    else if (wt == WidgetType::Dock) {
        // TODO: add WHERE MainWindow...
        ConnView cv = _findRecord("frameType", frameType);
        if (cv.ID) {
            // Attach docWidget to existing record
            QDockWidget *subWidget = static_cast<QDockWidget *>(cv.subWidget);
            subWidget->setWidget(docWidget);
            // TODO: We have new doc info, so either:
            // 1. replace old doc info with new doc info, keeping frame info (forgetting old doc)
            // 2. replace old frame info with nulls, create new frame info with new doc info
            // (1) Cannot forget old doc info because it has docWidget
            // (2) Implies we can have a doc-only record, which means we have to redo how we add a document initially
        }
        else {
            QDockWidget *subWidget = new QDockWidget();
            subWidget->setWidget(docWidget);
            subWidget->setWindowTitle(QString::fromStdString(frameType));
            mw->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, subWidget);
            _addConnView({0, docName, const_cast<Document *>(doc), docWidget, frameType, "Dock", subWidget, mw});
        }
    }
}
