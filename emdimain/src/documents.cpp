#include "documents.h"

#include <QTextEdit>
#include <QDebug>

TxtDocument::TxtDocument(const std::string & name) :
    m_name(name),
    m_activeState(false)
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

QWidget *TxtDocument::newView(const std::string & userType) const {
    qDebug() << QString("TxtDocument::newView(%1) (%2)").
                arg(userType.c_str()).arg(m_name.c_str());
    return new QTextEdit(QString("TxtDocument/%1/%2").
                         arg(m_name.c_str()).arg(userType.c_str()));
}
const std::string & TxtDocument::name() const {
    return m_name;
}



SchDocument::SchDocument(const std::string & name) :
    m_name(name),
    m_activeState(false)
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

QWidget *SchDocument::newView(const std::string & userType) const {
    qDebug() << QString("SchDocument::newView(%1) (%2)").
                arg(userType.c_str()).arg(m_name.c_str());
    return new QTextEdit(QString("SchDocument/%1/%2").
                         arg(m_name.c_str()).arg(userType.c_str()));

}
const std::string & SchDocument::name() const {
    return m_name;
}
