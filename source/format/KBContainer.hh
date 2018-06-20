#ifndef KBCONTAINER_HH
#define KBCONTAINER_HH

#include "TObject.h"
#ifdef ACTIVATE_EVE
#include "TEveElement.h"
#endif

class KBContainer : public TObject
{
  public:
    KBContainer();
    virtual ~KBContainer();

#ifdef ACTIVATE_EVE
    virtual bool DrawByDefault()              { return false; } ///< return true if to be displayed on eve.
    virtual bool IsEveSet()                   { return false; } ///< Check if this element should be a "set" of TEveElements (e.g. TEvePointSet, TEveStraightLineSet)
    virtual TEveElement *CreateEveElement()   { return nullptr; } ///< Create TEveElement
    virtual void SetEveElement(TEveElement *) {} ///< Set TEveElement. For when IsEveSet() is false.
    virtual void AddToEveSet(TEveElement *)   {} ///< Add TEveElement to this eve-set
#endif

  ClassDef(KBContainer, 1)
};

#endif
