#ifndef VDOCUMENT_H
#define VDOCUMENT_H

#include <QDebug>
#include <QWidget>
#include <string>


class Document {
public:
    virtual ~Document() {qDebug("Document::~Document()");}
    virtual QWidget *OpenView(const std::string & frameType) const = 0;
    virtual const std::string & docName() const = 0;
};



#endif
