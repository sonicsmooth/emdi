#ifndef DOCUMENTS_H
#define DOCUMENTS_H

#include "vdocument.h"
#include <string>

class TxtDocument : public Document {
private:
    const std::string m_name;
    bool m_activeState;
public:
    TxtDocument(const std::string &);
    ~TxtDocument() override;
    void init() override;
    void done() override;
    bool isActive() override;
    bool supportsUserType(const std::string &) const override;
    QWidget *newView(const std::string &) const override;
    const std::string & name() const override;
};

class SchDocument : public Document {
private:
    const std::string m_name;
    bool m_activeState;
public:
    SchDocument(const std::string &);
    ~SchDocument() override;
    void init() override;
    void done() override;
    bool isActive() override;
    bool supportsUserType(const std::string &) const override;
    QWidget *newView(const std::string &) const override;
    const std::string & name() const override;
};

class PrjDocument : public Document {
private:
    const std::string m_name;
    bool m_activeState;
public:
    PrjDocument(const std::string &);
    ~PrjDocument() override;
    void init() override;
    void done() override;
    bool isActive() override;
    bool supportsUserType(const std::string &) const override;
    QWidget *newView(const std::string &) const override;
    const std::string & name() const override;
};


#endif
