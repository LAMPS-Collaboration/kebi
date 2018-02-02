#ifndef KBCONTAINER_HH
#define KBCONTAINER_HH

#include "TObject.h"
#include "TEveElement.h"

class KBContainer : public TObject
{
  public:
    KBContainer();
    virtual ~KBContainer();

#ifdef ACTIVATE_EVE
    virtual bool DrawByDefault() = 0;
    virtual bool IsEveSet() = 0; ///< Check if this element should be a "set" of TEveElements (e.g. TEvePointSet, TEveStraightLineSet)
    virtual TEveElement *CreateEveElement() = 0; ///< Create TEveElement
    virtual void SetEveElement(TEveElement *element) = 0; ///< Set TEveElement. For when IsEveSet() is false.
    virtual void AddToEveSet(TEveElement *eveSet) = 0; ///< Add TEveElement to this eve-set
#endif

  ClassDef(KBContainer, 1)
};

#endif
