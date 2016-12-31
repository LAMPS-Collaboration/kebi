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
  SetLine(pos1.X(), pos1.Y(), pos1.Z(), pos2.X(), pos2.Y(), pos2.Z());
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

Double_t KBGeoLine::GetX1() { return fX1; }
Double_t KBGeoLine::GetY1() { return fY1; }
Double_t KBGeoLine::GetZ1() { return fZ1; }
Double_t KBGeoLine::GetX2() { return fX2; }
Double_t KBGeoLine::GetY2() { return fY2; }
Double_t KBGeoLine::GetZ2() { return fZ2; }

TVector3 KBGeoLine::GetPoint1() { return TVector3(fX1, fY1, fZ1); }
TVector3 KBGeoLine::GetPoint2() { return TVector3(fX2, fY2, fZ2); }

Double_t KBGeoLine::Length() { return std::sqrt((fX1-fX2)*(fX1-fX2) + (fY1-fY2)*(fY1-fY2) + (fZ1-fZ2)*(fZ1-fZ2)); }

void KBGeoLine::ClosestPointOnLine(Double_t x, Double_t y, Double_t z, Double_t &x0, Double_t &y0, Double_t &z0)
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

  //x0 = fX1 + l*norm*xv;
  //y0 = fY1 + l*norm*yv;
  //z0 = fZ1 + l*norm*zv;
  x0 = fX1 + l*xv;
  y0 = fY1 + l*yv;
  z0 = fZ1 + l*zv;
}

void KBGeoLine::ClosestPointOnLine(TVector3 pos, TVector3 &pos0)
{
  Double_t x0 = 0, y0 = 0, z0 = 0;

  ClosestPointOnLine(pos.X(), pos.Y(), pos.Z(), x0, y0, z0);

  pos0.SetXYZ(x0, y0, z0);
}

Double_t KBGeoLine::DistanceToLine(Double_t x, Double_t y, Double_t z)
{
  Double_t x0 = 0, y0 = 0, z0 = 0;

  ClosestPointOnLine(x, y, z, x0, y0, z0);

  return std::sqrt((x-x0)*(x-x0) + (y-y0)*(y-y0) + (z-z0)*(z-z0));
}

Double_t KBGeoLine::DistanceToLine(TVector3 pos)
{
  Double_t x0 = 0, y0 = 0, z0 = 0;

  ClosestPointOnLine(pos.X(), pos.Y(), pos.Z(), x0, y0, z0);

  return std::sqrt((pos.X()-x0)*(pos.X()-x0) + (pos.Y()-y0)*(pos.Y()-y0) + (pos.Z()-z0)*(pos.Z()-z0));
}

KBGeoLine *KBGeoLine::CreateGeoLineToPoint(Double_t x, Double_t y, Double_t z)
{
  Double_t x0 = 0, y0 = 0, z0 = 0;

  ClosestPointOnLine(x, y, z, x0, y0, z0);

  return new KBGeoLine(x, y, z, x0, y0, z0);
}

KBGeoLine *KBGeoLine::CreateGeoLineToPoint(TVector3 pos)
{
  return CreateGeoLineToPoint(pos.X(), pos.Y(), pos.Z());
}

TLine *KBGeoLine::CreateTLineXY() { return new TLine(fX1, fY1, fX2, fY2); }
TLine *KBGeoLine::CreateTLineYZ() { return new TLine(fY1, fZ1, fY2, fZ2); }
TLine *KBGeoLine::CreateTLineZX() { return new TLine(fZ1, fX1, fZ2, fX2); }
