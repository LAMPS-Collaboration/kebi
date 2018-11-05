#include "KBHit.hh"
#include "KBPulseGenerator.hh"
#ifdef ACTIVATE_EVE
#include "TEvePointSet.h"
#endif
#include "TMath.h"
#include <iostream>
#include <iomanip>

ClassImp(KBHit)

void KBHit::Clear(Option_t *)
{
  KBWPointCluster::Clear();

  fHitID = -1;
  fTrackID = -1;

  fTrackCandArray.clear();

  fHitList.Clear();
}

void KBHit::Print(Option_t *option) const
{
  cout << "HTM-ID|XYZ|Charge: "
    << setw(4)  << fHitID
    << setw(4)  << fTrackID
    << setw(4)  << fMCID << " |"
    << setw(12) << fX
    << setw(12) << fY
    << setw(12) << fZ << " |"
    << setw(12) << fW << endl;

  if (TString(option).Index("r")>=0)
    fHitList.Print("r");
}

void KBHit::Copy(TObject &obj) const
{
  KBWPointCluster::Copy(obj);
  auto hit = (KBHit &) obj;

  hit.SetHitID(fHitID);
  hit.SetTrackID(fTrackID);

  auto numHits = fHitList.GetNumHits();
  for (auto i = 0; i < numHits; ++i)
    hit.AddHit(fHitList.GetHit(i));
}

void KBHit::PropagateMC()
{
  if (!IsCluster())
    return;

  auto hitArray = fHitList.GetHitArray();
  vector<Int_t> mcIDs;
  vector<Int_t> counts;

  for (auto component : *hitArray) {
    auto mcIDCoponent = component -> GetMCID();

    Int_t numMCIDs = mcIDs.size();
    Int_t idxFound = -1;
    for (Int_t idx = 0; idx < numMCIDs; ++idx) {
      if (mcIDs[idx] == mcIDCoponent) {
        idxFound = idx;
        break;
      }
    }
    if (idxFound == -1) {
      mcIDs.push_back(mcIDCoponent);
      counts.push_back(1);
    }
    else {
      counts[idxFound] = counts[idxFound] + 1;
    }
  }

  auto maxCount = 0;
  for (auto count : counts)
    if (count > maxCount)
      maxCount = count;

  vector<Int_t> iIDCandidates;
  for (auto iID = 0; iID < Int_t(counts.size()); ++iID)
    if (counts[iID] == maxCount)
      iIDCandidates.push_back(iID);


  //TODO @todo
  if (iIDCandidates.size() == 1)
  {
    auto iID = iIDCandidates[0];
    auto mcIDFinal = mcIDs[iID];

    auto errorFinal = 0.;
    for (auto component : *hitArray)
      if (component -> GetMCID() == mcIDFinal)
        errorFinal += component -> GetMCError();

    errorFinal = errorFinal/counts[iID];
    Double_t purity = Double_t(counts[iID])/hitArray->size();
    SetMCID(mcIDFinal, errorFinal, purity);
  }
  else
  {
    auto mcIDFinal = 0;
    auto errorFinal = DBL_MAX;
    Double_t purity = -1;

    for (auto iID : iIDCandidates) {
      auto mcIDCand = mcIDs[iID];

      auto errorCand = 0.;
      for (auto component : *hitArray)
        if (component -> GetMCID() == mcIDCand)
          errorCand += component -> GetMCError();
      errorCand = errorCand/counts[iID];

      if (errorCand < errorFinal) {
        mcIDFinal = mcIDCand;
        errorFinal = errorCand;
        purity = Double_t(counts[iID])/hitArray->size();
      }
    }
    SetMCID(mcIDFinal, errorFinal, purity);
  }
}

Bool_t KBHit::IsCluster() { return fHitList.GetNumHits() == 0 ? false : true; }

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
  KBWPointCluster::Add((KBWPoint &) *hit);
  fHitList.AddHit(hit);
}

void KBHit::RemoveHit(KBHit *hit)
{
  KBWPointCluster::Remove((KBWPoint &) *hit);
  fHitList.RemoveHit(hit);
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
