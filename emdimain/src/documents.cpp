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

QWidget *TxtDocument::OpenView(const std::string & viewType) {
    (void) viewType;
    qDebug("TxtDocument::OpenView(...)");
    if (viewType == "MainView")
        return new QTextEdit("Hi from main TxtDocument");
    else if (viewType == "SideView")
        return new QTextEdit("Hi from side TxtDocument");
    else
        return nullptr;
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
QWidget *SchDocument::OpenView(const std::string & viewType) {
    (void) viewType;
    qDebug("SchDocument::OpenView");
    if (viewType == "MainView")
        return new QTextEdit("Hi from main SchDocument");
    else if (viewType == "SideView")
        return new QTextEdit("Hi from side SchDocument");
    else
        return nullptr;
}
