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
    QThread m_thread;
    std::unique_ptr<IDocument> m_doc;

public:
    DocThreadWrapper(std::unique_ptr<IDocument> dp) :
        m_doc(std::move(dp))//,
        //m_thread(new QThread)
    {
        m_doc->setWrapper(this);
        moveToThread(&m_thread);
        connect(&m_thread, &QThread::finished, []{
            qDebug() << "Threadwrapper thread finished";
        });
        m_thread.start();
        qDebug() << "Wrapper thread" << &m_thread;
    }
    DocThreadWrapper(const DocThreadWrapper & ) {}
    ~DocThreadWrapper() override {
        qDebug() << "DocThreadWrapper::~DocThreadWrapper" << QThread::currentThread();
        QMetaObject::invokeMethod(this, "done", Qt::BlockingQueuedConnection);
        m_thread.quit();
        m_thread.wait();
        // When this exits, m_doc is destroyed, then m_thread
        // m_doc being destroyed causes its done() function to be
        // called, which must be done in the wrapper thread
        // Problem is done() might take a long time, so how to
        // shut down multiple docs concurrently?  Probably should
        // have something at the level of the docvec calling
        // multiple done's() asynchronously, then waiting for them
        // all to finish.  So docvec must be some type of class,
        // possibly with futures or other mechanism.
    }
    IDocument *doc() const {return m_doc.get();}
// The slots largely match the IDocument functions
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
    std::string name() {return m_doc->name();}
signals:
    void initDone();
    void doneDone();
};


#endif // DOCTHREADWRAPPER_H
