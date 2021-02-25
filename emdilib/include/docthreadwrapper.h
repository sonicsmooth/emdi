#ifndef DOCTHREADWRAPPER_H
#define DOCTHREADWRAPPER_H

#include "idocument.h"
#include <QObject>
#include <memory>

// Intended to wrap an instance of IDocument
// This allows IDocuments to participate in signals/slots
// without dirtying IDocument itself with extra features
// Specifically, it is intended that each IDocument gets its
// own thread so opening/saving and other database functions
// can run concurrently without locking up the UI.

// Instantiate the wrapper with a pointer to the IDocument.
// The wrapper takes ownership of the IDocument

class DocThreadWrapper : public QObject
{
    Q_OBJECT
private:
    std::unique_ptr<IDocument> m_doc;
public:
    DocThreadWrapper(const DocThreadWrapper & ) {}
    DocThreadWrapper(std::unique_ptr<IDocument> dp) :
        m_doc(std::move(dp)){}
    IDocument *get() const {return m_doc.get();}
// The slots largely match the IDocument functions
// But also some of the save functions that real
// Documents have
public slots:
    void init() {m_doc->init();}
    void done() {m_doc->done();}
    bool isActive() {return m_doc->isActive();}
    bool supportsUserType(const std::string & userType) {
        return m_doc->supportsUserType(userType);}
    void accept(IDocVisitor *dv) {m_doc->accept(dv);}
    void accept(const IDocVisitor *dv) {m_doc->accept(dv);}
    void accept(IDocVisitor *dv) const {m_doc->accept(dv);}
    void accept(const IDocVisitor *dv) const {m_doc->accept(dv);}
    void setName(const std::string & name) {m_doc->setName(name);}
};

#endif // DOCTHREADWRAPPER_H
