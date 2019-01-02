#ifndef KBCONTAINER_HH
#define KBCONTAINER_HH

#include "TObject.h"
#ifdef ACTIVATE_EVE
#include "TEveElement.h"
#endif

#include "KBGlobal.hh"

/**
 * Print(Option_t *option), option convension
 * - a (aligned)      : Aligned print format. [Default]
 * - s (simple)       : Simplest print format. Suppress all aligned spacings if possible.
 * - t (title)        : Print titles of contents. [Default]
 * - d (daughter)     : Print daughter objects.
 * - > (as reference) : Print as reference.
 */

class KBContainer : public TObject
{
  public:
    KBContainer();
    virtual ~KBContainer();

    virtual void Clear(Option_t *option = "");
    virtual void Copy (TObject &object) const;

#ifdef ACTIVATE_EVE
    virtual bool DrawByDefault()              { return false; } ///< return true if to be displayed on eve.
    virtual bool IsEveSet()                   { return false; } ///< Check if this element should be a "set" of TEveElements (e.g. TEvePointSet, TEveStraightLineSet)
    virtual TEveElement *CreateEveElement()   { return nullptr; } ///< Create TEveElement
    virtual void SetEveElement(TEveElement *) {} ///< Set TEveElement. For when IsEveSet() is false.
    virtual void AddToEveSet(TEveElement *)   {} ///< Add TEveElement to this eve-set
#endif

  ClassDef(KBContainer, 3)
};

#define kc_raw     KBLog(this->ClassName(),__FUNCTION__,0,-1)
#define kc_out     KBLog(this->ClassName(),__FUNCTION__,0,0)
#define kc_print   KBLog(this->ClassName(),__FUNCTION__,0,1)
#define kc_info    KBLog(this->ClassName(),__FUNCTION__,0,2)
#define kc_warning KBLog(this->ClassName(),__FUNCTION__,0,3)
#define kc_error   KBLog(this->ClassName(),__FUNCTION__,0,4)
#define kc_debug   KBLog(__FILE__,__LINE__)

#endif
