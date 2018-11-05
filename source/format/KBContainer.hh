#ifndef KBCONTAINER_HH
#define KBCONTAINER_HH

#include "TObject.h"
#ifdef ACTIVATE_EVE
#include "TEveElement.h"
#endif

class KBContainer : public TObject
{
  protected:
    Int_t fMCID = -1;
    Double_t fMCError = -1;
    Double_t fMCPurity = -1;

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

    virtual void SetMCID(Int_t id, Double_t error = 0, Double_t purity = 0) { fMCID = id; fMCError = error; fMCPurity = purity; }
    Int_t GetMCID() const { return fMCID; }
    Int_t GetMCError() const { return fMCError; } /// mm

    virtual void PropagateMC() {};

  ClassDef(KBContainer, 2)
};

#endif
