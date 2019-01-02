#ifndef KBGEOCLOSEDPLANE_HH
#define KBGEOCLOSEDPLANE_HH

#include "TVector3.h"
#include "KBGeoPlane.hh"
#include "KBGeometry.hh"

class KBGeoClosedPlane : public KBGeoPlane
{
  public:
    KBGeoClosedPlane();
    KBGeoClosedPlane(TVector3 c1, TVector3 c2, TVector3 c3, TVector3 c4);
    virtual ~KBGeoClosedPlane() {}

    void SetClosedPlane(TVector3 c1, TVector3 c2, TVector3 c3, TVector3 c4);

    //virtual TVector3 ClosestPointOnPlane(TVector3 pos) const;
    //virtual Double_t DistanceToPlane(Double_t x, Double_t y, Double_t z) const;
    //virtual Double_t DistanceToPlane(TVector3 pos) const;

    TVector3 GetCorner(Int_t idx) const;

  protected:
    TVector3 fCorner[4];

  ClassDef(KBGeoClosedPlane, 1)
};

#endif
