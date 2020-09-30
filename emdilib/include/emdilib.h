#ifndef EMDILIB_H
#define EMDILIB_H

#include "emdilib_global.h"
#include "docworker.h"

#include <QWidget>
#include <QMainWindow>
#include <QMdiSubWindow>

#include <list>
#include <memory>
#include <string>

class DocWorker;

class Emdi {
private:
    std::vector<std::unique_ptr<DocWorker> > m_workers;
public:
    Emdi();
    void registerWorker(std::unique_ptr<DocWorker>);
    void openDoc(const std::string &);
};




#endif // EMDILIB_H
