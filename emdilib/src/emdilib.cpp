#include "emdilib.h"

#include <QApplication>
#include <QDebug>
#include <QDockWidget>
#include <QEvent>
#include <QFocusEvent>
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

void executeList(QSqlQuery & query, const QStringList & qsl, const QString & errstr, int linenum) {
    for (QString qs: qsl)
        if (!query.exec(qs))
            fatalStr(querr(errstr, query), linenum);
}

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

DocWidgetRecord::DocWidgetRecord() :
    ID(0),
    ptr(nullptr),
    frameID(0),
    docID(0){}
DocWidgetRecord::DocWidgetRecord(const QSqlQuery & query) :
    ID(qVal<decltype(ID)>(query, "ID")),
    ptr(qVal<decltype(ptr)>(query, "ptr")),
    frameID(qVal<decltype(frameID)>(query, "ptr")),
    docID(qVal<decltype(docID)>(query, "docID")){}
DocWidgetRecord & DocWidgetRecord::operator=(const DocWidgetRecord & other) {
    this->ID = other.ID;
    this->ptr = other.ptr;
    this->frameID = other.frameID;
    this->docID = other.docID;
    return *this;
}

FrameRecord::FrameRecord() :
    ID(0),
    ptr(nullptr),
    attach(AttachmentType::ERROR),
    userType(""),
    mainWindowID(0),
    docWidgetID(0){}
FrameRecord::FrameRecord(const QSqlQuery & query) :
    ID(qVal<decltype(ID)>(query, "ID")),
    ptr(qVal<decltype(ptr)>(query, "ptr")),
    attach(str2attach(qVal<QString>(query, "attach"))),
    userType(qVal<QString>(query, "userType").toStdString()),
    mainWindowID(qVal<decltype(mainWindowID)>(query, "mainWindowID")),
    docWidgetID(qVal<decltype(docWidgetID)>(query, "docWidgetID")){}
FrameRecord & FrameRecord::operator=(const FrameRecord & other) {
    this->ID = other.ID;
    this->ptr = other.ptr;
    this->attach = other.attach;
    this->userType = other.userType;
    this->mainWindowID = other.mainWindowID;
    this->docWidgetID = other.docWidgetID;
    return *this;
}

MainWindowRecord::MainWindowRecord() :
    ID(0),
    ptr(nullptr){}
MainWindowRecord::MainWindowRecord(const QSqlQuery & query) :
    ID(qVal<decltype(ID)>(query, "ID")),
    ptr(qVal<decltype(ptr)>(query, "ptr")){}
