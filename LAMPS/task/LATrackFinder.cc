//#define PRINT_PROCESS
//#define PRINT_INIT

#include "LATrackFinder.hh"

#include <iostream>
using namespace std;

ClassImp(LATrackFinder)

void LATrackFinder::Init()
{
  fPadPlane = (LAPadPlane *) fTpc -> GetPadPlane();

  fPadD = fPadPlane -> PadDisplacement();

  fCandHits = new vKBHit;
  fGoodHits = new vKBHit;
  fBadHits = new vKBHit;

  fPar -> GetParDouble("LATF_defaultScale", fDefaultScale);
  fPar -> GetParDouble("LATF_trackWCutLL", fTrackWCutLL);
  fPar -> GetParDouble("LATF_trackWCutHL", fTrackWCutHL);
  fPar -> GetParDouble("LATF_trackHCutLL", fTrackHCutLL);
  fPar -> GetParDouble("LATF_trackHCutHL", fTrackHCutHL);

  fFitter = new KBHelixTrackFitter();
}

void LATrackFinder::FindTrack(TClonesArray *in, TClonesArray *out)
{
  fTrackArray = out;

  fPadPlane -> ResetHitMap();
  fPadPlane -> SetHitArray(in);

  fCandHits -> clear();
  fGoodHits -> clear();
  fBadHits -> clear();

  while(1)
  {
    auto track = NewTrack();
    if (track == nullptr)
      break;

    bool survive = false;

#ifdef PRINT_PROCESS
    if (cout << "INIT TRACK" << endl, InitTrack(track))
      if (cout << "TRACK CONTINUUM" << endl, TrackContinuum(track))
        if (cout << "TRACK EXTRAPOLATION" << endl, TrackExtrapolation(track)) {
          TrackConfirmation(track);
          survive = true;
        }
#else
    if (InitTrack(track))
      if (TrackContinuum(track))
        if (TrackExtrapolation(track)) {
          TrackConfirmation(track);
          survive = true;
        }
#endif

    for (auto hit : *fBadHits)
      fPadPlane -> AddHit(hit);
    fBadHits -> clear();

    if (track -> GetNumHits() < 10)
      survive = false;

    if (survive) {
      auto trackHits = track -> GetHitArray();
      auto trackID = track -> GetTrackID();
      for (auto trackHit : *trackHits) {
        trackHit -> AddTrackCand(trackID);
        fPadPlane -> AddHit(trackHit);
      }
    }
    else {
      auto trackHits = track -> GetHitArray();
      for (auto trackHit : *trackHits) {
        trackHit -> AddTrackCand(-1);
        fPadPlane -> AddHit(trackHit);
      }
      fTrackArray -> Remove(track);
    }
  }

  fTrackArray -> Compress();
}

KBHelixTrack *LATrackFinder::NewTrack()
{
  auto hit = fPadPlane -> PullOutNextFreeHit();
  if (hit == nullptr)
    return nullptr;

  Int_t idx = fTrackArray -> GetEntries();
  auto track = new ((*fTrackArray)[idx]) KBHelixTrack(idx);
  track -> AddHit(hit);

  fGoodHits -> push_back(hit);

  return track;
}

bool LATrackFinder::InitTrack(KBHelixTrack *track)
{
  fPadPlane -> PullOutNeighborHitsIn(fGoodHits, fCandHits);
  fGoodHits -> clear();

  Int_t numCandHits = fCandHits -> size();;
  while (numCandHits != 0) {
    sort(fCandHits->begin(), fCandHits->end(), KBHitSortByDistanceTo(track->GetMean()));

    for (Int_t iHit = 0; iHit < numCandHits; iHit++) {
      KBHit *candHit = fCandHits -> back();
      fCandHits -> pop_back();

      Double_t quality = CorrelateSimple(track, candHit);
#ifdef PRINT_INIT
      cout << "Simple Quality : " << quality << endl;
#endif

      if (quality > 0) {
        fGoodHits -> push_back(candHit);
        track -> AddHit(candHit);

        if (track -> GetNumHits() > 15) {
          for (auto candHit2 : *fCandHits)
            fPadPlane -> AddHit(candHit2);
          fCandHits -> clear();
          break;
        }

        if (track -> GetNumHits() > 6) {
          fFitter -> Fit(track);
          if (!(track -> GetNumHits() < 10 && track -> GetHelixRadius() < 30) && (track -> TrackLength() > 2.5 * track -> GetRMSW()))
            return true;
        }

        fFitter -> FitPlane(track);
      }
      else
        fBadHits -> push_back(candHit);
    }

    fPadPlane -> PullOutNeighborHitsIn(fGoodHits, fCandHits);
    fGoodHits -> clear();

    numCandHits = fCandHits -> size();
  }

  for (auto badHit : *fBadHits)
    fPadPlane -> AddHit(badHit);
  fBadHits -> clear();

  return false;
}

