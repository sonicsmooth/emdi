#ifndef DOCWORKER_H
#define DOCWORKER_H

#include "emdilib.h"

#include <string>
#include <list>
#include <optional>

class Document {
public:
};

class TxtDocument : public Document {

};
class SchDocument : public Document {

};


class DocWorker {
public:
    virtual ~DocWorker() = 0;
    virtual Document openDoc(const std::string &);
};


class TxtWorker : public DocWorker {
public:
     Document openDoc(const std::string &) {
         return TxtDocument();
     }
};

class SchWorker : public DocWorker {
public:
    Document openDoc(const std::string &) {
        return SchDocument();
    }
};

#endif
