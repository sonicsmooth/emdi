#ifndef VDOCUMENT_H
#define VDOCUMENT_H

#include <QDebug>
#include <QWidget>
#include <string>


class Document {
public:
    virtual ~Document() {qDebug("Document::~Document()");}
    virtual QWidget *OpenView(const std::string & viewType) const = 0;
    virtual const std::string & docId() const = 0;
};



#endif
