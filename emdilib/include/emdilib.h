#ifndef EMDILIB_H
#define EMDILIB_H

#include "emdilib_global.h"
#include "vdocument.h"

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QWidget>

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
    DocRecord(const QSqlQuery &);
};

struct DocWidgetsRecord {
    unsigned int ID;
    QWidget *ptr;
    unsigned int docID;
    DocWidgetsRecord(const QSqlQuery &);
};

struct FramesRecord {
    unsigned int ID;
    QWidget *ptr;
    std::string userType;
    AttachmentType attach;
    unsigned int mainWindowID;
    unsigned int docWidgetID;
    FramesRecord(const QSqlQuery &);
};

struct MainWindowsRecord {
    unsigned int ID;
    QMainWindow *ptr;
    MainWindowsRecord(const QSqlQuery &);
};

[[noreturn ]] void fatalStr(const QString &, int = 0);
QString querr(const QString &, const QSqlQuery &);


template<typename T>
T qVal(const QSqlQuery & query, int i = 0) {
    return qvariant_cast<T>(query.value(i));
}
template<typename T>
T qVal(const QSqlQuery & query, const QString & field) {
    int i = query.record().indexOf(field);
    return qvariant_cast<T>(query.value(i));
}

QString selectStr(const QString & table, const QString & field, unsigned int);
QString selectStr(const QString & table, const QString & field, const std::string &);
QString selectStr(const QString & table, const QString & field, QMainWindow *);
QString selectStr(const QString & table, const QString & field, const QMainWindow *);
QString selectStr(const QString & table, const QString & field, QWidget *);
QString selectStr(const QString & table, const QString & field, const QWidget *);
QString selectStr(const QString & table, const QString & field, Document *);
QString selectStr(const QString & table, const QString & field, const Document *);
QString selectStr(const QString & table, const QString & field, AttachmentType);

template<typename T>
QString tableName() {}

template<typename RET_T, typename ARG_T>
RET_T getRecord(const QString & field, ARG_T val) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    QString s = selectStr(tableName<RET_T>(), field, val);
    if (!query.exec(s))
        fatalStr(querr("Could not execute find record", query), __LINE__);
    if(!query.next())
        fatalStr(querr("Can't find record", query), __LINE__);
    return RET_T(query);
}
template<typename RET_T>
RET_T getRecord(const QString & select) {
    QSqlQuery query(QSqlDatabase::database("connviews"));
    if (!query.exec(select))
        fatalStr(querr("Could not execute find record", query), __LINE__);
    if(!query.next())
        fatalStr(querr("Can't find record", query), __LINE__);
    return RET_T(query);
}

class Emdi {
private:
    void _dbInitDb();

    void         _dbAddDocument(const Document *);
//    Document    *_dbFindDocPtr(const std::string &) const;
//    Document    *_dbFindDocPtr(unsigned int) const;
//    unsigned int _dbFindDocID(const std::string &) const;
//    unsigned int _dbFindDocID(const Document *) const;

    void         _dbAddMainWindow(const QMainWindow *);
    MainWindowsRecord _dbFindLatestMainWindow() const;
//    unsigned int _dbFindLatestMainWindowID() const;
//    QMainWindow *_dbFindMainWindowPtr(unsigned int) const;
//    unsigned int _dbFindMainWindowID(const QMainWindow *) const;

    void         _dbAddDocWidget(const QWidget *, unsigned int);
//    unsigned int _dbFindDocWidgetID(const QWidget *);
//    unsigned int _dbFindDocWidgetID(unsigned int);
//    QWidget     *_dbFindDocWidgetPtr(unsigned int, const std::string &);
//    unsigned int _dbFindDocWidgetDocID(unsigned int);
//    unsigned int _dbFindDocWidgetDocID(const QWidget *);

    void _dbAddFrame(const QWidget *, const std::string &, AttachmentType,
                     unsigned int, unsigned int);
public:
    Emdi();
    ~Emdi();
    void AddMainWindow(const QMainWindow *);
    void AddDocument(const Document *);
    void ShowView(const std::string & docId, const std::string & frameType, AttachmentType);

};




#endif // EMDILIB_H
