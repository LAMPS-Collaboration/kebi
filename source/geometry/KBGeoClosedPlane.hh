#ifndef KBGEOCLOSEDPLANE_HH
#define KBGEOCLOSEDPLANE_HH

#include "TVector3.h"
#include "TGraph.h"

#include "KBVector3.hh"
#include "KBGeoPlane.hh"
#include "KBGeometry.hh"

typedef KBVector3::Axis kbaxis_t;

class KBGeoClosedPlane : public KBGeoPlane
{
  public:
    KBGeoClosedPlane();
    KBGeoClosedPlane(TVector3 c1, TVector3 c2, TVector3 c3, TVector3 c4);
    virtual ~KBGeoClosedPlane() {}

    virtual void Print(Option_t *option = "") const;

    void SetClosedPlane(TVector3 c0, TVector3 c1, TVector3 c2, TVector3 c3);

    //virtual TVector3 ClosestPointOnPlane(TVector3 pos) const;
    //virtual Double_t DistanceToPlane(Double_t x, Double_t y, Double_t z) const;
    //virtual Double_t DistanceToPlane(TVector3 pos) const;

    TVector3 GetCorner(Int_t idx) const;

    TGraph *Draw(kbaxis_t axis1 = KBVector3::kX, kbaxis_t axis2 = KBVector3::kY);

  protected:
    KBVector3 fCorner[4];

  ClassDef(KBGeoClosedPlane, 1)
};

#endif
