#ifndef EMDILIB_H
#define EMDILIB_H

#include "emdilib_global.h"
#include "vdocument.h"

#include <QWidget>
#include <QMainWindow>
#include <QMdiSubWindow>

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

enum class WidgetType {MDI, Dock};

template<typename T>
T *_ptr(const QVariant &);
unsigned int _uint(const QVariant &);
std::string _str(const QVariant &);

class Emdi {
private:
    std::vector<std::unique_ptr<Document>> m_docs;
    const Document * _findDocument(const std::string &) const;
    void _initDb();
    void _addMainWindow(const QMainWindow *);
    void _addConnView(const ConnView &);
    QMainWindow * _latestMainWindow() const;
    ConnView _findRecord(const std::string & field, const std::string & value);
    //ConnView _attachDocToDock(int ID, )
public:
    Emdi();
    ~Emdi();
    void AddMainWindow(const QMainWindow *);
    void AddDocument(const std::unique_ptr<Document>);
    void ShowView(const std::string & docId, const std::string & frameType, WidgetType);

};




#endif // EMDILIB_H
