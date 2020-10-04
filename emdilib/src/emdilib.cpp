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

Emdi::Emdi() {
#if defined(QT_DEBUG)
    qDebug("Hi from lib qt_debug");
#elif defined(QT_NO_DEBUG)
    qDebug("Hi from lib qt_no_debug");
#endif
    _initDb();

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

void Emdi::_initDb() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "emdiviews");
    //db.setDatabaseName("file:TheFile.db?mode=memory&cache=shared");
    db.setDatabaseName("TheFile.db");
    db.open();
    QSqlQuery query(db);
    query.exec("DROP TABLE IF EXISTS views");
    query.exec("CREATE TABLE views (ID                INTEGER PRIMARY KEY AUTOINCREMENT,"
               "                    DocID             TEXT,                             "
               "                    Document          INTEGER,                          "
               "                    ContentWidget     INTEGER,                          "
               "                    SubWidget         INTEGER,                          "
               "                    SubWidgetType     TEXT,                             "
               "                    MainWindow        INTEGER)                          ");
}

void Emdi::_addMainWindow(QMainWindow *mw) {
    QSqlDatabase db = QSqlDatabase::database("emdiviews");
    QSqlQuery query(db);
    query.prepare("INSERT INTO views (MainWindow) VALUES (:mw)");
    query.bindValue(":mw", reinterpret_cast<qulonglong>(mw));
    if (!query.exec()) {
        qDebug("Insert failed");
        qDebug(query.lastError().text().toLatin1());
        throw(std::logic_error("Insert failed"));
    }
}

QMainWindow *Emdi::_latestMainWindow() const {
    QSqlQuery query(QSqlDatabase::database("emdiviews"));
    query.exec("SELECT MainWindow FROM VIEWS ORDER BY ID DESC");
    if (query.next()) {
        return reinterpret_cast<QMainWindow *>(query.value(0).toULongLong());
    }
    else {
        qDebug("Can't find MainWindow");
        throw(std::logic_error("Can't find MainWindow"));
    }
}

const Document *Emdi::_findDocument(const std::string & docId) const {
    auto pred = [& docId](const std::unique_ptr<Document> & ptr) {
        const std::string each_docId = ptr->docId();
        return each_docId == docId;
        };
    auto docit = std::find_if(m_docs.begin(), m_docs.end(), pred);
    if (docit == m_docs.end())
        return nullptr;
    const Document *doc = docit->get();
    return doc;
}

void Emdi::AddMainWindow(QMainWindow *mw) {
    _addMainWindow(mw);
}
void Emdi::AddDocument(std::unique_ptr<Document> doc) {
    (void) doc;
    qDebug("Emdi::AddDocument");
    m_docs.push_back(std::move(doc));
}

void Emdi::ShowView(const std::string & docId, const std::string & viewType, WidgetType wt) {
    // docId is the unique string identifier for the document
    // viewType is specific to the document, eg SchView, SymView, etc.
    // WidgetType is either MDI or Dock

    // Use most recent host window
    QMainWindow *mw = _latestMainWindow();
    QMdiArea *mdi = static_cast<QMdiArea *>(mw->centralWidget());
    if (!mdi) {
        mdi = new QMdiArea();
        mw->setCentralWidget(mdi);
    }

    // Retrieve the Document based on its name
    const Document *doc = _findDocument(docId);

    // Get a new view from the doc
    QWidget *contentWidget = doc->OpenView(viewType);
    if (!contentWidget) {
        qDebug("Cannot open document viewtype");
        throw(std::logic_error("Cannot open document viewtype"));
    }

    // Create new view for now; later search if it already exists
    if (wt == WidgetType::MDI) {
        QMdiSubWindow *subWidget = new QMdiSubWindow();
        subWidget->setWidget(contentWidget);
        mdi->addSubWindow(subWidget);
    }
    else if (wt == WidgetType::Dock) {
        QDockWidget *subWidget = new QDockWidget();
        subWidget->setWidget(contentWidget);
        mw->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, subWidget);
    }
}
