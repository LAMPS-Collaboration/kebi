#include "KBGeoBox.hh"
#include <cmath>

ClassImp(KBGeoBox)

KBGeoBox::KBGeoBox()
{
}

KBGeoBox::KBGeoBox(Double_t xc, Double_t yc, Double_t zc, Double_t dx, Double_t dy, Double_t dz)
{
  SetBox(xc,yc,zc,dx,dy,dz);
}

KBGeoBox::KBGeoBox(TVector3 center, Double_t dx, Double_t dy, Double_t dz)
{
  SetBox(center,dx,dy,dz);
}

void KBGeoBox::SetBox(Double_t xc, Double_t yc, Double_t zc, Double_t dx, Double_t dy, Double_t dz)
{
  fCenter.SetXYZ(xc,yc,zc);
  fdX = dx;
  fdY = dy;
  fdZ = dz;
}

void KBGeoBox::SetBox(TVector3 center, Double_t dx, Double_t dy, Double_t dz)
{
  fCenter = center;
  fdX = dx;
  fdY = dy;
  fdZ = dz;
}

TVector3 KBGeoBox::GetCenter() const
{
  return fCenter;
}

Double_t KBGeoBox::GetdX() const { return fdX; }
Double_t KBGeoBox::GetdY() const { return fdY; }
Double_t KBGeoBox::GetdZ() const { return fdZ; }

TVector3 KBGeoBox::GetCorner(Int_t idx) const
{
  Int_t xpm[] = {1, 1, 1, 1, -1,-1,-1,-1};
  Int_t ypm[] = {1, 1,-1,-1, -1,-1, 1, 1};
  Int_t zpm[] = {1,-1,-1, 1,  1,-1,-1, 1};

  return TVector3(xpm[idx]*fdX+fCenter.X(), ypm[idx]*fdY+fCenter.Y(), zpm[idx]*fdZ+fCenter.Z());
}

KBGeoLine KBGeoBox::GetEdge(Int_t idx) const
{
  Int_t c1[] = {0,1,2,3, 3,2,1,0, 4,5,6,7};
  Int_t c2[] = {1,2,3,0, 4,5,6,7, 5,6,7,4};
  return KBGeoLine(GetCorner(c1[idx]),GetCorner(c2[idx]));
}

KBGeoClosedPlane KBGeoBox::GetFace(Int_t idx) const
{
  Int_t c1[] = {0, 0, 1, 2, 3, 4};
  Int_t c2[] = {1, 1, 2, 3, 0, 5};
  Int_t c3[] = {2, 6, 5, 4, 7, 6};
  Int_t c4[] = {3, 7, 6, 5, 4, 7};
  return KBGeoClosedPlane(GetCorner(c1[idx]),GetCorner(c2[idx]),
                          GetCorner(c3[idx]),GetCorner(c4[idx]));
}
