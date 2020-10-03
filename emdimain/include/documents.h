#ifndef DOCUMENTS_H
#define DOCUMENTS_H

#include "vdocument.h"
#include <string>

class TxtDocument : public Document {
private:
    std::string m_docId;
public:
    TxtDocument(const std::string &);
    ~TxtDocument();
    QWidget *OpenView(const std::string &);
};

class SchDocument : public Document {
private:
    std::string m_docId;
public:
    SchDocument(const std::string &);
    ~SchDocument();
    QWidget *OpenView(const std::string &);
};


#endif