#include "KBWPoint.hh"
#include <iostream>
#include <iomanip>
using namespace std;

ClassImp(KBWPoint)

KBWPoint::KBWPoint()
{
  Clear();
}

KBWPoint::KBWPoint(Double_t x, Double_t y, Double_t z, Double_t w)
{
  Set(x,y,z,w);
}

void KBWPoint::Set(Double_t x, Double_t y, Double_t z, Double_t w)
{
  fX = x;
  fY = y;
  fZ = z;
  fW = w;
}

void KBWPoint::Print(Option_t *) const
{
  cout << "XYZ|W: "
       << setw(12) << fX 
       << setw(12) << fY
       << setw(12) << fZ << " |"
       << setw(12) << fW << endl;
}

void KBWPoint::Clear(Option_t *)
{
  fX = 0;
  fY = 0;
  fZ = 0;
  fW = 0;
}
