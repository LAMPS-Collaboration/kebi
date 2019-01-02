#ifndef KBGEOBOX_HH
#define KBGEOBOX_HH

#include "TVector3.h"
#include "KBGeometry.hh"
#include "KBGeoLine.hh"
#include "KBGeoClosedPlane.hh"

class KBGeoBox : public KBGeometry
{
  public:
    KBGeoBox();
    KBGeoBox(Double_t xc, Double_t yc, Double_t zc, Double_t dx, Double_t dy, Double_t dz);
    KBGeoBox(TVector3 center, Double_t dx, Double_t dy, Double_t dz);
    virtual ~KBGeoBox() {}

    void SetBox(Double_t xc, Double_t yc, Double_t zc, Double_t dx, Double_t dy, Double_t dz);
    void SetBox(TVector3 center, Double_t dx, Double_t dy, Double_t dz);

    TVector3 GetCenter() const;
    Double_t GetdX() const;
    Double_t GetdY() const;
    Double_t GetdZ() const;

    TVector3 GetCorner(Int_t idx) const;
    KBGeoLine GetEdge(Int_t idx) const;
    KBGeoClosedPlane GetFace(Int_t idx) const;

  protected:
    TVector3 fCenter;
    Double_t fdX;
    Double_t fdY;
    Double_t fdZ;

  ClassDef(KBGeoBox, 1)
};

#endif
