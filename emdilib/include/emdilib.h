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

struct DocWidgetsRecord {
    unsigned int ID;
    QWidget *ptr;
    std::string userType;
    unsigned int docID;
    DocWidgetsRecord();
    DocWidgetsRecord(const QSqlQuery &);
    DocWidgetsRecord & operator=(const DocWidgetsRecord &);
};

struct FramesRecord {
    unsigned int ID;
    QWidget *ptr;
    AttachmentType attach;
    unsigned int mainWindowID;
    unsigned int docWidgetID;
    FramesRecord();
    FramesRecord(const QSqlQuery &);
    FramesRecord & operator=(const FramesRecord &);
};

struct MainWindowsRecord {
    unsigned int ID;
    QMainWindow *ptr;
    MainWindowsRecord();
    MainWindowsRecord(const QSqlQuery &);
    MainWindowsRecord & operator=(const MainWindowsRecord &);
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
QString selectStr(const QString & table, const QString & field, const std::string &, int = -1);
QString selectStr(const QString & table, const QString & field, const QMainWindow *, int = -1);
QString selectStr(const QString & table, const QString & field, const QWidget *, int = -1);
QString selectStr(const QString & table, const QString & field, const Document *, int = -1);
QString selectStr(const QString & table, const QString & field, AttachmentType, int = -1);


template<typename T> QString tableName() {return "undefined";}
template<> inline QString tableName<DocRecord>() {return "docs";}
template<> inline QString tableName<DocWidgetsRecord>() {return "docWidgets";}
template<> inline QString tableName<FramesRecord>() {return "frames";}
template<> inline QString tableName<MainWindowsRecord>() {return "mainWindows";}


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
    void _dbAddMainWindow(const QMainWindow *);
    std::optional<MainWindowsRecord> _dbFindLatestMainWindow() const;
    void _dbAddDocWidget(const QWidget *, const std::string &, unsigned int);
    //std::optional<DocWidgetsRecord> _dbFindDocWidgetsRecordByUserTypeDocID(const std::string &, unsigned int);
    void _dbAddFrame(const QWidget *, AttachmentType, unsigned int, unsigned int);
    std::optional<FramesRecord> _dbFindExistingDockFrame(const std::string &, unsigned int);
    void _dbUpdateFrameDocWidgetID(unsigned int, unsigned int);
    

public:
    Emdi();
    ~Emdi();
    void AddMainWindow(QMainWindow *);
    MainWindowsRecord mainWindowsRecord(QMainWindow * = nullptr);
    void AddDocument(const Document *);
    void ShowMDIView(const std::string & docName, const std::string & userType, QMainWindow *mainWindow = nullptr);
    void ShowDockView(const std::string & docName, const std::string & userType, QMainWindow *mainWindow = nullptr);
    void duplicateMDIView();
signals:
    void destroy(void *);
public slots:
    void _onMdiActivated(QMdiSubWindow *);
    void _onMdiClosed(QObject *);

};




#endif // EMDILIB_H
