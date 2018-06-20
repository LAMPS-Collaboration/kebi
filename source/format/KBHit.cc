#include "KBHit.hh"
#include "KBPulseGenerator.hh"
#ifdef ACTIVATE_EVE
#include "TEvePointSet.h"
#endif
#include <iostream>
#include <iomanip>

ClassImp(KBHit)

void KBHit::Clear(Option_t *)
{
  KBWPointCluster::Clear();

  fHitID = -1;
  fTrackID = -1;

  fTrackCandArray.clear();
}

void KBHit::Print(Option_t *option) const
{
  cout << "ID|XYZ|Charge: "
    << setw(4)  << fHitID << " |"
    << setw(4)  << fHitID << " |"
    << setw(12) << fX
    << setw(12) << fY
    << setw(12) << fZ << " |"
    << setw(12) << fW << endl;
}

void KBHit::Copy(TObject &obj) const
{
  KBWPointCluster::Copy(obj);
  auto hit = (KBHit &) obj;

  hit.SetHitID(fHitID);
  hit.SetTrackID(fTrackID);
}

void KBHit::SetHitID(Int_t id) { fHitID = id; }
void KBHit::SetTrackID(Int_t id) { fTrackID = id; }
void KBHit::SetX(Double_t x) { fX = x; }
void KBHit::SetY(Double_t y) { fY = y; }
void KBHit::SetZ(Double_t z) { fZ = z; }
void KBHit::SetDX(Double_t dx) { fCov[0][0] = dx * dx; }
void KBHit::SetDY(Double_t dy) { fCov[1][1] = dy * dy; }
void KBHit::SetDZ(Double_t dz) { fCov[2][2] = dz * dz; }
void KBHit::SetCharge(Double_t charge) { fW = charge; }

void KBHit::AddHit(KBHit *hit)
{
  KBWPointCluster::Add((KBWPoint &) hit);
}

Int_t KBHit::GetHitID()   const { return fHitID; }
Int_t KBHit::GetTrackID() const { return fTrackID; }
Double_t KBHit::GetX()  const { return fX; }
Double_t KBHit::GetY()  const { return fY; }
Double_t KBHit::GetZ()  const { return fZ; }
Double_t KBHit::GetDX() const { return sqrt(fCov[0][0]); }
Double_t KBHit::GetDY() const { return sqrt(fCov[1][1]); }
Double_t KBHit::GetDZ() const { return sqrt(fCov[2][2]); }
Double_t KBHit::GetCharge() const { return fW; }

std::vector<Int_t> *KBHit::GetTrackCandArray() { return &fTrackCandArray; }
Int_t KBHit::GetNumTrackCands() { return fTrackCandArray.size(); }
void KBHit::AddTrackCand(Int_t id) { fTrackCandArray.push_back(id); }

void KBHit::RemoveTrackCand(Int_t trackID)
{
  Int_t n = fTrackCandArray.size();
  for (auto i = 0; i < n; i++) {
    if (fTrackCandArray[i] == trackID) {
      fTrackCandArray.erase(fTrackCandArray.begin()+i); 
      return;
    }
  }
  fTrackCandArray.push_back(-1);
}

#ifdef ACTIVATE_EVE
bool KBHit::DrawByDefault() { return true; }
bool KBHit::IsEveSet() { return true; }

TEveElement *KBHit::CreateEveElement()
{
  auto pointSet = new TEvePointSet("Hit");
  pointSet -> SetMarkerColor(kAzure-8);
  pointSet -> SetMarkerSize(0.4);
  pointSet -> SetMarkerStyle(38);

  return pointSet;
}

void KBHit::SetEveElement(TEveElement *)
{
}

void KBHit::AddToEveSet(TEveElement *eveSet)
{
  auto pointSet = (TEvePointSet *) eveSet;
  pointSet -> SetNextPoint(fX, fY, fZ);
}
#endif
