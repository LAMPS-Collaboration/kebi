#include "KBHit.hh"

#include "TEvePointSet.h"

#include <iostream>

ClassImp(KBHit)

void KBHit::Clear(Option_t *)
{
  fHitID = -1;
  fPadID = -1;
  fTrackID = -1;

  fX = -999;
  fY = -999;
  fZ = -999;
  fDX = -999;
  fDY = -999;
  fDZ = -999;

  fSection = -999;
  fRow = -999;
  fLayer = -999;

  fTb = -1;
  fCharge = 0;
}

void KBHit::Print(Option_t *option) const
{
  if (TString(option) == "s") {
    cout << "[KBHit] "
         << setw(6) << fHitID
         << setw(4) << fRow
         << setw(4) << fLayer
         << setw(12) << fTb
         << setw(12) << fCharge << endl;
    return;
  }

  cout << "[KBHit]" << endl;
  cout << "  Hit-ID           : " << fHitID << endl;
  cout << "  Pad-ID           : " << fPadID << endl;
  cout << "  Track-ID         : " << fTrackID << endl;
  cout << "  Position         : (" << fX << ", " << fY << ", " << fZ << ") " << endl;
  cout << "  (tb, Charge)     : (" << fTb << ", " << fCharge << ")" << endl;
}

void KBHit::SetHitID(Int_t id) { fHitID = id; }
void KBHit::SetPadID(Int_t id) { fPadID = id; }
void KBHit::SetTrackID(Int_t id) { fTrackID = id; }
void KBHit::SetX(Double_t x) { fX = x; }
void KBHit::SetY(Double_t y) { fY = y; }
void KBHit::SetZ(Double_t z) { fZ = z; }
void KBHit::SetDX(Double_t dx) { fDX = dx; }
void KBHit::SetDY(Double_t dy) { fDY = dy; }
void KBHit::SetDZ(Double_t dz) { fDZ = dz; }
void KBHit::SetSection(Int_t section) { fSection = section; }
void KBHit::SetRow(Int_t row) { fRow = row; }
void KBHit::SetLayer(Int_t layer) { fLayer = layer; }
void KBHit::SetTb(Double_t tb) { fTb = tb; }
void KBHit::SetCharge(Double_t charge) { fCharge = charge; }

void KBHit::AddHit(KBHit *hit)
{
  auto charge = hit -> GetCharge();
  fX = (fCharge*fX + charge*hit->GetX()) / (fCharge + charge);
  fY = (fCharge*fY + charge*hit->GetY()) / (fCharge + charge);
  fZ = (fCharge*fZ + charge*hit->GetZ()) / (fCharge + charge);
  fCharge += charge;
}

Int_t KBHit::GetHitID() const { return fHitID; }
Int_t KBHit::GetPadID() const { return fPadID; }
Int_t KBHit::GetTrackID() const { return fTrackID; }
Double_t KBHit::GetX() const { return fX; }
Double_t KBHit::GetY() const { return fY; }
Double_t KBHit::GetZ() const { return fZ; }
TVector3 KBHit::GetPosition() const { return TVector3(fX, fY, fZ); }
Double_t KBHit::GetDX() const { return fDX; }
Double_t KBHit::GetDY() const { return fDY; }
Double_t KBHit::GetDZ() const { return fDZ; }
Int_t KBHit::GetSection() const { return fSection; }
Int_t KBHit::GetRow() const { return fRow; }
Int_t KBHit::GetLayer() const { return fLayer; }
TVector3 KBHit::GetPosSigma() const { return TVector3(fDX, fDY, fDZ); }
Double_t KBHit::GetTb() const { return fTb; }
Double_t KBHit::GetCharge() const { return fCharge; }

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

void KBHit::Change()
{
  Double_t x = fX;
  fX = fY;
  fY = fZ;
  fZ = x;
}

void KBHit::ChangeBack()
{
  Double_t x = fX;
  fX = fZ;
  fZ = fY;
  fY = x;
}



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
  //pointSet -> SetNextPoint(fZ, fX, fY);
  pointSet -> SetNextPoint(fX, fY, fZ);
}
