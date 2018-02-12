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
  fHitID = -1;
  fPadID = -1;
  fTrackID = -1;

  fPosition.SetXYZ(-999,-999,-999);
  fPosSigma.SetXYZ(-999,-999,-999);

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
  cout << "  Position         : (" << fPosition.X() << ", " << fPosition.Y() << ", " << fPosition.Z() << ") " << endl;
  cout << "  (tb, Charge)     : (" << fTb << ", " << fCharge << ")" << endl;
}

TF1 *KBHit::GetPulseFunction(Option_t *)
{
  auto pulseGen = KBPulseGenerator::GetPulseGenerator();
  auto f1 = pulseGen -> GetPulseFunction("pulse");
  f1 -> SetParameters(fCharge, fTb);
  return f1;
}

void KBHit::SetHitID(Int_t id) { fHitID = id; }
void KBHit::SetPadID(Int_t id) { fPadID = id; }
void KBHit::SetTrackID(Int_t id) { fTrackID = id; }

void KBHit::SetX(Double_t x) { fPosition.SetX(x); }
void KBHit::SetY(Double_t y) { fPosition.SetY(y); }
void KBHit::SetZ(Double_t z) { fPosition.SetZ(z); }
void KBHit::SetDX(Double_t dx) { fPosSigma.SetX(dx); }
void KBHit::SetDY(Double_t dy) { fPosSigma.SetY(dy); }
void KBHit::SetDZ(Double_t dz) { fPosSigma.SetZ(dz); }

void KBHit::SetSection(Int_t section) { fSection = section; }
void KBHit::SetRow(Int_t row) { fRow = row; }
void KBHit::SetLayer(Int_t layer) { fLayer = layer; }

void KBHit::SetTb(Double_t tb) { fTb = tb; }
void KBHit::SetCharge(Double_t charge) { fCharge = charge; }

void KBHit::AddHit(KBHit *hit)
{
  auto charge = hit -> GetCharge();
  fPosition.SetX((fCharge*fPosition.X() + charge*hit->GetX()) / (fCharge + charge));
  fPosition.SetY((fCharge*fPosition.Y() + charge*hit->GetY()) / (fCharge + charge));
  fPosition.SetZ((fCharge*fPosition.Z() + charge*hit->GetZ()) / (fCharge + charge));
  fCharge += charge;

  //TODO: Update fPosSigma and add hit-ids
}

Int_t KBHit::GetHitID() const { return fHitID; }
Int_t KBHit::GetPadID() const { return fPadID; }
Int_t KBHit::GetTrackID() const { return fTrackID; }

TVector3 KBHit::GetPosition() const { return fPosition; }
Double_t KBHit::GetX() const { return fPosition.X(); }
Double_t KBHit::GetY() const { return fPosition.Y(); }
Double_t KBHit::GetZ() const { return fPosition.Z(); }

TVector3 KBHit::GetPosSigma() const { return fPosSigma; }
Double_t KBHit::GetDX() const { return fPosSigma.X(); }
Double_t KBHit::GetDY() const { return fPosSigma.Y(); }
Double_t KBHit::GetDZ() const { return fPosSigma.Z(); }

Int_t KBHit::GetSection() const { return fSection; }
Int_t KBHit::GetRow() const { return fRow; }
Int_t KBHit::GetLayer() const { return fLayer; }

Double_t KBHit::GetTb() const { return fTb; }
Double_t KBHit::GetCharge() const { return fCharge; }

/////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////

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
  pointSet -> SetNextPoint(fPosition.X(),fPosition.Y(),fPosition.Z());
}
#endif
