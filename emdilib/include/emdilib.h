#ifndef EMDILIB_H
#define EMDILIB_H

#include "emdilib_global.h"
#include "idocument.h"

#include <QEvent>
#include <QFocusEvent>
#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSize>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QStackedWidget>
#include <QWidget>

#include <map>
#include <optional>


enum class AttachmentType {ERROR, MDI, Dock};
template<typename T>
T attach2str(AttachmentType at) {
    switch(at) {
    case AttachmentType::MDI: return "MDI";
    case AttachmentType::Dock: return "Dock";
    default: return "ERROR";
    }
}
template<typename T>
AttachmentType str2attach(T && s) {
    if (s == "MDI") return AttachmentType::MDI;
    if (s == "Dock") return AttachmentType::Dock;
    return AttachmentType::ERROR;
}

struct DocRecord {
    unsigned int ID;
    IDocument *ptr;
    std::string name;
    DocRecord();
    DocRecord(const QSqlQuery &) ;
    DocRecord & operator=(const DocRecord &);
};

struct DocWidgetRecord {
    unsigned int ID;
    QWidget *ptr;
    unsigned int frameID;
    unsigned int docID;
    DocWidgetRecord();
    DocWidgetRecord(const QSqlQuery &);
    DocWidgetRecord & operator=(const DocWidgetRecord &);
};

struct FrameRecord {
    unsigned int ID;
    QWidget *ptr;
    AttachmentType attach;
    std::string userType;
    unsigned int mainWindowID;
    unsigned int docWidgetID;
    FrameRecord();
    FrameRecord(const QSqlQuery &);
    FrameRecord & operator=(const FrameRecord &);
};

struct MainWindowRecord {
    unsigned int ID;
    QMainWindow *ptr;
    MainWindowRecord();
    MainWindowRecord(const QSqlQuery &);
    MainWindowRecord & operator=(const MainWindowRecord &);
};

[[noreturn ]] void fatalStr(const QString &, int = 0);
QString querr(const QString &, const QSqlQuery &);


template<typename T>
T qVal(const QSqlQuery & query, int i = 0) {
    T val = qvariant_cast<T>(query.value(i));
    return val;
}
template<typename T>
T qVal(const QSqlQuery & query, const QString & field) {
    int i = query.record().indexOf(field);
    assert(i >= 0);
    T val = qvariant_cast<T>(query.value(i));
    return val;
}


inline int argConvert(int v) {return v;}
inline const char * argConvert(const std::string & v) {return v.c_str();}
inline uint64_t argConvert(const QMainWindow *v) {return uint64_t(v);}
inline uint64_t argConvert(const QWidget *v) {return uint64_t(v);}
inline uint64_t argConvert(const IDocument *v) {return uint64_t(v);}
inline QString argConvert(AttachmentType v) {return attach2str<QString>(v);}

template<typename T> QString tableName() {return "undefined";}
template<> inline QString tableName<DocRecord>() {return "docs";}
template<> inline QString tableName<DocWidgetRecord>() {return "docWidgets";}
template<> inline QString tableName<FrameRecord>() {return "frames";}
template<> inline QString tableName<MainWindowRecord>() {return "mainWindows";}


void executeList(QSqlQuery &, const QStringList &, const QString &, int);

// TODO: Figure out getRecord and getRecords with 
// TODO: name/value pairs of arbitrary type value
// TODO: and variable arity
template<typename RET_T, typename ARG_T>
std::optional<RET_T> getRecord(const QString & field, ARG_T val) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString("SELECT * FROM %1 WHERE %2 = :val LIMIT 1;").arg(tableName<RET_T>()).arg(field);
    query.prepare(s);
    query.bindValue(":val", argConvert(val));
    if (!query.exec())
        fatalStr(querr("Could not execute find record", query), __LINE__);
    if(query.first())
        return query;
    return std::nullopt;

}
template<typename RET_T, typename ARG_T>
std::vector<RET_T> getRecords(const QString & field, ARG_T val) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = QString("SELECT * FROM %1 WHERE %2 = :val ;").arg(tableName<RET_T>()).arg(field);
    query.prepare(s);
    query.bindValue(":val", argConvert(val));
    if (!query.exec())
        fatalStr(querr("Could not execute find record", query), __LINE__);
    std::vector<RET_T> vec;
    while(query.next())
        vec.push_back(query);
    return vec;
}

// TODO: Figure out how to do prepared strings so weird values can be bound
template<typename RET_T>
std::optional<RET_T> getRecord(const QString & select) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    if (!query.exec(select))
        fatalStr(querr("Could not execute find record", query), __LINE__);
    if(query.first())
        return query;
     return std::nullopt;
}
template<typename RET_T>
std::vector<RET_T> getRecords(const QString & select) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    if (!query.exec(select))
        fatalStr(querr("Could not execute find record", query), __LINE__);
    std::vector<RET_T> vec;
    while(query.next())
        vec.push_back(query);
    return vec;
}

class Emdi;

class CloseFilter : public QObject {
private:
    Emdi *m_emdi;
    std::function<void (QObject *)> m_fn;
public:
    CloseFilter(QObject *parent, Emdi *emdi, std::function<void (QObject *)> fn) :
        QObject(parent),
        m_emdi(emdi),
        m_fn(fn) {}
    ~CloseFilter() override {qDebug("~CloseFilter()");}
    bool eventFilter(QObject *watched, QEvent *event) override;
};


