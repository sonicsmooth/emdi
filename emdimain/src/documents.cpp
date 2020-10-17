#include "documents.h"

#include <QTextEdit>
#include <QDebug>

TxtDocument::TxtDocument(const std::string & docId) :
    m_name(docId)
{
    // Throw if can't open doc
    //qDebug("TxtDocument::TxtDocument(...)");
}
TxtDocument::~TxtDocument() {
    //qDebug("TxtDocument::~TxtDocument()");
}
void TxtDocument::init() {
    qDebug("TxtDocument::init()");
}
void TxtDocument::done() {
    qDebug("TxtDocument::done()");
}

QWidget *TxtDocument::newView(const std::string & userType) const {
    (void) userType;
    //qDebug("TxtDocument::OpenView(...)");
    QString txt = QString("Hi from TxtDocument %1 %2").
            arg(m_name.c_str()).arg(userType.c_str());
    return new QTextEdit(txt);
}
const std::string & TxtDocument::name() const {
    return m_name;
}



SchDocument::SchDocument(const std::string & docId) :
    m_name(docId)
{
    // Throw if can't open doc
    //qDebug("SchDocument::SchDocument(...)");
}
SchDocument::~SchDocument() {
    //qDebug("SchDocument::~SchDocument()");
}
void SchDocument::init() {
    qDebug("SchDocument::init()");
}
void SchDocument::done() {
    qDebug("SchDocument::done()");
}
QWidget *SchDocument::newView(const std::string & userType) const {
    (void) userType;
    //qDebug("SchDocument::OpenView");
    QString txt = QString("Hi from SchDocument %1 %2").
            arg(m_name.c_str()).arg(userType.c_str());
    return new QTextEdit(txt);
}
const std::string & SchDocument::name() const {
    return m_name;
}
