#ifndef KBGEOLINE_HH
#define KBGEOLINE_HH

#include "TObject.h"
#include "TVector3.h"
#include "TLine.h"

class KBGeoLine : public TObject
{
  public:
    KBGeoLine();
    KBGeoLine(Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2);
    KBGeoLine(TVector3 pos1, TVector3 pos2);
    virtual ~KBGeoLine() {}

    void SetLine(Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2);

    Double_t GetX1();
    Double_t GetY1();
    Double_t GetZ1();
    Double_t GetX2();
    Double_t GetY2();
    Double_t GetZ2();

    TVector3 GetPoint1();
    TVector3 GetPoint2();

    Double_t Length();

    void ClosestPointOnLine(Double_t x, Double_t y, Double_t z, Double_t &x0, Double_t &y0, Double_t &z0);
    void ClosestPointOnLine(TVector3 pos, TVector3 &pos0);

    Double_t DistanceToLine(Double_t x, Double_t y, Double_t z);
    Double_t DistanceToLine(TVector3 pos);

    KBGeoLine *CreateGeoLineToPoint(Double_t x, Double_t y, Double_t z);
    KBGeoLine *CreateGeoLineToPoint(TVector3 pos);

    TLine *CreateTLineXY();
    TLine *CreateTLineYZ();
    TLine *CreateTLineZX();

  private:
    Double_t fX1 = 0;
    Double_t fY1 = 0;
    Double_t fZ1 = 0;

    Double_t fX2 = 0;
    Double_t fY2 = 0;
    Double_t fZ2 = 0;


  ClassDef(KBGeoLine, 1)
};

#endif
