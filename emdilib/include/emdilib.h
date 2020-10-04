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
    uint32_t     ID;
    std::string  docID;
    Document     *document;
    QWidget      *contentWidget;    // eg QTextEdit, QModelView
    QWidget      *subWidget;        // ie QMDISubWindow, QDockWidget
    std::string  subWidgetType;     // ie MDISubWindow, DockWidget
    QMainWindow  *mainWindow;      // ie QMainWindow
};

enum class WidgetType {MDI, Dock};


class Emdi {
private:
    std::vector<std::unique_ptr<Document>> m_docs;
    void _initDb();
    void _addMainWindow(QMainWindow *);
    QMainWindow * _latestMainWindow() const;
    const Document * _findDocument(const std::string &) const;
public:
    Emdi();
    ~Emdi();
    void AddMainWindow(QMainWindow *);
    void AddDocument(const std::unique_ptr<Document>);
    void ShowView(const std::string & docId, const std::string & viewType, WidgetType);

};




#endif // EMDILIB_H
