#ifndef DOCTHREADWRAPPER_H
#define DOCTHREADWRAPPER_H

#include "idocument.h"
#include <QDebug>
#include <QObject>
#include <QThread>
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
    QThread *m_thread;

public:
    DocThreadWrapper(std::unique_ptr<IDocument> dp) :
        m_doc(std::move(dp)),
        m_thread(new QThread)
    {
        m_doc->setWrapper(this);
        moveToThread(m_thread);
        connect(m_thread, &QThread::finished, []{
            qDebug() << "Threadwrapper thread finished";
        });
        m_thread->start();
        qDebug() << "Wrapper thread" << m_thread;
    }
    DocThreadWrapper(const DocThreadWrapper & ) {}
    ~DocThreadWrapper() override {
        m_thread->quit();
        m_thread->wait();
        delete m_thread;
    }
    IDocument *doc() const {return m_doc.get();}
// The slots largely match the IDocument functions
public slots:
    void init() {m_doc->init();}
    void done() {m_doc->done();}
    bool isActive(Qt::ConnectionType ct) {return m_doc->isActive();}
    bool supportsUserType(const std::string & userType, Qt::ConnectionType ct) {
        return m_doc->supportsUserType(userType);}
    void accept(IDocVisitor *dv, Qt::ConnectionType ct) {m_doc->accept(dv);}
    void accept(const IDocVisitor *dv, Qt::ConnectionType ct) {m_doc->accept(dv);}
    void accept(IDocVisitor *dv, Qt::ConnectionType ct) const {m_doc->accept(dv);}
    void accept(const IDocVisitor *dv, Qt::ConnectionType ct) const {m_doc->accept(dv);}
    void setName(const std::string & name, Qt::ConnectionType ct) {m_doc->setName(name);}
signals:
    void initDone();
    void doneDone();
};


#endif // DOCTHREADWRAPPER_H
