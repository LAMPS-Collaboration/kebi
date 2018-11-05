#include "KBTracklet.hh"
#ifdef ACTIVATE_EVE
#include "TEveLine.h"
#endif

ClassImp(KBTracklet)

void KBTracklet::PropagateMC()
{
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

void KBTracklet::AddHit(KBHit *hit)
{
  fHitList.AddHit(hit);
}

void KBTracklet::RemoveHit(KBHit *hit)
{
  fHitList.RemoveHit(hit);
}

#ifdef ACTIVATE_EVE
bool KBTracklet::DrawByDefault() { return true; }
bool KBTracklet::IsEveSet() { return false; }

TEveElement *KBTracklet::CreateEveElement()
{
  auto element = new TEveLine();

  return element;
}

void KBTracklet::SetEveElement(TEveElement *element)
{
  auto line = (TEveLine *) element;
  line -> SetElementName("Tracklet");
  line -> Reset();

  line -> SetElementName(Form("Tracklet%d",fTrackID));

  if (fParentID > -1)
    line -> SetLineColor(kPink);
  else
    line -> SetLineColor(kGray);

  auto dr = 0.02;
  if (dr < 5./TrackLength())
    dr = 5./TrackLength();

  for (Double_t r = 0.; r < 1.0001; r += dr) {
    auto pos = ExtrapolateByRatio(r);
    line -> SetNextPoint(pos.X(), pos.Y(), pos.Z());
  }
}

void KBTracklet::AddToEveSet(TEveElement *)
{
}
#endif

bool KBTracklet::DoDrawOnDetectorPlane()
{
  return true;
}

TGraph *KBTracklet::TrajectoryOnPlane(KBVector3::Axis axis1, KBVector3::Axis axis2)
{
  if (fTrajectoryOnPlane == nullptr) {
    fTrajectoryOnPlane = new TGraph();
    fTrajectoryOnPlane -> SetLineColor(kRed);
  }

  fTrajectoryOnPlane -> Set(0);

  for (Double_t r = 0.; r < 1.001; r += 0.02) {
    auto pos = KBVector3(ExtrapolateByRatio(r),fReferenceAxis);
    fTrajectoryOnPlane -> SetPoint(fTrajectoryOnPlane->GetN(), pos.At(axis1), pos.At(axis2));
  }

  return fTrajectoryOnPlane;
}
