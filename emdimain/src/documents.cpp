#include "documents.h"

#include <QTextEdit>
#include <QDebug>

TxtDocument::TxtDocument(const std::string & name) :
    m_name(name),
    m_activeState(false)
//    m_mdiBehavior(MdiBehavior::REQUIRED),
//    m_dockBehavior(DockBehavior::OPTIONAL),
//    m_closeBehavior(CloseBehavior::LAST_MDI)
{
    qDebug() << QString("TxtDocument::TxtDocument(%1)").arg(name.c_str());
}
TxtDocument::~TxtDocument() {
    done();
    qDebug() << QString("TxtDocument::~TxtDocument() (%1)").arg(m_name.c_str());
}
void TxtDocument::init() {
    if (m_activeState)
        return;
    qDebug() << "TxtDocument::init()" << m_name.c_str();
    m_activeState = true;
}
void TxtDocument::done() {
    if (!m_activeState)
        return;
    qDebug() << "TxtDocument::done() " << m_name.c_str();
    m_activeState = false;
}
bool TxtDocument::isActive() {
    return m_activeState;
}
bool TxtDocument::supportsUserType(const std::string & userType) const {
    return (userType != "Project Tree") &&
           (userType != "Hierarchy");
}
QWidget *TxtDocument::newView(const std::string & userType) const {
    if (supportsUserType(userType)) {
        qDebug() << QString("TxtDocument::newView(%1) (%2)").
                    arg(userType.c_str()).arg(m_name.c_str());
        return new QTextEdit(QString("TxtDocument/%1/%2").
                            arg(m_name.c_str()).arg(userType.c_str()));
    } else {
        return nullptr;
    }
}
const std::string & TxtDocument::name() const {
    return m_name;
}
//MdiBehavior TxtDocument::mdiBehavior() const {
//    return m_mdiBehavior;
//}
//DockBehavior TxtDocument::dockBehavior() const {
//    return m_dockBehavior;
//}
//CloseBehavior TxtDocument::closeBehavior() const {
//    return m_closeBehavior;
//}

SchDocument::SchDocument(const std::string & name) :
    m_name(name),
    m_activeState(false)
//    m_mdiBehavior(MdiBehavior::REQUIRED),
//    m_dockBehavior(DockBehavior::OPTIONAL),
//    m_closeBehavior(CloseBehavior::LAST_MDI)
{
    qDebug() << QString("SchDocument::SchDocument(%1)").arg(name.c_str());
}
SchDocument::~SchDocument() {
    done();
    qDebug() << QString("SchDocument::~SchDocument() (%1)").arg(m_name.c_str());
}
void SchDocument::init() {
    if (m_activeState)
        return;
    qDebug() << "SchDocument::init()" << m_name.c_str();
    m_activeState = true;
}
void SchDocument::done() {
    if (!m_activeState)
        return;
    qDebug() << "SchDocument::done() " << m_name.c_str();
    m_activeState = false;
}
bool SchDocument::isActive() {
    return m_activeState;
}
bool SchDocument::supportsUserType(const std::string & userType) const {
    return userType != "Project Tree";
}
QWidget *SchDocument::newView(const std::string & userType) const {
    if (supportsUserType(userType)) {
        qDebug() << QString("SchDocument::newView(%1) (%2)").
                    arg(userType.c_str()).arg(m_name.c_str());
        return new QTextEdit(QString("SchDocument/%1/%2").
                             arg(m_name.c_str()).arg(userType.c_str()));
    } else {
        return nullptr;
    }

}
const std::string & SchDocument::name() const {
    return m_name;
}
//MdiBehavior SchDocument::mdiBehavior() const {
//    return m_mdiBehavior;
//}
//DockBehavior SchDocument::dockBehavior() const {
//    return m_dockBehavior;
//}
//CloseBehavior SchDocument::closeBehavior() const {
//    return m_closeBehavior;
//}


PrjDocument::PrjDocument(const std::string & name) :
    m_name(name),
    m_activeState(false)
//    m_mdiBehavior(MdiBehavior::OPTIONAL),
//    m_dockBehavior(DockBehavior::OPTIONAL),
//    m_closeBehavior(CloseBehavior::LAST_MDI)
{
    qDebug() << QString("PrjDocument::PrjDocument(%1)").arg(name.c_str());
}
PrjDocument::~PrjDocument() {
    done();
    qDebug() << QString("PrjDocument::~PrjDocument() (%1)").arg(m_name.c_str());
}
void PrjDocument::init() {
    if (m_activeState)
        return;
    qDebug() << "PrjDocument::init()" << m_name.c_str();
    m_activeState = true;
}
void PrjDocument::done() {
    if (!m_activeState)
        return;
    qDebug() << "PrjDocument::done() " << m_name.c_str();
    m_activeState = false;
}
bool PrjDocument::isActive() {
    return m_activeState;
}
bool PrjDocument::supportsUserType(const std::string & userType) const {
    return userType == "Project Tree";
}
QWidget *PrjDocument::newView(const std::string & userType) const {
    if (supportsUserType(userType)) {
        qDebug() << QString("PrjDocument::newView(%1) (%2)").
                    arg(userType.c_str()).arg(m_name.c_str());
        return new QTextEdit(QString("PrjDocument/%1/%2").
                            arg(m_name.c_str()).arg(userType.c_str()));
    } else {
        return nullptr;
    }

}
const std::string & PrjDocument::name() const {
    return m_name;
}
//MdiBehavior PrjDocument::mdiBehavior() const {
//    return m_mdiBehavior;
//}
//DockBehavior PrjDocument::dockBehavior() const {
//    return m_dockBehavior;
//}
//CloseBehavior PrjDocument::closeBehavior() const {
//    return m_closeBehavior;
//}

