#ifndef EMDILIB_H
#define EMDILIB_H

#include "emdilib_global.h"
#include "vdocument.h"

#include <QWidget>
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QSqlQuery>

#include <list>
#include <memory>
#include <string>

struct ConnView {
    unsigned int      ID;
    std::string docName;
    Document    *document;
    QWidget     *docWidget;    // eg QTextEdit, QModelView
    std::string frameType; // ie QMDISubWindow, QDockWidget
    std::string frameAttach; // ie MDISubWindow, DockWidget
    QWidget     *subWidget;    // ie QMDISubWindow, QDockWidget
    QMainWindow *mainWindow;   // ie QMainWindow
};

enum class AttachmentType {MDI, Dock};

template<typename T>
T *_ptr(const QVariant &);
unsigned int _uint(const QVariant &);
std::string _str(const QVariant &);
[[noreturn ]] void fatalStr(const QString &, int = 0);
bool fatalExec (QSqlQuery &, int = 0);
bool fatalExec (QSqlQuery &, const QString &, int = 0);
template <typename T>
T qVal(QSqlQuery &);

class Emdi {
private:
    void _dbInitDb();
    void _dbAddDocument(const Document *);
    const Document * _dbFindDocPtr(const std::string &) const; // where name ...
    const Document * _dbFindDocPtr(unsigned int) const; // where ID ...
    unsigned int _dbFindDocID(const std::string &) const; // where name ...
    unsigned int _dbFindDocID(const Document *) const; // where ptr ...
    void _dbAddMainWindow(const QMainWindow *);
    QMainWindow * _dbFindMainWindow() const;
    void _dbAddDocWidget(const QWidget *, int);
    void _dbAddFrame(const QWidget *, const std::string &, AttachmentType, int, int);
public:
    Emdi();
    ~Emdi();
    void AddMainWindow(const QMainWindow *);
    void AddDocument(const Document *);
    void ShowView(const std::string & docId, const std::string & frameType, AttachmentType);

};




#endif // EMDILIB_H
