#include "emdilib.h"
#include "docworker.h"

#include <iostream>

#include <QString>
#include <QDebug>

Emdi::Emdi() {
#if defined(QT_DEBUG)
    qDebug("Hi from lib qt_debug");
#elif defined(QT_NO_DEBUG)
   qDebug("Hi from lib qt_no_debug");
#endif
}

void Emdi::registerWorker(std::unique_ptr<DocWorker> updw) {
    qDebug("Emdi::registerWorker()");
    m_workers.push_back(std::move(updw));
}

void Emdi::openDoc(const std::string & name) {
    for (const std::unique_ptr<DocWorker> & worker : m_workers) {
        //std::cout << "what?" << worker.get();
        auto docopt = worker->OpenDoc(name);
    }
}
