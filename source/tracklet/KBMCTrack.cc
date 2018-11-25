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
  fPX.clear(); fPX.push_back(-999);
  fPY.clear(); fPY.push_back(-999);
  fPZ.clear(); fPZ.push_back(-999);
  fVX.clear(); fVX.push_back(-999);
  fVY.clear(); fVY.push_back(-999);
  fVZ.clear(); fVZ.push_back(-999);
}

void KBMCTrack::Print(Option_t *) const
{
  kc_info << "        Momentum: (" << setw(12) << fPX[0] << "," << setw(12) << fPY[0] << "," << setw(12) << fPZ[0] << ")" << endl;
  kc_info << "Primary position: (" << setw(12) << fVX[0] << "," << setw(12) << fVY[0] << "," << setw(12) << fVZ[0] << ")" << endl;
}

void KBMCTrack::SetPDG(Int_t val)      { fPDG = val; }
void KBMCTrack::SetPX(Double_t val)    { fPX[0] = val; }
void KBMCTrack::SetPY(Double_t val)    { fPY[0] = val; }
void KBMCTrack::SetPZ(Double_t val)    { fPZ[0] = val; }
void KBMCTrack::SetVX(Double_t val)    { fVX[0] = val; }
void KBMCTrack::SetVY(Double_t val)    { fVY[0] = val; }
void KBMCTrack::SetVZ(Double_t val)    { fVZ[0] = val; }

void KBMCTrack::SetMCTrack(Int_t trackID, Int_t parentID, Int_t pdg, Double_t px, Double_t py, Double_t pz, Double_t vx, Double_t vy, Double_t vz)
{
  fTrackID = trackID;
  fParentID = parentID;
  fPDG = pdg;
  fPX[0] = px;
  fPY[0] = py;
  fPZ[0] = pz;
  fVX[0] = vx;
  fVY[0] = vy;
  fVZ[0] = vz;
}

void KBMCTrack::AddVertex(Double_t px, Double_t py, Double_t pz, Double_t vx, Double_t vy, Double_t vz)
{
  fPX.push_back(px);
  fPY.push_back(py);
  fPZ.push_back(pz);
  fVX.push_back(vx);
  fVY.push_back(vy);
  fVZ.push_back(vz);
}

Int_t KBMCTrack::GetNumVertices() const { return (Int_t) fPX.size(); }

Int_t KBMCTrack::GetPDG() const { return fPDG; }
Double_t KBMCTrack::GetPX(Int_t idx) const { return fPX[idx]; }
Double_t KBMCTrack::GetPY(Int_t idx) const { return fPY[idx]; }
Double_t KBMCTrack::GetPZ(Int_t idx) const { return fPZ[idx]; }
TVector3 KBMCTrack::GetMomentum(Int_t idx) const { return TVector3(fPX[idx], fPY[idx], fPZ[idx]); }

Double_t KBMCTrack::GetVX(Int_t idx) const { return fVX[idx]; }
Double_t KBMCTrack::GetVY(Int_t idx) const { return fVY[idx]; }
Double_t KBMCTrack::GetVZ(Int_t idx) const { return fVZ[idx]; }
TVector3 KBMCTrack::GetVertex(Int_t idx) const { return TVector3(fVX[idx], fVY[idx], fVZ[idx]); }

TVector3 KBMCTrack::GetPrimaryPosition() const { return TVector3(fVX[0], fVY[0], fVZ[0]); }

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

  Int_t numVertices = fPX.size();
  if (numVertices==1) {
    line -> SetNextPoint(fVX[0], fVY[0], fVZ[0]);
    line -> SetNextPoint(fPX[0], fPY[0], fPZ[0]);
  }
  else {
    for (auto idx = 0; idx < numVertices; ++idx)
      line -> SetNextPoint(fVX[idx], fVY[idx], fVZ[idx]);
  }
}

void KBMCTrack::AddToEveSet(TEveElement *)
{
}
#endif
