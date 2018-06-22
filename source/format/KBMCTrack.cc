#include "KBMCTrack.hh"
#include "TVector3.h"
#ifdef ACTIVATE_EVE
#include "TEveLine.h"
#endif

ClassImp(KBMCTrack)

KBMCTrack::KBMCTrack()
{
  Clear();
}

KBMCTrack::~KBMCTrack()
{
}

void KBMCTrack::Clear(Option_t *option)
{
  fTrackID = -1;
  fParentID = -1;
  fPDG = -1;
  fPX = -999;
  fPY = -999;
  fPZ = -999;
}

void KBMCTrack::SetTrackID(Int_t val)  { fTrackID = val; }
void KBMCTrack::SetParentID(Int_t val) { fParentID = val; }
void KBMCTrack::SetPDG(Int_t val)      { fPDG = val; }
void KBMCTrack::SetPX(Double_t val)    { fPX = val; }  
void KBMCTrack::SetPY(Double_t val)    { fPY = val; }
void KBMCTrack::SetPZ(Double_t val)    { fPZ = val; }

void KBMCTrack::SetMCTrack(Int_t trackID, Int_t parentID, Int_t pdg, Double_t px, Double_t py, Double_t pz)
{
  fTrackID = trackID;
  fParentID = parentID;
  fPDG = pdg;
  fPX = px;
  fPY = py;
  fPZ = pz;
}

Int_t KBMCTrack::GetTrackID()  const { return fTrackID; }
Int_t KBMCTrack::GetParentID() const { return fParentID; }
Int_t KBMCTrack::GetPDG()      const { return fPDG; }
Double_t KBMCTrack::GetPX()    const { return fPX; }
Double_t KBMCTrack::GetPY()    const { return fPY; }
Double_t KBMCTrack::GetPZ()    const { return fPZ; }
TVector3 KBMCTrack::GetMomentum() const { return TVector3(fPX, fPY, fPZ); }

void KBMCTrack::AddStep(KBMCStep *hit) { fStepArray.push_back(hit); }
vector<KBMCStep *> *KBMCTrack::GetStepArray() { return &fStepArray; }

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
  line -> SetElementName("MCTrack");
  line -> Reset();

  line -> SetLineColor(kBlue);
  line -> SetNextPoint(0,0,0);
  auto pos = 1000*TVector3(fPX, fPY, fPZ).Unit();
  line -> SetNextPoint(pos.X(), pos.Y(), pos.Z());
}

void KBMCTrack::AddToEveSet(TEveElement *)
{
}
#endif
