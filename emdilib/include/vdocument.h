#ifndef VDOCUMENT_H
#define VDOCUMENT_H

#include <QDebug>
#include <QWidget>
#include <string>

// Document class -- init, close, isActive, newView
// Design Doc -- Must have >=1 MDI, closes with last MDI, can show in Dock

class Document {
public:
    virtual ~Document() {}
    virtual void init() = 0;
    virtual void done() = 0;
    virtual bool isActive() = 0;
    virtual bool supportsUserType(const std::string &) const = 0;
    virtual QWidget *newView(const std::string &) const = 0;
    virtual const std::string & name() const = 0;
};



#endif
