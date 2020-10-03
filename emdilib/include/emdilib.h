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


class Emdi {
private:
    std::vector<QMainWindow *> m_hostWindows;
    std::vector<std::unique_ptr<Document>> m_docs;
public:
    Emdi();
    ~Emdi();
    void AddHostWindow(QMainWindow *);
    void AddDocument(const std::unique_ptr<Document>);

};




#endif // EMDILIB_H
