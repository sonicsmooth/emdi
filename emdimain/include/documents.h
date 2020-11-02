#ifndef DOCUMENTS_H
#define DOCUMENTS_H

#include "vdocument.h"
#include <string>

class TxtDocument : public Document {
private:
    const std::string m_name;
    bool m_activeState;
    const MdiBehavior m_mdiBehavior;
    const DockBehavior m_dockBehavior;
    const CloseBehavior m_closeBehavior;
public:
    TxtDocument(const std::string &);
    ~TxtDocument() override;
    void init() override;
    void done() override;
    bool isActive() override;
    QWidget *newView(const std::string &) const override;
    const std::string & name() const override;
    MdiBehavior mdiBehavior() const override;
    DockBehavior dockBehavior() const override;
    CloseBehavior closeBehavior() const override;
};

class SchDocument : public Document {
private:
    const std::string m_name;
    bool m_activeState;
    const MdiBehavior m_mdiBehavior;
    const DockBehavior m_dockBehavior;
    const CloseBehavior m_closeBehavior;
public:
    SchDocument(const std::string &);
    ~SchDocument() override;
    void init() override;
    void done() override;
    bool isActive() override;
    QWidget *newView(const std::string &) const override;
    const std::string & name() const override;
    MdiBehavior mdiBehavior() const override;
    DockBehavior dockBehavior() const override;
    CloseBehavior closeBehavior() const override;
};

class PrjDocument : public Document {
private:
    const std::string m_name;
    bool m_activeState;
    const MdiBehavior m_mdiBehavior;
    const DockBehavior m_dockBehavior;
    const CloseBehavior m_closeBehavior;
public:
    PrjDocument(const std::string &);
    ~PrjDocument() override;
    void init() override;
    void done() override;
    bool isActive() override;
    QWidget *newView(const std::string &) const override;
    const std::string & name() const override;
    MdiBehavior mdiBehavior() const override;
    DockBehavior dockBehavior() const override;
    CloseBehavior closeBehavior() const override;
};


#endif
