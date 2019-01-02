#include "KBGeoClosedPlane.hh"
#include <cmath>

ClassImp(KBGeoClosedPlane)

KBGeoClosedPlane::KBGeoClosedPlane()
{
}

KBGeoClosedPlane::KBGeoClosedPlane(TVector3 c1, TVector3 c2, TVector3 c3, TVector3 c4)
{
  SetClosedPlane(c1,c2,c3,c4);
}

void KBGeoClosedPlane::SetClosedPlane(TVector3 c1, TVector3 c2, TVector3 c3, TVector3 c4)
{
  fCorner[0] = c1;
  fCorner[1] = c2;
  fCorner[2] = c3;
  fCorner[3] = c4;

  KBGeoPlane::SetPlane(c1,(c1-c2).Cross(c1-c3));
}

TVector3 KBGeoClosedPlane::GetCorner(Int_t idx) const
{
  return fCorner[idx];
}
