#ifndef KBVECTOR3_HH
#define KBVECTOR3_HH

#include "TVector3.h"

class KBVector3 : public TVector3
{
  public:
    enum Axis { kNon=0, kX=1, kY=2, kZ=3, kMX=4, kMY=5, kMZ=6, kI=7, kJ=8, kK=9 }; 

    KBVector3()
    :TVector3() { Clear(); }

    KBVector3(KBVector3::Axis referenceAxis)
    :TVector3(), fReferenceAxis(referenceAxis) {}

    KBVector3(const KBVector3 &p)
    :TVector3(p), fReferenceAxis(p.fReferenceAxis) {}

    KBVector3(const TVector3 &p, KBVector3::Axis referenceAxis)
    :TVector3(p), fReferenceAxis(referenceAxis) {}

    KBVector3(Double_t x0, Double_t y0, Double_t z0, KBVector3::Axis referenceAxis)
    :TVector3(x0,y0,z0), fReferenceAxis(referenceAxis) {}

    virtual ~KBVector3() {}

    virtual void Print(Option_t *option = "") const;
    virtual void Clear(Option_t *option = "");

    void SetReferenceAxis(KBVector3::Axis referenceAxis);
    KBVector3::Axis GetReferenceAxis() const;

    Double_t At(KBVector3::Axis axis) const;
    void AddAt(Double_t value, KBVector3::Axis axis);

    void SetIJKR(Double_t i, Double_t j, Double_t k, KBVector3::Axis referenceAxis);
    void SetIJK(Double_t i, Double_t j, Double_t k);
    void SetI(Double_t i);
    void SetJ(Double_t j);
    void SetK(Double_t k);

    Double_t I() const;
    Double_t J() const;
    Double_t K() const;

    TVector3 GetV3IJK();

  private:
    KBVector3::Axis fReferenceAxis = KBVector3::kZ;

  ClassDef(KBVector3, 1)
};

#endif
