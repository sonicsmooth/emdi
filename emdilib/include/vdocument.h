#ifndef VDOCUMENT_H
#define VDOCUMENT_H

#include <QDebug>
#include <QWidget>
#include <string>


class Document {
public:
    virtual ~Document() {/*qDebug("Document::~Document()");*/}
    virtual void init() = 0;
    virtual void done() = 0;
    virtual QWidget *newView(const std::string & frameType) const = 0;
    virtual const std::string & name() const = 0;
};



#endif
