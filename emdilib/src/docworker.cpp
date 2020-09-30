#include "docworker.h"

DocWorker::~DocWorker() {qDebug("DocWorker::~DocWorker");}

TxtWorker::TxtWorker() {qDebug("TxtWorker::TxtWorker()");}
TxtWorker::~TxtWorker() {qDebug("TxtWorker::~TxtWorker()");}
Document TxtWorker::OpenDoc(const std::string & name) {
    qDebug("TxtWorker::OpenDoc()");
    return Document();
}
SchWorker::SchWorker() {qDebug("SchWorker::SchWorker()");}
SchWorker::~SchWorker() {qDebug("SchWorker::~SchWorker()");}
Document SchWorker::OpenDoc(const std::string & name) {
    qDebug("SchWorker::OpenDoc()");
    return Document();
}

