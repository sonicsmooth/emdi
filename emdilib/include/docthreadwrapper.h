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
    std::unique_ptr<IDocument> doc;
public:
    DocThreadWrapper(const DocThreadWrapper & dtw) {}
    DocThreadWrapper(std::unique_ptr<IDocument> _doc) : doc(std::move(_doc)) {}
// The slots largely match the IDocument functions
// But also some of the save functions that real
// Documents have
public slots:

};

#endif // DOCTHREADWRAPPER_H
