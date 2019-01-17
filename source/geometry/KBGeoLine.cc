#include "KBGeoLine.hh"
#include <cmath>

ClassImp(KBGeoLine)

KBGeoLine::KBGeoLine()
{
}

KBGeoLine::KBGeoLine(Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2)
{
  SetLine(x1, y1, z1, x2, y2, z2);
}

KBGeoLine::KBGeoLine(TVector3 pos1, TVector3 pos2)
{
  SetLine(pos1, pos2);
}

void KBGeoLine::SetLine(Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2)
{
  fX1 = x1;
  fY1 = y1;
  fZ1 = z1;
  fX2 = x2;
  fY2 = y2;
  fZ2 = z2;
}

void KBGeoLine::SetLine(TVector3 pos1, TVector3 pos2)
{
  fX1 = pos1.X();
  fY1 = pos1.Y();
  fZ1 = pos1.Z();
  fX2 = pos2.X();
  fY2 = pos2.Y();
  fZ2 = pos2.Z();
}

Double_t KBGeoLine::GetX1() const { return fX1; }
Double_t KBGeoLine::GetY1() const { return fY1; }
Double_t KBGeoLine::GetZ1() const { return fZ1; }
Double_t KBGeoLine::GetX2() const { return fX2; }
Double_t KBGeoLine::GetY2() const { return fY2; }
Double_t KBGeoLine::GetZ2() const { return fZ2; }

TVector3 KBGeoLine::GetPoint1() const { return TVector3(fX1, fY1, fZ1); }
TVector3 KBGeoLine::GetPoint2() const { return TVector3(fX2, fY2, fZ2); }

TVector3 KBGeoLine::Direction() const
{
  auto v = TVector3(fX2-fX1, fY2-fY1, fZ2-fZ1);
  return v.Unit();
}

Double_t KBGeoLine::Length(Double_t x, Double_t y, Double_t z) const
{
  auto length = std::sqrt((fX1-x)*(fX1-x) + (fY1-y)*(fY1-y) + (fZ1-z)*(fZ1-z)); 
  auto direction = TVector3(fX1-x, fY1-y, fZ1-z).Dot(TVector3(fX1-fX2, fY1-fY2, fZ1-fZ2));
  if (direction > 0)
    direction = 1;
  else
    direction = -1;

  return direction * length;
}

Double_t KBGeoLine::Length(TVector3 position) const { return Length(position.X(), position.Y(), position.Z()); }
Double_t KBGeoLine::Length() const { return std::sqrt((fX1-fX2)*(fX1-fX2) + (fY1-fY2)*(fY1-fY2) + (fZ1-fZ2)*(fZ1-fZ2)); }

void KBGeoLine::ClosestPointOnLine(Double_t x, Double_t y, Double_t z, Double_t &x0, Double_t &y0, Double_t &z0) const
{
  Double_t xv = fX2 - fX1;
  Double_t yv = fY2 - fY1;
  Double_t zv = fZ2 - fZ1;

  Double_t norm = 1./std::sqrt(xv*xv + yv*yv + zv*zv);

  xv = norm*xv;
  yv = norm*yv;
  zv = norm*zv;

  Double_t xp = x - fX1;
  Double_t yp = y - fY1;
  Double_t zp = z - fZ1;

  Double_t l = xv*xp + yv*yp + zv*zp;

  x0 = fX1 + l*xv;
  y0 = fY1 + l*yv;
  z0 = fZ1 + l*zv;
}

TVector3 KBGeoLine::ClosestPointOnLine(TVector3 pos) const
{
  Double_t x0 = 0, y0 = 0, z0 = 0;
  ClosestPointOnLine(pos.X(), pos.Y(), pos.Z(), x0, y0, z0);

  return TVector3(x0, y0, z0);
}

Double_t KBGeoLine::DistanceToLine(Double_t x, Double_t y, Double_t z) const
{
  Double_t x0 = 0, y0 = 0, z0 = 0;

  ClosestPointOnLine(x, y, z, x0, y0, z0);

  return std::sqrt((x-x0)*(x-x0) + (y-y0)*(y-y0) + (z-z0)*(z-z0));
}

Double_t KBGeoLine::DistanceToLine(TVector3 pos) const
{
  Double_t x0 = 0, y0 = 0, z0 = 0;

  ClosestPointOnLine(pos.X(), pos.Y(), pos.Z(), x0, y0, z0);

  return std::sqrt((pos.X()-x0)*(pos.X()-x0) + (pos.Y()-y0)*(pos.Y()-y0) + (pos.Z()-z0)*(pos.Z()-z0));
}

TArrow *KBGeoLine::DrawArrowXY(Double_t asize) { return new TArrow(fX1, fY1, fX2, fY2, asize); }
TArrow *KBGeoLine::DrawArrowYZ(Double_t asize) { return new TArrow(fY1, fZ1, fY2, fZ2, asize); }
TArrow *KBGeoLine::DrawArrowZY(Double_t asize) { return new TArrow(fZ1, fY1, fZ2, fY2, asize); }
TArrow *KBGeoLine::DrawArrowZX(Double_t asize) { return new TArrow(fZ1, fX1, fZ2, fX2, asize); }
TArrow *KBGeoLine::DrawArrowXZ(Double_t asize) { return new TArrow(fX1, fZ1, fX2, fZ2, asize); }
