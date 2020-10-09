#include "documents.h"

#include <QTextEdit>
#include <QDebug>

TxtDocument::TxtDocument(const std::string & docId) :
    m_name(docId)
{
    // Throw if can't open doc
    qDebug("TxtDocument::TxtDocument(...)");
}
TxtDocument::~TxtDocument() {
    qDebug("TxtDocument::~TxtDocument()");
}

QWidget *TxtDocument::newView(const std::string & frameType) const {
    (void) frameType;
    qDebug("TxtDocument::OpenView(...)");
    if (frameType == "Schematic")
        return new QTextEdit("Hi from main TxtDocument Schematic");
    else if (frameType == "SymView")
        return new QTextEdit("Hi from main TxtDocument SymView");
    else if (frameType == "Properties")
        return new QTextEdit("Hi from side TxtDocument Properties");
    else
        return nullptr;
}
const std::string & TxtDocument::name() const {
    return m_name;
}



SchDocument::SchDocument(const std::string & docId) :
    m_name(docId)
{
    // Throw if can't open doc
    qDebug("SchDocument::SchDocument(...)");
}
SchDocument::~SchDocument() {
    qDebug("SchDocument::~SchDocument()");
}
QWidget *SchDocument::newView(const std::string & frameType) const {
    (void) frameType;
    qDebug("SchDocument::OpenView");
    if (frameType == "Schematic")
        return new QTextEdit("Hi from main SchDocument Schematic");
    else if (frameType == "SymView")
        return new QTextEdit("Hi from main SchDocument SymView");
    else if (frameType == "Properties")
        return new QTextEdit("Hi from side SchDocument Properties");
    else
        return nullptr;
}
const std::string & SchDocument::name() const {
    return m_name;
}