class MouseMoveFilter : public QObject {
private:
    Emdi *m_emdi;
    typedef std::function<void (QObject *, QEvent *)> emdi_fn;
    emdi_fn m_moveFn;
    emdi_fn m_releaseFn;

public:
    MouseMoveFilter(QObject *parent, Emdi *emdi, emdi_fn mfn, emdi_fn rfn) :
        QObject(parent),
        m_emdi(emdi),
        m_moveFn(mfn),
        m_releaseFn(rfn) {}
    ~MouseMoveFilter() override {qDebug("~MoveFilter()");}
    bool eventFilter(QObject *watched, QEvent *event) override;
};

using QMainWindowFn_t = std::function<QMainWindow * ()>;
using QMdiSubWindowFn_t = std::function<QMdiSubWindow * ()>;
using QDockWidgetFn_t = std::function<QDockWidget * ()>;

class Emdi : public QObject {
    Q_OBJECT
private:

    QMainWindowFn_t m_mainWindowCtor;
    QMdiSubWindowFn_t m_mdiSubWindowCtor;
    QDockWidgetFn_t m_dockWidgetCtor;
    bool m_lastOutsideState;
    QStackedWidget *m_dragFrame;   // to mdiSubWindow contents while dragging

    void _dbInitDb();
    DocRecord _dbAddDocument(const IDocument *);
    void _dbRemoveDocument(const DocRecord &);
    bool _dbRemoveDocument(const IDocument *);
    MainWindowRecord _newMainWindow();
    void _dbRemoveMainWindow(const QMainWindow *);
    std::optional<MainWindowRecord> _dbMainWindow(unsigned int = 0);
    DocWidgetRecord _dbAddDocWidget(const QWidget *, unsigned int);
    FrameRecord _dbAddFrame(const QWidget *, AttachmentType, const std::string &, unsigned int);
    FrameRecord _dbAttachDocWidgetToFrame(const DocWidgetRecord &, const FrameRecord &);
    FrameRecord _newMdiFrame(const DocWidgetRecord &, const std::string & userType, const MainWindowRecord &);
    void _updateDockFrames(std::optional<MainWindowRecord> = std::nullopt);
    void _clearDockFrames();
    std::optional<FrameRecord> _selectedMdiFrame(const QMainWindow * = nullptr);
    std::optional<DocWidgetRecord> _selectedDocWidget(const QMainWindow * = nullptr);
    std::optional<DocRecord> _selectedDoc(const QMainWindow * = nullptr);
    DocRecord _mdiDoc(const QMdiSubWindow * = nullptr) const;
    unsigned int _dbCountMdiFrames();
    unsigned int _dbCountMainWindows();
    void _dbIncrMainWindow(unsigned int);
    bool _dbMoveMdiFrame(const FrameRecord &, const MainWindowRecord &, const MainWindowRecord &);
    std::optional<MainWindowRecord> _dbEmptyMainWindow();
    std::vector<std::string> _dbDockFrameUserTypes(const FrameRecord &);
    auto _calcOutside(const QMouseEvent *, const QWidget *);
    std::optional<MainWindowRecord> _findUnderWindow(const QPoint &);
    void _transparent(QWidget *, bool);
    void _moveSubToDragframe(QStackedWidget *, QMdiSubWindow *);
    void _moveDragframeToSub(QMdiSubWindow *, QStackedWidget *);
    void _mdiMoveCallback(QObject *, const QEvent *);
    void _mdiReleaseCallback(QObject *, const QEvent *);

public:
    Emdi();
    ~Emdi();
    void setMainWindowCtor(const QMainWindowFn_t &);
    void setMdiWindowCtor(const QMdiSubWindowFn_t &);
    void setDockWidgetCtor(const QDockWidgetFn_t &);
    QMainWindow *newMainWindow();
    void openDocument(const IDocument *);
    void closeAll();
    bool closeDocument();
    bool closeDocument(const std::string &);
    bool closeDocument(IDocument *);
    void newMdiFrame(const std::string & docName, const std::string & userType /*, QMainWindow *mainWindow = nullptr*/);
    void duplicateMdiFrame();
    void showDockFrame(const std::string & userType, /* TODO: const */ QMainWindow *mainWindow = nullptr);
    void closeDockFrame(const std::string & userType, /* TODO: const */ QMainWindow *mainWindow = nullptr);
    bool popoutMdiFrame();
    bool duplicateAndPopoutMdiFrame();
    bool moveMdiToPrevious();
    IDocument *document(const QMdiSubWindow *) const;
    std::string userType(const QMdiSubWindow *) const;

signals:
    void docClosed(void *);
    void dockShown(QWidget *, std::string, bool); // mainwindow is passed as arg
    void subWindowActivated(const QMdiSubWindow *);
public slots:
    void _onMainWindowClosed(QObject *);
    void _onMdiActivated(QMdiSubWindow *);
    void _onMdiClosed(QObject *);
    void _onDockClosed(QObject *);
    void _onFocusChanged(QWidget *, QWidget *);

};




#endif // EMDILIB_H