bool LATrackFinder::TrackContinuum(KBHelixTrack *track)
{
  fPadPlane -> PullOutNeighborHitsIn(fGoodHits, fCandHits);
  fGoodHits -> clear();

  Int_t numCandHits = fCandHits -> size();

  while (numCandHits != 0)
  {
    sort(fCandHits -> begin(), fCandHits -> end(), KBHitSortCharge());

    for (Int_t iHit = 0; iHit < numCandHits; iHit++) {
      KBHit *candHit = fCandHits -> back();
      fCandHits -> pop_back();

      Double_t quality = 0; 
      if (CheckHitOwner(candHit) == -2)
        quality = Correlate(track, candHit);

      if (quality > 0) {
        fGoodHits -> push_back(candHit);
        track -> AddHit(candHit);
        fFitter -> Fit(track);
      } else
        fBadHits -> push_back(candHit);
    }

    fPadPlane -> PullOutNeighborHitsIn(fGoodHits, fCandHits);
    fGoodHits -> clear();

    numCandHits = fCandHits -> size();
  }

  return TrackQualityCheck(track);
}

bool LATrackFinder::TrackExtrapolation(KBHelixTrack *track)
{
  for (auto badHit : *fBadHits)
    fPadPlane -> AddHit(badHit);
  fBadHits -> clear();

  Int_t count = 0;
  bool buildHead = true;
  Double_t extrapolationLength = 0;
  while (AutoBuildByExtrapolation(track, buildHead, extrapolationLength)) {
    if (count++ > 100)
      break;
  }

  count = 0;
  buildHead = !buildHead;
  extrapolationLength = 0;
  while (AutoBuildByExtrapolation(track, buildHead, extrapolationLength)) {
    if (count++ > 100)
      break;
  }

  for (auto badHit : *fBadHits)
    fPadPlane -> AddHit(badHit);
  fBadHits -> clear();

  return TrackQualityCheck(track);
}

bool LATrackFinder::TrackConfirmation(KBHelixTrack *track)
{
  auto tailToHead = false;
  if (track -> PositionAtTail().Z() > track -> PositionAtHead().Z())
    tailToHead = true;

  for (auto badHit : *fBadHits)
    fPadPlane -> AddHit(badHit);
  fBadHits -> clear();
  ConfirmHits(track, tailToHead);

  tailToHead = !tailToHead; 

  for (auto badHit : *fBadHits)
    fPadPlane -> AddHit(badHit);
  fBadHits -> clear();
  ConfirmHits(track, tailToHead);

  for (auto badHit : *fBadHits)
    fPadPlane -> AddHit(badHit);
  fBadHits -> clear();

  return true;
}

Int_t LATrackFinder::CheckHitOwner(KBHit *hit)
{
  auto candTracks = hit -> GetTrackCandArray();
  if (candTracks -> size() == 0)
    return -2;

  Int_t trackID = -1;
  for (auto candTrackID : *candTracks) {
    if (candTrackID != -1) {
      trackID = candTrackID;
    }
  }

  return trackID;
}