MainWindowRecord & MainWindowRecord::operator=(const MainWindowRecord & other) {
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

bool CloseFilter::eventFilter(QObject *obj, QEvent *event) {
    (void) event;
    if (event->type() == QEvent::Close)
        m_fn(obj);
    return false;
}
bool MoveFilter::eventFilter(QObject *obj, QEvent *event) {
    (void) event;
    static int i = 0;
    switch (event->type()) {
        case QEvent::Move:
            qDebug() << i++ << "QEvent::Move";
            break;
        // case QEvent::DragMove:
        //     qDebug() << i++ << "QEvent::DragMove";
        //     break;
        // case QEvent::MouseMove:
        //     qDebug() << i++ << "QEvent::MouseMove";
        //     break;
        default:
            break;
    }
    //m_fn(obj);
    return false;
}


Emdi::Emdi() {
#if defined(QT_DEBUG)
    qDebug("Hi from lib qt_debug");
#elif defined(QT_NO_DEBUG)
    qDebug("Hi from lib qt_no_debug");
#endif
    _dbInitDb();
    QObject::connect(qApp, &QApplication::focusChanged, this, &Emdi::_onFocusChanged);

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
                       "    (SELECT DISTINCT userType, mainWindowID,                                  \n"
                       "                     COUNT(userType)>1 as fail                                \n"
                       "     FROM            frames                                                   \n"
                       "     WHERE           attach is 'Dock'                                         \n"
                       "     GROUP BY mainWindowID, userType)                                         \n";
    const
    QStringList qsl = {"DROP TABLE IF EXISTS docs;                                                      ",
                       "DROP TABLE IF EXISTS docWidgets;                                                ",
                       "DROP TABLE IF EXISTS frames;                                                    ",
                       "DROP TABLE IF EXISTS mainWindows;                                               ",
                       "CREATE TABLE docs (ID   INTEGER PRIMARY KEY AUTOINCREMENT,                    \n"
                       "                   ptr  INTEGER UNIQUE,                                       \n"
                       "                   name TEXT CHECK(length(name) > 0));                          ",

                       "CREATE TABLE docWidgets  (ID       INTEGER PRIMARY KEY AUTOINCREMENT,         \n"
                       "                          ptr      INTEGER UNIQUE,                            \n"
                       //"                          userType TEXT CHECK(userType IS NOT NULL AND        \n"
                       //"                                              length(userType) > 0),          \n"
                       "                          frameID  INTEGER REFERENCES frames(ID),             \n"
                       "                          docID    INTEGER REFERENCES docs(ID));                ",

                       "CREATE TABLE frames (ID           INTEGER PRIMARY KEY AUTOINCREMENT,          \n"
                       "                     ptr          INTEGER UNIQUE,                             \n"
                       "                     attach       TEXT CHECK (attach IN ('MDI', 'Dock')),     \n"
                       "                     userType     TEXT CHECK(length(userType) > 0),           \n"
                       "                     docWidgetID  INTEGER UNIQUE,                             \n"
                       "                     mainWindowID INTEGER,                                    \n"
                       "                     FOREIGN KEY(docWidgetID) REFERENCES docWidgets(ID),      \n"
                       "                     FOREIGN KEY(mainWindowID) REFERENCES mainWindows(ID));   \n",

                       "CREATE TABLE mainWindows (ID          INTEGER PRIMARY KEY AUTOINCREMENT,      \n"
                       "                          selected    INTEGER UNIQUE,                         \n"
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
                       "    SELECT RAISE(ABORT, 'Cannot allow more than one type of userType in mainWindow'); \n"
                       "END;                                                                            "};
    executeList(query, qsl, "Could not init", __LINE__);

}
DocRecord Emdi::_dbAddDocument(const Document *ptr) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    query.prepare("INSERT INTO docs (ptr,name) VALUES (:ptr, :name)");
    query.bindValue(":ptr", uint64_t(ptr));
    query.bindValue(":name", QString::fromStdString(ptr->name()));
    if (!query.exec())
        fatalStr(querr("Could not execute add Document", query), __LINE__);
    return *getRecord<DocRecord>("ptr", ptr);
}
void Emdi::_dbRemoveDocument(const DocRecord & dr) {
    // This could be called originally from UI or from onMdiClosed
    // Close all the MDI frames and docWidgets relating to this doc
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString("SELECT   *                                  "
                        "FROM     frames                             "
                        "JOIN     docWidgets                         "
                        "ON       frames.docWidgetID = docWidgets.ID "
                        "WHERE    docWidgets.docID = %1 AND          "
                        "         frames.attach = 'MDI';             ").
                        arg(dr.ID);

    // If this fn was called from onMdiClosed, then it must be because
    // it was the last mdiSubwindow, in which case the below should be
    // empty, and the docWidget associated with that mdiSubwindow will
    // be deleted automatically.  This loop will also delete all the
    // existing docWidgets which are currently associated with an
    // mdiSubwindow.  The subWidgets associated with Docks need to be
    // deleted.
    auto frs = getRecords<FrameRecord>(s);
    for (const FrameRecord & fr : frs) {
        fr.ptr->close();
    }

    // For now, let's see if this works.  We are risking deleting the pointer
    // to docWidgets which are currently the children of active/exposed Docks.
    // If this fails, then we need to break this up and deal with those separately.
    s = QString("SELECT   docWidgets.*                       "
                "FROM     docWidgets                         "
                "JOIN     frames                             "
                "ON       docWidgets.ID = frames.docWidgetID "
                "WHERE    docWidgets.docID = %1 AND          "
                "         frames.attach = 'Dock';            ").
                arg(dr.ID);
    auto dwrs = getRecords<DocWidgetRecord>(s);

    // Remove all the above Docked docWidgets and the main doc.
    auto qsl = QStringList({QString("DELETE FROM docWidgets WHERE docID = %1;").arg(dr.ID),
                            QString("DELETE FROM docs WHERE ID = %1;").arg(dr.ID)});
    executeList(query, qsl, "Could not delete frame and docWidget", __LINE__);

    // Destroy the docWidgets
    for (const DocWidgetRecord & dwr : dwrs)
        delete dwr.ptr;
}
bool Emdi::_dbRemoveDocument(const Document *ptr) {
    auto dropt = getRecord<DocRecord>("ptr", ptr);
    if(dropt) {
        _dbRemoveDocument(*dropt);
        return true;
    } else {
        return false;
    }
}
MainWindowRecord Emdi::_dbAddMainWindow(const QMainWindow *ptr) {
    // Add a main window to the database
    // Make sure selected is the highest select
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString("INSERT INTO mainWindows (selected, ptr)      \n "
                        "VALUES ((SELECT IFNULL(MAX(selected), 0) + 1 \n "
                        "         FROM    mainWindows), %1);").arg(uint64_t(ptr));
    if (!query.exec(s))
        fatalStr(querr("Could not execute add mainWindow", query), __LINE__);
    return *getRecord<MainWindowRecord>("ptr", ptr);
}
std::optional<MainWindowRecord> Emdi::_dbMainWindow(unsigned int offset) {
    // Return most recently selected mainWindow, or if offset > 0, that row
    // or nullopt if nothing found
    QString s = QString("SELECT * FROM mainWindows "
                        "ORDER BY selected DESC    "
                        "LIMIT 1 OFFSET %2;").arg(offset);
    return getRecord<MainWindowRecord>(s);
}
DocWidgetRecord Emdi::_dbAddDocWidget(const QWidget *ptr, unsigned int docID) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString("INSERT INTO docWidgets (ptr,docID) VALUES (%1,%2);").
                arg(uint64_t(ptr)).arg(docID);
    if (!query.exec(s))
        fatalStr(querr("Could not execute add docWidget", query), __LINE__);
    return *getRecord<DocWidgetRecord>("ptr", ptr);
}
FrameRecord Emdi::_dbAddFrame(const QWidget *ptr, AttachmentType at, const std::string & userType,
                       unsigned int mwID) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString("INSERT INTO frames (ptr,attach,userType,docWidgetID,mainWindowID) \n"
                        "VALUES (:ptr,:attach,:userType,:dwid,:mwid);");
    query.prepare(s);
    query.bindValue(":ptr", uint64_t(ptr));
    query.bindValue(":attach", at == AttachmentType::Dock ? "Dock" : "MDI");
    query.bindValue(":userType", QString::fromStdString(userType));
    query.bindValue(":mwid", mwID);
    if (!query.exec())
        fatalStr(querr("Could not execute add Frame", query), __LINE__);
    return *getRecord<FrameRecord>("ptr", ptr);
}
FrameRecord Emdi::_dbAttachDocWidgetToFrame(const DocWidgetRecord & dwr, const FrameRecord & fr) {
    // Modifies one FrameRecord
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QStringList qsl = {QString("UPDATE frames SET docWidgetID = %1 WHERE  ID is %2;").
                             arg(dwr.ID).arg(fr.ID),
                       QString("UPDATE docWidgets SET frameID = %1 WHERE ID is %2;").
                             arg(fr.ID).arg(dwr.ID)};
    executeList(query, qsl, "Could not attach docWidget to frame", __LINE__);
    return *getRecord<FrameRecord>("ID", fr.ID);
}
FrameRecord Emdi::_newMdiFrame(const DocWidgetRecord & dwr, const std::string & userType, const MainWindowRecord & mwr) {
    // Create new MDI frame as subroutine of newMdiFrame and duplicateMdiFrame
    QMdiSubWindow *frame = m_mdiSubWindowCtor ? m_mdiSubWindowCtor() : new QMdiSubWindow;
    MoveFilter *mf = new MoveFilter(frame, this, [](QObject *obj){});
    frame->installEventFilter(mf);

    FrameRecord fr = _dbAddFrame(frame, AttachmentType::MDI, userType, mwr.ID);
    _dbAttachDocWidgetToFrame(dwr, fr);
    QObject::connect(frame, &QObject::destroyed, this, &Emdi::_onMdiClosed);
    frame->setWidget(dwr.ptr);
    QMdiArea *mdi = static_cast<QMdiArea *>(mwr.ptr->centralWidget());
    mdi->addSubWindow(frame);
    mdi->setActiveSubWindow(frame);
    frame->setAttribute(Qt::WA_DeleteOnClose);
    frame->setWindowTitle(QString::fromStdString(userType));
    frame->show();
    return fr;
}

