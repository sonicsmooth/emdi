#include "emdilib.h"
#include "docworker.h"

#include <QDebug>

Emdi::Emdi() {
#if defined(QT_DEBUG)
    qDebug("Hi from lib qt_debug");
#elif defined(QT_NO_DEBUG)
   qDebug("Hi from lib qt_no_debug");
#endif
}

void Emdi::registerWorker(std::unique_ptr<DocWorker> updw) {
    qDebug("Hi from registerWorker");
    m_workers.push_back(std::move(updw));
}

void Emdi::openDoc(const std::string & name) {
    for (auto const & worker : m_workers) {
        auto docopt = worker->openDoc(name);
    }
}
