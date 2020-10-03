#ifndef VDOCUMENT_H
#define VDOCUMENT_H

#include <QDebug>
#include <QWidget>
#include <string>


class Document {
public:
    virtual ~Document() {qDebug("Document::~Document()");}
    virtual QWidget *OpenView(const std::string & viewType) = 0;
};



#endif
