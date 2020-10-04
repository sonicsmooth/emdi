#include "documents.h"

#include <QTextEdit>
#include <QDebug>

TxtDocument::TxtDocument(const std::string & docId) :
    m_docId(docId)
{
    // Throw if can't open doc
    qDebug("TxtDocument::TxtDocument(...)");
}
TxtDocument::~TxtDocument() {
    qDebug("TxtDocument::~TxtDocument()");
}

QWidget *TxtDocument::OpenView(const std::string & viewType) const {
    (void) viewType;
    qDebug("TxtDocument::OpenView(...)");
    if (viewType == "Schematic")
        return new QTextEdit("Hi from main TxtDocument Schematic");
    else if (viewType == "SymView")
        return new QTextEdit("Hi from side TxtDocument SymView");
    else if (viewType == "Properties")
        return new QTextEdit("Hi from side TxtDocument Properties");
    else
        return nullptr;
}
const std::string & TxtDocument::docId() const {
    return m_docId;
}



SchDocument::SchDocument(const std::string & docId) :
    m_docId(docId)
{
    // Throw if can't open doc
    qDebug("SchDocument::SchDocument(...)");
}
SchDocument::~SchDocument() {
    qDebug("SchDocument::~SchDocument()");
}
QWidget *SchDocument::OpenView(const std::string & viewType) const {
    (void) viewType;
    qDebug("SchDocument::OpenView");
    if (viewType == "Schematic")
        return new QTextEdit("Hi from main SchDocument Schematic");
    else if (viewType == "SymView")
        return new QTextEdit("Hi from side SchDocument SymView");
    else if (viewType == "Properties")
        return new QTextEdit("Hi from side SchDocument Properties");
    else
        return nullptr;
}
const std::string & SchDocument::docId() const {
    return m_docId;
}