void Emdi::_updateDockFrames(std::optional<MainWindowRecord> mwropt) {
    // Find the current MDI to get the current doc
    // For each Dock frame, attach the associated docWidget
    std::optional<DocRecord> dropt;
    std::optional<FrameRecord> fropt;

    if (mwropt) {
        QMdiArea *mdi = static_cast<QMdiArea *>(mwropt->ptr->centralWidget());
        QWidget *sw = mdi->activeSubWindow();
        if (!sw) {
            _clearDockFrames();
            return;
        }
        fropt = getRecord<FrameRecord>("ptr", sw);
        dropt = getRecord<DocRecord>(
                    QString("SELECT docs.*                              "
                            "FROM   docs                                "
                            "JOIN   docWidgets                          "
                            "ON     docs.ID = docWidgets.docID          "
                            "JOIN   frames                              "
                            "ON     docWidgets.ID = frames.docWidgetID  "
                            "WHERE  frames.ID = %1                      ").
                            arg(fropt->ID));
    } else {
        mwropt = _dbMainWindow();
        fropt = _selectedMdiFrame();
        dropt = _selectedDoc();
    }

    assert(mwropt);
    if (!dropt) return;
    assert(fropt);

    // Select all Dock frames in this or given window
    const QString
    dockFrameStr = QString("SELECT  *                   "
                           "FROM    frames              "
                           "WHERE   attach = 'Dock' AND "
                           "        mainWindowID = %1;").
                           arg(mwropt->ID);

    // Find correct docWidget based on loop Dock frame and
    // selected MDI Frame
    const QString
    docWidgetStr = "SELECT dw2.*                       "
                   "FROM   docWidgets dw1              "
                   "JOIN   frames fr_sel               "
                   "ON     dw1.ID = fr_sel.docWidgetID "
                   "JOIN   docWidgets dw2              "
                   "ON     dw1.docID = dw2.docID       "
                   "WHERE  dw2.frameID = %1 AND        "
                   "       fr_sel.ID = %2;             ";

    for (FrameRecord fr : getRecords<FrameRecord>(dockFrameStr)) {
        QWidget *ptr = nullptr;
        // See if there is DocWidget that belongs to this MDI Frame
        QString dws = docWidgetStr.arg(fr.ID).arg(fropt->ID);
        auto dwropt = getRecord<DocWidgetRecord>(dws);
        if (dwropt) { // attach if already exists
            _dbAttachDocWidgetToFrame(*dwropt, fr);
            ptr = dwropt->ptr;
        } else { // attempt to create new docWidget
            QWidget *docWidget = dropt->ptr->newView(fr.userType);
            if(docWidget) { // New view is valid
                DocWidgetRecord dwr = _dbAddDocWidget(docWidget, dropt->ID);
                _dbAttachDocWidgetToFrame(dwr, fr);
                ptr = dwr.ptr;
            }
        }
        static_cast<QDockWidget *>(fr.ptr)->setWidget(ptr);
    }
}
void Emdi::_clearDockFrames() {
    // Remove all docWidgets from all dock frames
    auto mwr = *_dbMainWindow();
    // Nullifies docWidgetID from all dock frames in this mainWindow
    QSqlQuery query(QSqlDatabase::database("connviews"));
    if (!query.exec(QString("UPDATE frames               \n"
                            "SET    docWidgetID = NULL   \n"
                            "WHERE  attach ='Dock' AND   \n"
                            "       mainWindowID = %1;").arg(mwr.ID)))
        fatalStr(querr("Could not update frames table", query), __LINE__);

    for (FrameRecord fr : getRecords<FrameRecord>(
             QString("SELECT  *                   \n"
                     "FROM    frames              \n"
                     "WHERE   attach = 'Dock' AND \n"
                     "        mainWindowID = %1;").arg(mwr.ID))) {
        static_cast<QDockWidget *>(fr.ptr)->setWidget(nullptr);
    }
}
std::optional<FrameRecord> Emdi::_selectedMdiFrame(const QMainWindow *mainWindow) {
    (void) mainWindow;
    auto mwropt = _dbMainWindow();
    if (!mwropt)
        return std::nullopt;
    QMdiArea *mdi = static_cast<QMdiArea *>(mwropt->ptr->centralWidget());
    QMdiSubWindow *sw = mdi->activeSubWindow();
    if (!sw)
        return std::nullopt;
    else
        return getRecord<FrameRecord>("ptr", sw);
}
std::optional<DocWidgetRecord> Emdi::_selectedDocWidget(const QMainWindow *mainWindow) {
    auto fropt = _selectedMdiFrame(mainWindow);
    if (!fropt)
        return std::nullopt;
    else
        return getRecord<DocWidgetRecord>("ID", fropt->docWidgetID);
}
std::optional<DocRecord> Emdi::_selectedDoc(const QMainWindow *mainWindow) {
    auto dwropt = _selectedDocWidget(mainWindow);
    if (!dwropt)
        return std::nullopt;
    else
        return getRecord<DocRecord>("ID", dwropt->docID);
}
unsigned int Emdi::_dbCountMdiFrames() {
    // Return how many MDI frames are in the current mainwindow
    auto mwropt = _dbMainWindow();
    if (!mwropt)
        return 0;
    unsigned int mwID = mwropt->ID;
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString("SELECT count(ID)             "
                        "FROM   frames                "
                        "WHERE  mainWindowID = %1 AND "
                        "       attach = 'MDI'").arg(mwID);
    if (!query.exec(s))
        fatalStr(querr("Could not count MDI frames", query), __LINE__);
    query.next();
    return qVal<unsigned int>(query);
}
unsigned int Emdi::_dbCountMainWindows() {
    // Return how many mainWindows there are
    QSqlQuery query(QSqlDatabase::database("connviews"));
    if (!query.exec("SELECT count(ID) FROM mainWindows;"))
        fatalStr(querr("Could not count mainWindows", query), __LINE__);
    query.next();
    return qVal<unsigned int>(query);
}
void Emdi::_dbIncrMainWindow(unsigned int ID) {
    // Set the "selected" column of mainWindow given by ID to maximum + 1
    // This allows us to see which was the most recently selected window
    // and allows a natural ordering as the windows are created, selected,
    // and deleted
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString("UPDATE mainWindows                        \n "
                        "SET    selected = (SELECT MAX(selected)+1 \n"
                        "                   FROM   mainWindows)    \n"
                        "WHERE  ID = %1;").arg(ID);
    if (!query.exec(s))
        fatalStr(querr("Could not increment mainWindow selected", query), __LINE__);
}
void Emdi::_dbMoveMdiFrame(const FrameRecord &fr, const MainWindowRecord & oldmwr, const MainWindowRecord & newmwr) {
    // Move frame from one mainWindow to the other mainWindow
    // Assign frame record's mainWindowID to mwr.ID
    assert(fr.attach == AttachmentType::MDI);
    QMdiArea *oldmdi = static_cast<QMdiArea *>(oldmwr.ptr->centralWidget());
    QMdiArea *newmdi = static_cast<QMdiArea *>(newmwr.ptr->centralWidget());
    oldmdi->removeSubWindow(fr.ptr);
    newmdi->addSubWindow(fr.ptr);
    fr.ptr->show();
    fr.ptr->activateWindow();
   
QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString("UPDATE frames              \n "
                        "SET    mainWindowID = %1   \n"
                        "WHERE  ID = %2;").
                arg(newmwr.ID).
                arg(fr.ID);
    if (!query.exec(s))
        fatalStr(querr("Could not increment mainWindow selected", query), __LINE__);
}
std::optional<MainWindowRecord> Emdi::_dbEmptyMainWindow() {
    // Finds a mainWindow in db which does not have any MDI frames
    // Returns the one with highest selected value
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = "SELECT *                                        "
                "FROM   mainWindows                              "
                "EXCEPT                                          "
                "SELECT mainWindows.*                            "
                "FROM   mainWindows                              "
                "JOIN   frames                                   "
                "ON     mainWindows.ID = frames.mainWindowID AND "
                "       attach = 'MDI'                           "
                "ORDER  BY selected DESC                         "
                "LIMIT  1;                                       ";
    if (!query.exec(s))
        fatalStr(querr("Could not increment mainWindow selected", query), __LINE__);
    if (query.next())
        return std::move(query); // auto conversion to proper return type
    else
        return std::nullopt;
}
std::vector<std::string> Emdi::_dbDockFrameUserTypes(const FrameRecord & fr) {
    // Return list of usertypes associated with this mainwindow
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString("SELECT fr_dock.userType            \n"
                        "FROM   frames fr_sel               \n"
                        "JOIN   docWidgets dw1              \n"
                        "ON     fr_sel.docWidgetID = dw1.ID \n"
                        "JOIN   docWidgets dw2              \n"
                        "ON     dw1.docID = dw2.docID       \n"
                        "JOIN   frames fr_dock              \n"
                        "ON     dw2.frameID = fr_dock.ID    \n"
                        "WHERE  fr_sel.ID = %1 AND          \n"
                        "       fr_dock.attach = 'Dock';    \n").
                        arg(fr.ID);

    if (!query.exec(s))
        fatalStr(querr("Could not find userTypes", query), __LINE__);
    std::vector<std::string> ret;
    while (query.next()) {
        ret.push_back(qVal<QString>(query, "userType").toStdString());
    }
    return ret;
}
void Emdi::setMainWindowCtor(const QMainWindowFn_t & fn) {
    m_mainWindowCtor = fn;
}
void Emdi::setMdiWindowCtor(const QMdiSubWindowFn_t & fn) {
    m_mdiSubWindowCtor = fn;
}
void Emdi::setDockWidgetCtor(const QDockWidgetFn_t & fn) {
    m_dockWidgetCtor = fn;
}
void Emdi::newMainWindow() {
    // Creates and sets up new mainwindow
    // then calls the other addMainWindow to make sure
    // it goes in the db
    QMainWindow *mainWindow = m_mainWindowCtor ? m_mainWindowCtor() : new QMainWindow;
    _dbAddMainWindow(mainWindow);
    QMdiArea *mdi = new QMdiArea();
    mainWindow->setCentralWidget(mdi);
    QObject::connect(mdi, &QMdiArea::subWindowActivated, this, &Emdi::_onMdiActivated);

    // Make sure we know who is selected
    mainWindow->setFocusPolicy(Qt::ClickFocus);

    // Install MDI select filter so we can access mdiarea when closing, before destructor
    std::function<void(QObject *)> f2 = [this](QObject *obj) {_onMainWindowClosed(obj);};
    CloseFilter *cf = new CloseFilter(mainWindow, this, f2);
    mainWindow->installEventFilter(cf);
    mainWindow->show();
    mainWindow->setFocus(Qt::MouseFocusReason);
}
void Emdi::openDocument(const Document *doc) {
    // Don't allow nameless docs to be added
    assert(doc->name().size());
    _dbAddDocument(doc);
}
void Emdi::closeAll() {
    auto mwrs = getRecords<MainWindowRecord>("SELECT * FROM mainWindows;");
    for (const MainWindowRecord & mwr : mwrs) {
        mwr.ptr->close();
    }
}
bool Emdi::closeDocument() {
    std::optional<DocRecord> dropt = _selectedDoc();
    if (dropt) {
        return(closeDocument(dropt->ptr));
    } else {
        return false;
    }
}
bool Emdi::closeDocument(const std::string & name) {
    assert(name.size());
    std::optional<DocRecord> dropt = getRecord<DocRecord>("name", name);
    if (dropt) {
        return(closeDocument(dropt->ptr));
    } else {
        return false;
    }
}
bool Emdi::closeDocument(Document *ptr) {
    if (_dbRemoveDocument(ptr)) {
        // Remove doc from db, close, notify listeners
        ptr->done();
        emit docClosed(ptr);
        return true;
    } else {
        qDebug() << "Can't find document " << ptr;
        return false;
    }
}
void Emdi::newMdiFrame(const std::string & docName, const std::string & userType /*, QMainWindow *mainWindow*/) {
    // Always new MDI view and new DocWidget attaching to doc given by docName.
    // docName is critical -- error if not found or empty
    // userType is not critical -- just used in title
    // Uses the first docName found, so docName should be unique in database

    // Make sure we have a document
    assert(docName.size());
    auto dropt = getRecord<DocRecord>("name", docName);
    assert(dropt);

    // Make sure we have a mainWindow
    auto mwropt = _dbMainWindow();
    if (!mwropt)
        newMainWindow();
    auto mwr = *_dbMainWindow();

    // Ensure doc is open, then get a view
    Document *doc = dropt->ptr;
    bool oldActive = doc->isActive(); // remember for a few lines
    if (!oldActive)
        doc->init(); // generic version of "open"
    QWidget *docWidget = doc->newView(userType);
    if (!docWidget) {
        if (!oldActive && doc->isActive())
            doc->done();
        return; // nullptr if doc doesn't support userType
    }

    // Add doc and mdi to db
    auto dwr = _dbAddDocWidget(docWidget, dropt->ID);
    _newMdiFrame(dwr, userType, mwr);
    // update is called by onMdiActivated
}
void Emdi::duplicateMdiFrame() {
    // Duplicate currently selected MDI view in the same mainWindow.  Does not
    // create or duplicate the document.  Requires new docWidget.
    auto mwropt = _dbMainWindow();
    if (!mwropt)
        return;
    QMdiArea *mdi = dynamic_cast<QMdiArea *>(mwropt->ptr->centralWidget());
    QMdiSubWindow *currFrame = mdi->activeSubWindow();
    if(!currFrame) return;
    auto fropt = getRecord<FrameRecord>("ptr", currFrame);
    auto dwropt = getRecord<DocWidgetRecord>("ID", fropt->docWidgetID);
    auto dropt = getRecord<DocRecord>("ID", dwropt->docID);
    QWidget *docWidget = dropt->ptr->newView(fropt->userType);
    assert(docWidget);
    DocWidgetRecord dwr = _dbAddDocWidget(docWidget, dropt->ID);
    _newMdiFrame(dwr, fropt->userType, *mwropt);
    // update is called by onMdiActivated
}
void Emdi::showDockFrame(const std::string & userType, QMainWindow *mainWindow) {
    // Look for existing dockframe, return if found, else create new one

    // TODO: use mainWindow if not null
    (void) mainWindow;
    // Make sure we have a mainWindow
    auto mwropt = getRecord<MainWindowRecord>("SELECT * FROM mainWindows LIMIT 1");
    if (!mwropt)
        newMainWindow();
    auto mwr = *_dbMainWindow();

    QString qsUserType = QString::fromStdString(userType);
    QString s = QString("SELECT *      \n"
                        "FROM   frames \n"
                        "WHERE  userType = '%1' \n"
                        "AND    mainWindowID = %2;").
                        arg(qsUserType).
                        arg(mwr.ID);
    auto fropt = getRecord<FrameRecord>(s);

    if (fropt) {
        fropt->ptr->show();
        return;
    } else {
        QDockWidget *frame = m_dockWidgetCtor ? m_dockWidgetCtor() : new QDockWidget;
        mwr.ptr->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, frame);
        _dbAddFrame(frame, AttachmentType::Dock, userType, mwr.ID);
        QObject::connect(frame, &QObject::destroyed, this, &Emdi::_onDockClosed);
        frame->setAttribute(Qt::WA_DeleteOnClose);
        frame->setWindowTitle(qsUserType);
        frame->show();
    }
    _updateDockFrames();
    }

