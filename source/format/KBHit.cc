#include "KBHit.hh"

#include <iostream>

ClassImp(KBHit)

void KBHit::Print(Option_t *option) const
{
  cout << "[KBHit]" << endl;
  cout << "  Hit/Pad/Track-ID: " << fHitID << " / " << fPadID << " / " << fTrackID << endl;
  cout << "  Position: (" << fX << ", " << fY << ", " << fZ << ") " << endl;
  cout << "  Charge: " << fCharge << endl;
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
void KBHit::SetCharge(Double_t charge) { fCharge = charge; }

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
TVector3 KBHit::GetPosSigma() const { return TVector3(fDX, fDY, fDZ); }
Double_t KBHit::GetCharge() const { return fCharge; }

std::vector<Int_t> *KBHit::GetTrackCandArray() { return &fTrackCandArray; }
Int_t KBHit::GetNTrackCands() { return fTrackCandArray.size(); }
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
