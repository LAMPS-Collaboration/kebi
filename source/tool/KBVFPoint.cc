/**
 * Vector Field Point
*/

#include "KBVFPoint.hh"
#include <cmath>

ClassImp(KBVFPoint)

Int_t KBVFPoint::NeighborPointID(Int_t ix, Int_t iy) 
{
  if (iy == -1) {
         if (ix ==-1) return 6;
    else if (ix == 0) return 3;
    else if (ix == 1) return 7;
  } else if (iy == 0) {
         if (ix ==-1) return 2;
    else if (ix == 1) return 0;
  } else if (iy == 1) {
         if (ix ==-1) return 5;
    else if (ix == 0) return 1;
    else if (ix == 1) return 4;
  }
  return -1;
}

void KBVFPoint::Initialize()
{
  if (!fIsActive)
    return;

  for (auto id = 0; id < 4; id++)
    fVNeighbor[id] = fNeighborPoint[id] -> GetValue() - fValue;

  for (auto id = 4; id < 8; id++)
    fVNeighbor[id] = (fNeighborPoint[id] -> GetValue() - fValue);

  if (fVNeighbor[0] < 0 && fVNeighbor[1] < 0 && fVNeighbor[2] < 0 && fVNeighbor[3] < 0) {
    fVX = 0;
    fVY = 0;
  } else {
    auto sq2 = std::sqrt(2); 
    fVX = fVNeighbor[0] - fVNeighbor[2] + fVNeighbor[4]/sq2 - fVNeighbor[5]/sq2 - fVNeighbor[6]/sq2 + fVNeighbor[7]/sq2;
    fVY = fVNeighbor[1] - fVNeighbor[3] + fVNeighbor[4]/sq2 + fVNeighbor[5]/sq2 - fVNeighbor[6]/sq2 - fVNeighbor[7]/sq2;
  }
}

void KBVFPoint::SetIsActive(bool active) { fIsActive = active; }
void KBVFPoint::SetValue(Double_t v) { fValue = v; }
void KBVFPoint::SetNeighborPoint(Int_t id, KBVFPoint *point) { fNeighborPoint[id] = point; }

bool KBVFPoint::IsActive() const { return fIsActive; }
Double_t KBVFPoint::GetValue() const { return fValue; }
KBVFPoint *KBVFPoint::GetNeighborPoint(Int_t id) const { return fNeighborPoint[id]; }
Double_t KBVFPoint::GetVNeibor(Int_t id) const { return fVNeighbor[id]; }

Double_t KBVFPoint::GetVX() { return fVX; }
Double_t KBVFPoint::GetVY() { return fVY; }
