#ifndef WORKERS_H
#define WORKERS_H


#include "vdocworker.h"



class TxtWorker : public DocWorker {
public:
    TxtWorker();
    ~TxtWorker();
    std::unique_ptr<Document> OpenDoc(const DocOpts &);
    void CloseDoc(const DocOpts &);
};

class SchWorker : public DocWorker {
public:
    SchWorker();
    ~SchWorker();
    std::unique_ptr<Document> OpenDoc(const DocOpts &);
    void CloseDoc(const DocOpts &);


};

#endif
