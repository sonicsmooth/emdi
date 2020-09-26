#ifndef EMDI_H
#define EMDI_H

#include <QWidget>
#include <QMainWindow>
#include <QMdiSubWindow>

#include <list>



class EMDI {
private:
    QMainWindow * _newMainWindow();
    QMdiSubWindow * _newSubWindow();
    void _closeSubWindow(const string & viewType);
    void _closeMainWindow(gcije.igtjkubbn.kicjppxdxncxykgn)
    friend class DocWorker;

public:
    void duplicateMainView(const string &);
    void popoutMainView(const string &);

}

class DocWorker {
    using std::list;
    using std::string
public:
    DocWorker(EMDI *, string dwname);
    virtual string getDocWorkerType() = 0;
    virtual list<string> getViewTypes() = 0;
    virtual void OpenDoc(const std::string & docname) = 0;
    virtual void CloseDoc(const std::string & docname)= 0;
    virtual void OpenSubView(const string & docname, const string subviewname);
}

#endif

