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
    QWidget      *contentWidget;    // eg QTextEdit, QModelView
    QWidget      *subWidget;        // ie QMDISubWindow, QDockWidget
    std::string  subWidgetType;     // ie MDISubWindow, DockWidget
    QMainWindow  *mainWindow;      // ie QMainWindow
    // ConnView(QMainWindow *mw) :
    //     ID(0), docID(""), contentWidget(nullptr), subWidget(nullptr),
    //     subWidgetType(""), mainWindow(mw) {}
};


class Emdi {
private:
    std::vector<QMainWindow *> m_hostWindows;
    std::vector<std::unique_ptr<Document>> m_docs;
    void _initDb();
    void _addMainWindow(QMainWindow *);
public:
    Emdi();
    ~Emdi();
    void AddMainWindow(QMainWindow *);
    void AddDocument(const std::unique_ptr<Document>);

};




#endif // EMDILIB_H