Double_t LATrackFinder::Correlate(KBHelixTrack *track, KBHit *hit, Double_t rScale)
{
  Double_t scale = rScale * fDefaultScale;
  Double_t trackLength = track -> TrackLength();
  if (trackLength < 500.)
    scale = scale + (500. - trackLength)/500.;

  Double_t rmsWCut = track -> GetRMSW();
  if (rmsWCut < fTrackWCutLL) rmsWCut = fTrackWCutLL;
  if (rmsWCut > fTrackWCutHL) rmsWCut = fTrackWCutHL;
  rmsWCut = scale * rmsWCut;

  Double_t rmsHCut = track -> GetRMSH();
  if (rmsHCut < fTrackHCutLL) rmsHCut = fTrackHCutLL;
  if (rmsHCut > fTrackHCutHL) rmsHCut = fTrackHCutHL;
  rmsHCut = scale * rmsHCut;

  auto qHead = track -> Map(track -> PositionAtHead());
  auto qTail = track -> Map(track -> PositionAtTail());
  auto q = track -> Map(hit -> GetPosition());

  auto pos = hit -> GetPosition();

  /*
  auto LengthAlphaCut = [track](Double_t dLength) {
    if (dLength > 0) {
      if (dLength > .5*track -> TrackLength()) {
        if (abs(track -> AlphaByLength(dLength)) > .5*TMath::Pi()) {
          return true;
        }
      }
    }
    return false;
  };

  if (qHead.Z() > qTail.Z()) {
    if (LengthAlphaCut(q.Z() - qHead.Z())) return 0;
    if (LengthAlphaCut(qTail.Z() - q.Z())) return 0;
  } else {
    if (LengthAlphaCut(q.Z() - qTail.Z())) return 0;
    if (LengthAlphaCut(qHead.Z() - q.Z())) return 0;
  }
  */

  Double_t dr = abs(q.X());
  Double_t quality = 0;
  if (dr < rmsWCut && abs(q.Y()) < rmsHCut)
    quality = sqrt((dr-rmsWCut)*(dr-rmsWCut)) / rmsWCut;

  return quality;
}

Double_t LATrackFinder::CorrelateSimple(KBHelixTrack *track, KBHit *hit)
{
  if (hit -> GetNumTrackCands() != 0)
    return 0;

  Double_t quality = 0;

  Int_t row = hit -> GetRow();
  Int_t layer = hit -> GetLayer();

  auto trackHits = track -> GetHitArray();
  bool ycut = false;
  for (auto trackHit : *trackHits) {
    if (row == trackHit -> GetRow() && layer == trackHit -> GetLayer())
      return 0;
    auto tp = trackHit -> GetPosition();
#ifdef PRINT_INIT
    cout << "Z-cut: " << abs(hit->GetY()-tp.Y()) << " <? " << 1.2 * fPadD*abs(tp.Y())/sqrt(tp.X()*tp.X()+tp.Z()*tp.Z()) << endl;
#endif
    auto ycutv = 1.2 * fPadD*abs(tp.Y())/sqrt(tp.X()*tp.X()+tp.Z()*tp.Z());
    if (ycutv < 4)
      ycutv = 4;
    if (abs(hit->GetY()-tp.Y()) < ycutv) {
      ycut = true;
#ifdef PRINT_INIT
    cout << "true!" << endl;
#endif
    }
#ifdef PRINT_INIT
    else
      cout << "false!" << endl;
#endif
  }
  if (ycut == false)
    return 0;

  if (track -> IsBad()) {
    quality = 1;
  }
  else if (track -> IsLine()) {
    auto perp = track -> PerpLine(hit -> GetPosition());

    Double_t rmsCut = track -> GetRMSH();
    if (rmsCut < fTrackHCutLL) rmsCut = fTrackHCutLL;
    if (rmsCut > fTrackHCutHL) rmsCut = fTrackHCutHL;
    rmsCut = 3 * rmsCut;

    if (perp.Y() > rmsCut)
      quality = 0;
    else {
      perp.SetY(0);
      if (perp.Mag() < 15)
      //if (perp.Mag() < 10*tp.Y()/sqrt(tp.Mag()))
        quality = 1;
    }
  }
  else if (track -> IsPlane()) {
    Double_t dist = (track -> PerpPlane(hit -> GetPosition())).Mag();

    Double_t rmsCut = track -> GetRMSH();
    if (rmsCut < fTrackHCutLL) rmsCut = fTrackHCutLL;
    if (rmsCut > fTrackHCutHL) rmsCut = fTrackHCutHL;
    rmsCut = 3 * rmsCut;

    if (dist < rmsCut)
      quality = 1;
  }

  return quality;
}

