#ifndef KBVFPOINT_HH
#define KBVFPOINT_HH

/**
 * Vector Field Point
*/

#include "TObject.h"

class KBVFPoint : public TObject
{
  public:
    KBVFPoint() {};
    virtual ~KBVFPoint() {};

    void Initialize();

    Int_t NeighborPointID(Int_t ix, Int_t iy);

    void SetIsActive(bool active);
    void SetValue(Double_t v);
    void SetNeighborPoint(Int_t id, KBVFPoint *point);

    bool IsActive() const;
    Double_t GetValue() const;
    KBVFPoint *GetNeighborPoint(Int_t id) const;
    Double_t GetVNeibor(Int_t id) const;

    Double_t GetVX();
    Double_t GetVY();

  private:
    bool fIsActive = false;
    Double_t fValue = 0;
    KBVFPoint *fNeighborPoint[8]; //! <
    Double_t fVNeighbor[8];

    Double_t fVX;
    Double_t fVY;


  ClassDef(KBVFPoint, 1)
};

#endif