// Public Slots
void Emdi::_onMainWindowClosed(QObject *obj) {
    QMainWindow *mw = static_cast<QMainWindow *>(obj);
    // Uniformly close all mdiSubWindows and dockWidgets associated
    // with this mainwindow
    // This also has the desired effect of closing and removing
    // the docWidgets and the docs from the db.
    MainWindowRecord mwr = *getRecord<MainWindowRecord>("ptr", mw);
    QString s = QString("SELECT * FROM frames WHERE mainWindowID = %1").arg(mwr.ID);
    auto frs = getRecords<FrameRecord>(s);
    for (const FrameRecord & fr : frs) {
        fr.ptr->close();
    }

    QSqlQuery query(QSqlDatabase::database("connviews"));
    s = QString("DELETE FROM mainWindows WHERE ID = %1;").arg(mwr.ID);
    if (!query.exec(s))
        fatalStr(querr("Could not remove mainWindow or frames pointing to it", query), __LINE__);
}
void Emdi::_onMdiActivated(QMdiSubWindow *sw) {
    if (sw) {
        _updateDockFrames();
    }
}
void Emdi::_onMdiClosed(QObject *sw) {
    // Just remove the mdiSubWindow and associated docWidgets from db
    // Their destruction is handled automatically.  Also close mainWindow
    // if it has no more mdiSubWindows, but only if it's not the last one.
    QMdiSubWindow *mdiSubWindow = static_cast<QMdiSubWindow *>(sw);
    FrameRecord mdifr = *getRecord<FrameRecord>("ptr", mdiSubWindow);
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QStringList qsl = {QString("DELETE FROM frames WHERE ID = %1;").arg(mdifr.ID),
                       QString("DELETE FROM docWidgets WHERE ID = %1;").arg(mdifr.docWidgetID)};
    executeList(query, qsl, "Could not delete frame and docWidget", __LINE__);

    // At this point, the MDI and directly associated docWidget has been removed
    // The next query selects orphan docs, ie docs without a display.  There
    // should be exactly one, if any, and this is the one to close. Closing this
    // doc will trigger the other windows and records to close and be deleted.
    QString docIDsToClose = "SELECT ID from docs                       "
                            "EXCEPT                                    "
                            "SELECT DISTINCT docID                     "
                            "FROM   docWidgets                         "
                            "JOIN   frames                             "
                            "ON     docWidgets.ID = frames.docWidgetID "
                            "WHERE  frames.attach != 'Dock'            ";
    QString docsToCloseStr = QString("SELECT * FROM docs WHERE ID = (%1);").arg(docIDsToClose);
    auto docsToClose = getRecords<DocRecord>(docsToCloseStr);
    assert(docsToClose.size() <= 1);
    if (docsToClose.size()) {
        closeDocument(docsToClose[0].ptr);
    }

    // Close highest priority empty mainWindow, but only
    // if there is another mainWindow
    // TODO: put this in a function and call it cleanupMainWindows or something.
    auto emwr = _dbEmptyMainWindow();
    auto mwrs = getRecords<MainWindowRecord>("SELECT * FROM mainWindows");
    if(emwr && mwrs.size() > 1) {
        emwr->ptr->close();
    }
}
void Emdi::_onDockClosed(QObject *sw) {
    // this frame -> setWidget to nullptr
    // Find this frame's userType
    // delete all docWidgets in this window with this userType
    assert(sw);
    QDockWidget *frame = static_cast<QDockWidget *>(sw);
    auto fr = getRecord<FrameRecord>("ptr", frame);
    assert(fr);
    auto dwrs = getRecords<DocWidgetRecord>("frameID", fr->ID);
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s;
    if (dwrs.size()) {
        s = QString("DELETE FROM docWidgets                      \n"
                    "WHERE ID =                                  \n"
                    " (SELECT docWidgets.ID                      \n"
                    "  FROM   docWidgets                         \n"
                    "  WHERE  docWidgets.frameID = '%1');        \n").
                            arg(fr->ID);
        if (!query.exec(s)) {
            fatalStr(querr("Could not delete docWidgets", query), __LINE__);
        }
        for (const DocWidgetRecord & dw : dwrs) {
            delete dw.ptr;
        }
    }
    // Finally remove the frame
    s = QString("DELETE FROM frames    \n"
                "WHERE frames.ID = %1;  ").arg(fr->ID);
    if (!query.exec(s)) {
        fatalStr(querr("Could not delete frame", query), __LINE__);
    }
}
void Emdi::_onFocusChanged(QWidget *old, QWidget *now){
    // Increment the count of the newly selected mainWindow
    (void) old;
    QMainWindow *mw = now ? static_cast<QMainWindow *>(now->window()) : nullptr ;
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = "SELECT count(ID) AS CID FROM mainWindows;";
    if (!query.exec(s)) {
        fatalStr(querr("Could not count mainWindows", query), __LINE__);
    }
    query.next();
    auto mwropt = getRecord<MainWindowRecord>("ptr", mw);
    if (qVal<unsigned int>(query, "CID") && mwropt) {
        _dbIncrMainWindow(mwropt->ID);
    }
}
bool Emdi::popoutMdiFrame() {
    // Return false if there are fewer than 2 MDI frames in current window.
    // Otherwise, return true after making a new mainWindow and moving
    // currently selected MDI frame to it
    if (_dbCountMdiFrames() < 2)
        return false;
    auto fropt = _selectedMdiFrame();
    assert(fropt);

    auto oldmwropt = _dbMainWindow();
    assert(oldmwropt);

    // Use an existing empty main window, or create a new one if not found
    auto newmwropt = _dbEmptyMainWindow();
    if (!newmwropt) {
        newMainWindow();
        newmwropt = _dbMainWindow();
        assert(newmwropt);
    }
    _dbMoveMdiFrame(*fropt, *oldmwropt, *newmwropt);

    // Identify which userTypes are showing in old mainWindow for this doc
    // Show those userTypes in new window
    for (const std::string & ut : _dbDockFrameUserTypes(*fropt)) {
        showDockFrame(ut);
    }
    return true;
}
bool Emdi::duplicateAndPopoutMdiFrame() {
    duplicateMdiFrame();
    return popoutMdiFrame();
}
bool Emdi::moveMdiToPrevious() {
    // Moves current MDI frame to previously selected mainWindow
    // Fails and returns false if there is only one mainWindow
    if (_dbCountMainWindows() < 2)
        return false;
    auto fropt = _selectedMdiFrame();
    if (!fropt)
        return false;
    auto oldmwr = *_dbMainWindow();
    auto newmwr = *_dbMainWindow(1);
    _dbMoveMdiFrame(*fropt, oldmwr, newmwr);
    _updateDockFrames(oldmwr);
    _updateDockFrames(newmwr);
    // TODO: close empty mainwindows by calling cleanupMainWindows
    // TODO: based on the todo in line ~700-something.
    return true;
}
