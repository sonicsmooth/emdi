#ifndef DOCUMENTS_H
#define DOCUMENTS_H

#include "vdocument.h"
#include <string>

class TxtDocument : public Document {
private:
    const std::string m_docId;
public:
    TxtDocument(const std::string &);
    ~TxtDocument();
    QWidget *OpenView(const std::string &) const override;
    const std::string & docName() const override;
};

class SchDocument : public Document {
private:
    const std::string m_docId;
public:
    SchDocument(const std::string &);
    ~SchDocument();
    QWidget *OpenView(const std::string &) const override;
    const std::string & docName() const override;
};


#endif
