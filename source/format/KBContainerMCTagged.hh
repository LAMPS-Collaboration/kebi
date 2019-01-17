#ifndef KBCONTAINERMCTAGGED_HH
#define KBCONTAINERMCTAGGED_HH

#include "KBContainer.hh"

class KBContainerMCTagged : public KBContainer
{
  protected:
    Int_t fMCID = -1;
    Double_t fMCError = -1; //!
    Double_t fMCPurity = -1; //!

  public:
    KBContainerMCTagged();
    virtual ~KBContainerMCTagged();

    virtual void Clear(Option_t *option = "");
    virtual void Copy (TObject &object) const;

    virtual void SetMCID(Int_t id, Double_t error = 0, Double_t purity = 0) { fMCID = id; fMCError = error; fMCPurity = purity; }
    Int_t GetMCID() const { return fMCID; }
    Double_t GetMCError() const { return fMCError; } /// mm
    Double_t GetMCPurity() const { return fMCPurity; } /// mm

    virtual void PropagateMC() {};

  ClassDef(KBContainerMCTagged, 1)
};

#endif
