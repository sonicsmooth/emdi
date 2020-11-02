#ifndef DOCUMENTS_H
#define DOCUMENTS_H

#include "vdocument.h"
#include <string>

class TxtDocument : public Document {
private:
    const std::string m_name;
    bool m_activeState;
    DocBehavior m_behavior;
public:
    TxtDocument(const std::string &, const DocBehavior & = DocBehavior());
    ~TxtDocument() override;
    void init() override;
    void done() override;
    bool isActive() override;
    QWidget *newView(const std::string &) const override;
    const std::string & name() const override;
    void setBehavior(const DocBehavior &) override;
    const DocBehavior & behavior() const override;

};

class SchDocument : public Document {
private:
    const std::string m_name;
    bool m_activeState;
    DocBehavior m_behavior;
public:
    SchDocument(const std::string &, const DocBehavior & = DocBehavior());
    ~SchDocument() override;
    void init() override;
    void done() override;
    bool isActive() override;
    QWidget *newView(const std::string &) const override;
    const std::string & name() const override;
    void setBehavior(const DocBehavior &) override;
    const DocBehavior & behavior() const override;
};


#endif
