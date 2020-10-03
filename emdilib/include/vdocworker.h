#ifndef DOCWORKER_H
#define DOCWORKER_H

#include "vdocument.h"
#include <memory>
#include <string>


typedef struct {
    std::string name;
    std::string viewtype;
} DocOpts;

class DocWorker {
public:
    virtual ~DocWorker() {}
    virtual std::unique_ptr<Document> OpenDoc(const DocOpts &) = 0;
    virtual void CloseDoc(const DocOpts &) = 0;
};



#endif
