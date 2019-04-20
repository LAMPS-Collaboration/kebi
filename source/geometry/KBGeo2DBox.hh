#ifndef KBGEO2DBOX_HH
#define KBGEO2DBOX_HH

#include "TVector3.h"
#include "TGraph.h"

#include "KBVector3.hh"
#include "KBGeoRotated.hh"
#include "KBGeoLine.hh"

typedef KBVector3::Axis kbaxis_t;

class KBGeo2DBox : public KBGeoRotated
{
  public:
    KBGeo2DBox();
    KBGeo2DBox(Double_t x1, Double_t x2, Double_t y1, Double_t y2);
    virtual ~KBGeo2DBox() {}

    virtual void Print(Option_t *option = "") const;

    virtual TVector3 GetCenter() const;

    void Set2DBox(Double_t x1, Double_t x2, Double_t y1, Double_t y2);

    Double_t GetXCenter() const;
    Double_t GetdX() const;
    Double_t GetX1() const;
    Double_t GetX2() const;

    Double_t GetYCenter() const;
    Double_t GetdY() const;
    Double_t GetY1() const;
    Double_t GetY2() const;

    TVector3 GetCorner(Int_t idx) const;
    TVector3 GetCorner(Int_t xpm, Int_t ypm) const; ///< pm should be 1(high) or -1(low)

    KBGeoLine GetEdge(Int_t idx) const;
    KBGeoLine GetEdge(Int_t xpm, Int_t ypm) const; ///< pm of edge axis is 0 while the other should be 1(high) or -1(low)

    TGraph *DrawGraph();

    bool IsInside(Double_t x, Double_t y);

  protected:
    Double_t fX1;
    Double_t fX2;
    Double_t fY1;
    Double_t fY2;

  ClassDef(KBGeo2DBox, 1)
};

#endif
