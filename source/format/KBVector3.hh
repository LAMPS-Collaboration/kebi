#ifndef KBVECTOR3_HH
#define KBVECTOR3_HH

#include "TVector3.h"

class KBVector3 : public TVector3
{
  public:
    KBVector3()
    :TVector3() { Clear(); }

    KBVector3(Short_t referenceAxis)
    :TVector3(), fReferenceAxis(referenceAxis) {}

    KBVector3(const KBVector3 &p)
    :TVector3(p), fReferenceAxis(p.fReferenceAxis) {}

    KBVector3(const TVector3 &p, Short_t referenceAxis = 2)
    :TVector3(p), fReferenceAxis(referenceAxis) {}

    KBVector3(Double_t x0, Double_t y0, Double_t z0, Short_t referenceAxis = 2)
    :TVector3(x0,y0,z0), fReferenceAxis(referenceAxis) {}

    virtual ~KBVector3() {}

    virtual void Print(Option_t *option = "") const;
    virtual void Clear(Option_t *option = "");

    void SetReferenceAxis(Short_t referenceAxis);
    Short_t GetReferenceAxis() const;

    Double_t AtXYZ(Short_t axis);
    Double_t AtIJK(Short_t axis);

    void AddAtXYZ(Double_t value, Short_t axis);
    void AddAtIJK(Double_t value, Short_t axis);

    void SetIJKR(Double_t i, Double_t j, Double_t k, Short_t referenceAxis);
    void SetIJK(Double_t i, Double_t j, Double_t k);
    void SetI(Double_t i);
    void SetJ(Double_t j);
    void SetK(Double_t k);

    Double_t I() const;
    Double_t J() const;
    Double_t K() const;

    TVector3 GetV3IJK();

  private:
    Short_t fReferenceAxis = 2; ///< 0(x), 1(y,) 2(z).

  ClassDef(KBVector3, 1)
};

#endif
