#ifndef VDOCUMENT_H
#define VDOCUMENT_H

#include <QDebug>
#include <QWidget>
#include <string>

// Document class -- init, close, isActive, newView
// Design Doc -- Must have >=1 MDI, closes with last MDI, can show in Dock
// Project Doc -- Can show in MDI or Dock, does not close with MDI
// Setting -- Can show in MDI or Dock, closes with last frame
// Wizard -- shows in exactly one MDI or Dock, closes with frame

// Behavior can be controlled by Document having some property class
// DocBehavior::MDI = [PROHIBITTED, OPTIONAL, REQUIRED, EXCLUSIVE]
// DocBehavior::DOCK = [PROHIBITTED, OPTIONAL, REQUIRED, EXCLUSIVE]
// DocBehavior::CLOSING = [NONE, LAST_MDI, LAST_DOCK, BOTH] -- should there be EITHER too?

// Design doc would be REQUIRED, OPTIONAL, LAST_MDI
// Project doc would be OPTIONAL, OPTIONAL, NONE.  A new project doc without MDI or Dock would be fine
// Setting would be OPTIONAL, OPTIONAL, BOTH.  A new setting without MDI or Dock would close immediately
// Wizard would be EXCLUSIVE, EXCLUSIVE, BOTH

// So then instantiations of VDocument would probably have these properties
// built into the type, rather than having these properties added to them
// dynamically.

enum class MdiBehavior {
    OPTIONAL,   // System allows MDI for this doc (eg project doc)
    REQUIRED,   // Error if system detects no MDI for this doc (eg typical doc)
    PROHIBITED, // Error if system detects MDI attempt for this doc
    EXCLUSIVE}; // Exactly one of MDI or Dock required for this doc (eg wizard)

enum class DockBehavior {
    OPTIONAL,   // System allows MDI for this doc (eg project doc)
    REQUIRED,   // Error if system detects no MDI for this doc (eg typical doc)
    PROHIBITED, // Error if system detects MDI attempt for this doc
    EXCLUSIVE}; // Exactly one of MDI or Dock required for this doc (eg wizard)

enum class CloseBehavior {
    LAST_MDI,  // doc is done() when last of its MDI is closed
    LAST_DOCK, // doc is done() when last of its Dock is closed
    BOTH,      // doc is done() when both its last MDI and its last Dock is closed
    NONE};     // doc is not done() based on frames, but for some other reason

class Document {
public:
    virtual ~Document() {}
    virtual void init() = 0;
    virtual void done() = 0;
    virtual bool isActive() = 0;
    virtual QWidget *newView(const std::string & userType) const = 0;
    virtual const std::string & name() const = 0;
    virtual MdiBehavior mdiBehavior() const = 0;
    virtual DockBehavior dockBehavior() const = 0;
    virtual CloseBehavior closeBehavior() const = 0;
};



#endif