bool LATrackFinder::ConfirmHits(KBHelixTrack *track, bool &tailToHead)
{
  track -> SortHits(!tailToHead);
  auto trackHits = track -> GetHitArray();
  Int_t numHits = trackHits -> size();

  TVector3 q, m;
  auto lPre = track -> ExtrapolateByMap(trackHits->at(numHits-1)->GetPosition(), q, m);

  auto extrapolationLength = 10.;
  for (auto iHit = 1; iHit < numHits; iHit++) 
  {
    KBHit *trackHit = trackHits -> at(numHits-iHit-1);
    auto lCur = track -> ExtrapolateByMap(trackHit->GetPosition(), q, m);

    Double_t quality = Correlate(track, trackHit);

    if (quality <= 0) {
      track -> RemoveHit(trackHit);
      trackHit -> RemoveTrackCand(trackHit -> GetTrackID());
      auto helicity = track -> Helicity();
      fFitter -> Fit(track);
      if (helicity != track -> Helicity())
        tailToHead = !tailToHead;
    }

    auto dLength = abs(lCur - lPre);
    extrapolationLength = 10;
    while(dLength > 0 && AutoBuildByInterpolation(track, tailToHead, extrapolationLength, 1)) { dLength -= 10; }
  }

  extrapolationLength = 0;
  while (AutoBuildByExtrapolation(track, tailToHead, extrapolationLength)) {}

  return true;
}

bool LATrackFinder::AutoBuildByExtrapolation(KBHelixTrack *track, bool &buildHead, Double_t &extrapolationLength)
{
  TVector3 p;
  if (buildHead) p = track -> ExtrapolateHead(extrapolationLength);
  else           p = track -> ExtrapolateTail(extrapolationLength);

  return AutoBuildAtPosition(track, p, buildHead, extrapolationLength);
}

bool LATrackFinder::AutoBuildByInterpolation(KBHelixTrack *track, bool &tailToHead, Double_t &extrapolationLength, Double_t rScale)
{
  TVector3 p;
  if (tailToHead) p = track -> InterpolateByLength(extrapolationLength);
  else            p = track -> InterpolateByLength(track -> TrackLength() - extrapolationLength);

  return AutoBuildAtPosition(track, p, tailToHead, extrapolationLength, rScale);
}

bool LATrackFinder::AutoBuildAtPosition(KBHelixTrack *track, TVector3 p, bool &tailToHead, Double_t &extrapolationLength, Double_t rScale)
{
  if (fPadPlane -> IsInBoundary(p.X(), p.Z()) == false)
    return false;

  auto helicity = track -> Helicity();

  Double_t rms = 3*track -> GetRMSW();
  if (rms < 25) 
    rms = 25;

  Int_t range = Int_t(rms/8);
  TVector3 q(p.Z(), p.X(), p.Y());
  fPadPlane -> PullOutNeighborHits(q, range, fCandHits);

  Int_t numCandHits = fCandHits -> size();
  Bool_t foundHit = false;

  if (numCandHits != 0) 
  {
    sort(fCandHits -> begin(), fCandHits -> end(), KBHitSortCharge());

    for (Int_t iHit = 0; iHit < numCandHits; iHit++) {
      KBHit *candHit = fCandHits -> back();
      fCandHits -> pop_back();
      auto pos = candHit -> GetPosition();

      Double_t quality = 0; 
      if (CheckHitOwner(candHit) < 0) 
        quality = Correlate(track, candHit, rScale);

      if (quality > 0) {
        track -> AddHit(candHit);
        fFitter -> Fit(track);
        foundHit = true;
      } else
        fBadHits -> push_back(candHit);
    }
  }

  if (foundHit) {
    extrapolationLength = 10; 
    if (helicity != track -> Helicity())
      tailToHead = !tailToHead;
  }
  else {
    extrapolationLength += 10; 
    if (extrapolationLength > 3 * track -> TrackLength()) {
      return false;
    }
  }

  return true;
}

bool LATrackFinder::TrackQualityCheck(KBHelixTrack *track)
{
  Double_t continuity = track -> Continuity();
  if (continuity < .6) {
    if (track -> TrackLength() * continuity < 500)
      return false;
  }

  if (track -> GetHelixRadius() < 25)
    return false;

  return true;
}
