#ifndef DOCWORKER_H
#define DOCWORKER_H

#include "emdilib.h"

#include <string>
#include <list>
#include <optional>

#include<QDebug>

class Document {};
class TxtDocument : public Document {};
class SchDocument : public Document {};


class DocWorker {
public:
    virtual ~DocWorker() = 0;
    virtual Document OpenDoc(const std::string &) = 0;
};


class TxtWorker : public DocWorker {
public:
    TxtWorker();
    ~TxtWorker();
    Document OpenDoc(const std::string &);
};

class SchWorker : public DocWorker {
public:
    SchWorker();
    ~SchWorker();
    Document OpenDoc(const std::string &);
};

#endif
