#ifndef KBWPOINT_HH
#define KBWPOINT_HH

#include "TObject.h"
#include "TError.h"

/// position data with weight

class KBWPoint : public TObject
{
  public:
    KBWPoint();
    KBWPoint(Double_t x, Double_t y, Double_t z, Double_t w = 1);

    virtual void Print(Option_t *option = "") const;
    virtual void Clear(Option_t *option = "");

    void Set(Double_t x, Double_t y, Double_t z, Double_t w = 1);
    void SetAt(Double_t val, Int_t i);

    Double_t x() { return fX; }
    Double_t y() { return fY; }
    Double_t z() { return fZ; }
    Double_t w() { return fW; }

    Double_t X() { return fX; }
    Double_t Y() { return fY; }
    Double_t Z() { return fZ; }
    Double_t W() { return fW; }

    inline Double_t operator[](int i) const {
      switch(i) {
        case 0:
          return fX;
        case 1:
          return fY;
        case 2:
          return fZ;
        default:
          Error("operator[](i)", "bad index (%d) returning 0",i);
      }
      return 0.;
    }

  protected:
    Double_t fX;
    Double_t fY;
    Double_t fZ;
    Double_t fW;

  ClassDef(KBWPoint, 1)
};

#endif
