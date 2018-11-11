#include "KBMCTrack.hh"
#include "TVector3.h"

#ifdef ACTIVATE_EVE
#include "TEveLine.h"
#endif

#include <iostream>
#include <iomanip>
using namespace std;

ClassImp(KBMCTrack)

KBMCTrack::KBMCTrack()
{
  Clear();
}

void KBMCTrack::Clear(Option_t *option)
{
  KBTracklet::Clear(option);

  fTrackID = -1;
  fParentID = -1;

  fPDG = -1;
  fPX = -999;
  fPY = -999;
  fPZ = -999;
  fVX = -999;
  fVY = -999;
  fVZ = -999;
}

void KBMCTrack::Print(Option_t *) const
{
  cout << "[KBMCTrack] Momentum: (" << setw(12) << fPX << "," << setw(12) << fPY << "," << setw(12) << fPZ << ")" << endl;
  cout << "    Primary position: (" << setw(12) << fVX << "," << setw(12) << fVY << "," << setw(12) << fVZ << ")" << endl;
}

void KBMCTrack::SetPDG(Int_t val)      { fPDG = val; }
void KBMCTrack::SetPX(Double_t val)    { fPX = val; }  
void KBMCTrack::SetPY(Double_t val)    { fPY = val; }
void KBMCTrack::SetPZ(Double_t val)    { fPZ = val; }
void KBMCTrack::SetVX(Double_t val)    { fVX = val; }
void KBMCTrack::SetVY(Double_t val)    { fVY = val; }
void KBMCTrack::SetVZ(Double_t val)    { fVZ = val; }

void KBMCTrack::SetMCTrack(Int_t trackID, Int_t parentID, Int_t pdg, Double_t px, Double_t py, Double_t pz, Double_t vx, Double_t vy, Double_t vz)
{
  fTrackID = trackID;
  fParentID = parentID;
  fPDG = pdg;
  fPX = px;
  fPY = py;
  fPZ = pz;
  fVX = vx;
  fVY = vy;
  fVZ = vz;
}

Int_t KBMCTrack::GetPDG() const { return fPDG; }
Double_t KBMCTrack::GetPX() const { return fPX; }
Double_t KBMCTrack::GetPY() const { return fPY; }
Double_t KBMCTrack::GetPZ() const { return fPZ; }
TVector3 KBMCTrack::GetMomentum() const { return TVector3(fPX, fPY, fPZ); }

Double_t KBMCTrack::GetVX() const { return fVX; }
Double_t KBMCTrack::GetVY() const { return fVY; }
Double_t KBMCTrack::GetVZ() const { return fVZ; }
TVector3 KBMCTrack::GetPrimaryPosition() const { return TVector3(fVX, fVY, fVZ); }

void KBMCTrack::AddStep(KBMCStep *hit) { fStepArray.push_back(hit); }
vector<KBMCStep *> *KBMCTrack::GetStepArray() { return &fStepArray; }

TVector3 KBMCTrack::Momentum(Double_t B) const { return GetMomentum(); }
TVector3 KBMCTrack::PositionAtHead() const { return GetPrimaryPosition() + GetMomentum(); }
TVector3 KBMCTrack::PositionAtTail() const { return GetPrimaryPosition(); }
Double_t KBMCTrack::TrackLength() const { return GetMomentum().Mag(); }

TVector3 KBMCTrack::ExtrapolateTo(TVector3)       const { return TVector3(); } //@todo
TVector3 KBMCTrack::ExtrapolateHead(Double_t)     const { return TVector3(); } //@todo
TVector3 KBMCTrack::ExtrapolateTail(Double_t)     const { return TVector3(); } //@todo
TVector3 KBMCTrack::ExtrapolateByRatio(Double_t)  const { return TVector3(); } //@todo
TVector3 KBMCTrack::ExtrapolateByLength(Double_t) const { return TVector3(); } //@todo
Double_t KBMCTrack::LengthAt(TVector3)            const { return 0; }          //@todo

#ifdef ACTIVATE_EVE
bool KBMCTrack::DrawByDefault() { return true; }
bool KBMCTrack::IsEveSet() { return false; }

TEveElement *KBMCTrack::CreateEveElement()
{
  auto element = new TEveLine();

  return element;
}

void KBMCTrack::SetEveElement(TEveElement *element)
{
  auto line = (TEveLine *) element;
  line -> Reset();

  if (fPDG == 211 || fPDG == -211) {
    line -> SetElementName(Form("MCTrack%d_Pion",fTrackID));
    line -> SetLineColor(kBlue-4);
  }
  if (fPDG == 2212) {
    line -> SetElementName(Form("MCTrack%d_Proton",fTrackID));
    line -> SetLineColor(kRed-7);
  }
  else if (fPDG == 2112) {
    line -> SetElementName(Form("MCTrack%d_Neutron",fTrackID));
    line -> SetLineColor(kGray);
  }
  else if (fPDG == 11 || fPDG == -11) {
    line -> SetElementName(Form("MCTrack%d_Electron",fTrackID));
    line -> SetLineColor(kGreen);
  }
  else {
    line -> SetElementName(Form("MCTrack%d",fTrackID));
    line -> SetLineColor(kMagenta+2);
  }

  line -> SetNextPoint(fVX, fVY, fVZ);
  line -> SetNextPoint(fPX, fPY, fPZ);
}

void KBMCTrack::AddToEveSet(TEveElement *)
{
}
#endif
