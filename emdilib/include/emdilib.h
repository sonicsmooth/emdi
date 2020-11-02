#ifndef EMDILIB_H
#define EMDILIB_H

#include "emdilib_global.h"
#include "vdocument.h"

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QWidget>

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
    Document *ptr;
    std::string name;
    DocRecord();
    DocRecord(const QSqlQuery &) ;
    DocRecord & operator=(const DocRecord &);
};

struct DocWidgetRecord {
    unsigned int ID;
    QWidget *ptr;
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

QString limitstr(int);
QString selectStr(const QString & table, const QString & field, unsigned int, int = -1);
//QString selectStr(const QString & table, const QString & field, uint64_t, int = -1);
QString selectStr(const QString & table, const QString & field, const std::string &, int = -1);
QString selectStr(const QString & table, const QString & field, const QMainWindow *, int = -1);
QString selectStr(const QString & table, const QString & field, const QWidget *, int = -1);
QString selectStr(const QString & table, const QString & field, const Document *, int = -1);
QString selectStr(const QString & table, const QString & field, AttachmentType, int = -1);


template<typename T> QString tableName() {return "undefined";}
template<> inline QString tableName<DocRecord>() {return "docs";}
template<> inline QString tableName<DocWidgetRecord>() {return "docWidgets";}
template<> inline QString tableName<FrameRecord>() {return "frames";}
template<> inline QString tableName<MainWindowRecord>() {return "mainWindows";}

// TODO: Figure out getRecord and getRecords with 
// TODO: name/value pairs of arbitrary type value
template<typename RET_T, typename ARG_T>
std::optional<RET_T> getRecord(const QString & field, ARG_T val) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = selectStr(tableName<RET_T>(), field, val, 1);
    if (!query.exec(s))
        fatalStr(querr("Could not execute find record", query), __LINE__);
    if(query.first())
        return query;
    return std::nullopt;

}
template<typename RET_T, typename ARG_T>
std::vector<RET_T> getRecords(const QString & field, ARG_T val) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = selectStr(tableName<RET_T>(), field, val, -1);
    if (!query.exec(s))
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


class Emdi : public QObject {
    Q_OBJECT
private:
    void _dbInitDb();
    void _dbAddDocument(const Document *);
    void _dbCloseDocument(const DocRecord &);
    void _dbAddMainWindow(const QMainWindow *);
    MainWindowRecord _dbMainWindow(const QMainWindow * = nullptr);
    void _dbAddDocWidget(const QWidget *, const std::string &, unsigned int);
    void _dbAddFrame(const QWidget *, AttachmentType, const std::string &, int, unsigned int);
    void _dbUpdateFrameDocWidgetID(unsigned int, unsigned int);
    void _newMdiFrame(const DocWidgetRecord &, const std::string & userType, const MainWindowRecord &);
    void _updateDockFrames(const std::optional<DocRecord> &, const MainWindowRecord &);
    std::optional<FrameRecord> _selectedMdiFrame(const QMainWindow * = nullptr);
    std::optional<DocWidgetRecord> _selectedDocWidget(const QMainWindow * = nullptr);
    std::optional<DocRecord> _selectedDoc(const QMainWindow * = nullptr);
    

public:
    Emdi();
    ~Emdi();
    void addMainWindow(QMainWindow *);
    void addDocument(const Document *);
    // removeDocument
    // openDocument
    void closeDocument(const std::string & = "");
    void newMdiFrame(const std::string & docName, const std::string & userType, QMainWindow *mainWindow = nullptr);
    void duplicateMdiFrame();
    void showDockFrame(const std::string & userType, /* TODO: const */ QMainWindow *mainWindow = nullptr);

signals:
    void destroy(void *);
public slots:
    void _onMdiActivated(QMdiSubWindow *);
    void _onMdiClosed(QObject *);
    void _onDockClosed(QObject *);

};

class FilterObject : public QObject {
private:
    Emdi *m_emdi;
public:
    FilterObject(QObject *parent, Emdi *emdi) : QObject(parent), m_emdi(emdi){}
    ~FilterObject() override {qDebug("~FilterObject()");}
    bool eventFilter(QObject *watched, QEvent *event) override;
};



#endif // EMDILIB_H
