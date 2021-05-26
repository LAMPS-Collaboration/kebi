#include "KBRun.hh"
#include "LAPTrackFindingTask.hh"

#include <iostream>

//#define DEBUG_STEP
//#define DEBUG_CORR
//#define DEBUG_HITSUMMARY
//#define DEBUT_HITRANGE

ClassImp(LAPTrackFindingTask)

bool LAPTrackFindingTask::Init()
{
  auto run = KBRun::GetRun();
  fTpc = (KBTpc *) (run -> GetDetectorSystem() -> GetTpc());
  fPadPlane = (KBPadPlane *) fTpc -> GetPadPlane();

  fHitArray = (TClonesArray *) run -> GetBranch(fBranchNameHit);

  fTrackArray = new TClonesArray("KBLinearTrack");
  run -> RegisterBranch(fBranchNameTracklet, fTrackArray, fPersistency);

  fTrackHits = new KBHitArray();
  fCandHits = new KBHitArray();
  fGoodHits = new KBHitArray();
  fBadHits  = new KBHitArray();

  run -> RegisterBranch("TrackHit", fTrackHits, false);
  run -> RegisterBranch("CandHit",  fCandHits,  false);
  run -> RegisterBranch("GoodHit",  fGoodHits,  false);
  run -> RegisterBranch("BadHit",   fBadHits,   false);

  // XXX
  fMinHitsToFitInitTrack = fPar -> GetParInt("laptf_minNumHitsInitTrack");
  fCutMinNumHitsFinalTrack = fPar -> GetParInt("laptf_minNumHitsFinalTrack");
  fTrackLengthCutScale = fPar -> GetParDouble("laptf_trackLegnthCutScale");

  fNextStep = StepNo::kStepInitArray;

  auto numSections = fPar -> GetParInt("numSections");
  for (auto section=0; section<numSections; ++section) {
    double w = fPar -> GetParDouble("padWidth",section);
    double h = fPar -> GetParDouble("padHeight",section);
    if (w<h) fMinGeomDistanceInPlane[section] = w;
    else     fMinGeomDistanceInPlane[section] = h;
    fMaxGeomDistanceInPlane[section] = sqrt(w*w + h*h);
  }

  if (fPar -> CheckPar("laptf_angleDivisionArray")) {
    fAngleDivisionIndex = 0;
    fNumAngleDivision = fPar -> GetParN("laptf_angleDivisionArray") - 1;
    fAngleDivisionArray = fPar -> GetParVDouble("laptf_angleDivisionArray");
  }

  if (fPar -> CheckPar("laptf_firstNeighborRange"))
    fFirstNeighborRange = fPar -> GetParInt("laptf_firstNeighborRange");
  else
    fFirstNeighborRange = 1;

  return true;
}

void LAPTrackFindingTask::Exec(Option_t*)
{
  while (ExecStep()) {}
}

bool LAPTrackFindingTask::ExecStep(Option_t *opt)
{
#ifdef DEBUG_STEP
       if (fNextStep==kStepInitArray)           kb_debug << "Current step is StepInitArray" << endl;
  else if (fNextStep==kStepNewTrack)            kb_debug << "Current step is StepNewTrack" << endl;
  else if (fNextStep==kStepRemoveTrack)         kb_debug << "Current step is StepRemoveTrack" << endl;
  else if (fNextStep==kStepInitTrack)           kb_debug << "Current step is StepInitTrack" << endl;
  else if (fNextStep==kStepInitTrackAddHit)     kb_debug << "Current step is StepInitTrackAddHit" << endl;
  else if (fNextStep==kStepContinuum)           kb_debug << "Current step is StepContinuum" << endl;
  else if (fNextStep==kStepContinuumAddHit)     kb_debug << "Current step is StepContinuumAddHit" << endl;
  else if (fNextStep==kStepExtrapolation)       kb_debug << "Current step is StepExtrapolation" << endl;
  else if (fNextStep==kStepExtrapolationAddHit) kb_debug << "Current step is StepExtrapolationAddHit" << endl;
  else if (fNextStep==kStepConfirmation)        kb_debug << "Current step is StepConfirmation" << endl;
  else if (fNextStep==kStepFinalizeTrack)       kb_debug << "Current step is StepFinalizeTrack" << endl;
  else if (fNextStep==kStepNextPhase)           kb_debug << "Current step is StepNextPhase" << endl;
  else if (fNextStep==kStepEndEvent)            kb_debug << "Current step is StepEndEvent" << endl;
#endif

  if (fNextStep==kStepEndOfEvent) {
    fNextStep = StepNo::kStepInitArray;
    return false; 
  }
  else if (fNextStep==kStepInitArray)           fNextStep = StepInitArray();
  else if (fNextStep==kStepNewTrack)            fNextStep = StepNewTrack();
  else if (fNextStep==kStepRemoveTrack)         fNextStep = StepRemoveTrack();
  else if (fNextStep==kStepInitTrack)           fNextStep = StepInitTrack();
  else if (fNextStep==kStepInitTrackAddHit)     fNextStep = StepInitTrackAddHit();
  else if (fNextStep==kStepContinuum)           fNextStep = StepContinuum();
  else if (fNextStep==kStepContinuumAddHit)     fNextStep = StepContinuumAddHit();
  else if (fNextStep==kStepExtrapolation)       fNextStep = StepExtrapolation();
  else if (fNextStep==kStepExtrapolationAddHit) fNextStep = StepExtrapolationAddHit();
  else if (fNextStep==kStepConfirmation)        fNextStep = StepConfirmation();
  else if (fNextStep==kStepFinalizeTrack)       fNextStep = StepFinalizeTrack();
  else if (fNextStep==kStepNextPhase)           fNextStep = StepNextPhase();
  else if (fNextStep==kStepEndEvent)            fNextStep = StepEndEvent();

#ifdef DEBUG_STEP
       if (fNextStep==kStepInitArray)           kb_debug << "Next step is StepInitArray" << endl;
  else if (fNextStep==kStepNewTrack)            kb_debug << "Next step is StepNewTrack" << endl;
  else if (fNextStep==kStepRemoveTrack)         kb_debug << "Next step is StepRemoveTrack" << endl;
  else if (fNextStep==kStepInitTrack)           kb_debug << "Next step is StepInitTrack" << endl;
  else if (fNextStep==kStepInitTrackAddHit)     kb_debug << "Next step is StepInitTrackAddHit" << endl;
  else if (fNextStep==kStepContinuum)           kb_debug << "Next step is StepContinuum" << endl;
  else if (fNextStep==kStepContinuumAddHit)     kb_debug << "Next step is StepContinuumAddHit" << endl;
  else if (fNextStep==kStepExtrapolation)       kb_debug << "Next step is StepExtrapolation" << endl;
  else if (fNextStep==kStepExtrapolationAddHit) kb_debug << "Next step is StepExtrapolationAddHit" << endl;
  else if (fNextStep==kStepConfirmation)        kb_debug << "Next step is StepConfirmation" << endl;
  else if (fNextStep==kStepFinalizeTrack)       kb_debug << "Next step is StepFinalizeTrack" << endl;
  else if (fNextStep==kStepNextPhase)           kb_debug << "Next step is StepNextPhase" << endl;
  else if (fNextStep==kStepEndEvent)            kb_debug << "Next step is StepEndEvent" << endl;

  if (fCurrentTrack != nullptr)
    kb_debug << "number of hits = " << fCurrentTrack -> GetNumHits() << endl;
#endif

#ifdef DEBUG_HITSUMMARY
  cout_info << "pad-plane : ";
  auto numPads = fPadPlane -> GetNumPads();
  Int_t numCheckHits;
  for (auto iPad=0; iPad<numPads; ++iPad) {
    auto pad = (KBPad *) fPadPlane -> GetPad(iPad);
    numCheckHits = pad -> GetNumHits();
    for (auto iHit=0; iHit<numCheckHits; ++iHit) {
      auto hit = (KBTpcHit *) pad -> GetHit(iHit);
      auto hitID = hit -> GetHitID();
      cout << hitID;
           if (hit->IsFreeHit())      { cout << "(f) "; }
      else if (hit->IsUsedHit())      { cout << "(u) "; }
      else if (hit->IsTrackHitCand()) { cout << "(c) "; }
      else if (hit->IsTrackHit())     { cout << "(t) "; }
    }
  }
  cout << endl;

  cout_info << "good-hits : ";
  numCheckHits = fGoodHits -> GetEntriesFast();
  for (auto iHit = 0; iHit<numCheckHits; ++iHit) {
    auto hit = (KBTpcHit *) fGoodHits -> GetHit(iHit);
    auto hitID = hit -> GetHitID();
    cout << hitID;
         if (hit->IsFreeHit())      { cout << "(f) "; }
    else if (hit->IsUsedHit())      { cout << "(u) "; }
    else if (hit->IsTrackHitCand()) { cout << "(c) "; }
    else if (hit->IsTrackHit())     { cout << "(t) "; }
  }
  cout << endl;

  cout_info << "baad-hits : ";
  numCheckHits = fBadHits -> GetEntriesFast();
  for (auto iHit = 0; iHit<numCheckHits; ++iHit) {
    auto hit = (KBTpcHit *) fBadHits -> GetHit(iHit);
    auto hitID = hit -> GetHitID();
    cout << hitID;
         if (hit->IsFreeHit())      { cout << "(f) "; }
    else if (hit->IsUsedHit())      { cout << "(u) "; }
    else if (hit->IsTrackHitCand()) { cout << "(c) "; }
    else if (hit->IsTrackHit())     { cout << "(t) "; }
  }
  cout << endl;

  cout_info << "cand-hits : ";
  numCheckHits = fCandHits -> GetEntriesFast();
  for (auto iHit = 0; iHit<numCheckHits; ++iHit) {
    auto hit = (KBTpcHit *) fCandHits -> GetHit(iHit);
    auto hitID = hit -> GetHitID();
    cout << hitID;
         if (hit->IsFreeHit())      { cout << "(f) "; }
    else if (hit->IsUsedHit())      { cout << "(u) "; }
    else if (hit->IsTrackHitCand()) { cout << "(c) "; }
    else if (hit->IsTrackHit())     { cout << "(t) "; }
  }
  cout << endl;

  cout_info << "trck-hits : ";
  numCheckHits = fTrackHits -> GetEntriesFast();
  for (auto iHit = 0; iHit<numCheckHits; ++iHit) {
    auto hit = (KBTpcHit *) fTrackHits -> GetHit(iHit);
    auto hitID = hit -> GetHitID();
    cout << hitID;
         if (hit->IsFreeHit())      { cout << "(f) "; }
    else if (hit->IsUsedHit())      { cout << "(u) "; }
    else if (hit->IsTrackHitCand()) { cout << "(c) "; }
    else if (hit->IsTrackHit())     { cout << "(t) "; }
  }
  cout << endl;

  auto numTracks = fTrackArray -> GetEntriesFast();
  for (auto iTrack=0; iTrack<numTracks; ++iTrack)
  {
    auto track = (KBLinearTrack *) fTrackArray -> At(iTrack);
    numCheckHits = track -> GetNumHits();
    cout_info << "track(" << right << setw(2) << iTrack << ") : ";
    for (auto iHit = 0; iHit<numCheckHits; ++iHit) {
      auto hit = (KBTpcHit *) track -> GetHit(iHit);
      auto hitID = hit -> GetHitID();
      cout << hitID;
           if (hit->IsFreeHit())      { cout << "(f) "; }
      else if (hit->IsUsedHit())      { cout << "(u) "; }
      else if (hit->IsTrackHitCand()) { cout << "(c) "; }
      else if (hit->IsTrackHit())     { cout << "(t) "; }
    }
    cout << endl;
  }
#endif

  return true;
}

bool LAPTrackFindingTask::ExecStepUptoTrackNum(Int_t numTracks)
{
  if (fNextStep==kStepEndOfEvent)
    return false;

  while (ExecStep()) {
    if (fNextStep==kStepNewTrack && fTrackArray -> GetEntriesFast() >= numTracks)
      break;
  }

  return true;
}

int LAPTrackFindingTask::StepInitArray()
{
  fCurrentTrack = nullptr;
  fAngleDivisionIndex = 0;

  // below should be done if track finding task is run separately from PSA task. because
  // fTrackCandArray is not persistent
  Int_t numHits2 = fHitArray -> GetEntriesFast();
  for (Int_t iHit = 0; iHit < numHits2; ++iHit) {
    KBTpcHit *hit = (KBTpcHit *) fHitArray -> At(iHit);
    hit -> SetFreeHit();
  }

#ifdef DEBUT_HITRANGE
  kb_debug << "set hit range!" << endl;
  if (fPar->CheckPar("debug_limit_xyz")) {
    Int_t numHits = fHitArray -> GetEntriesFast();
    for (Int_t iHit = 0; iHit < numHits; ++iHit) {
      KBTpcHit *hit = (KBTpcHit *) fHitArray -> At(iHit);
      auto x = hit -> GetX();
      auto y = hit -> GetY();
      auto z = hit -> GetZ();
      if (x<fPar->GetParDouble("debug_limit_xyz",0) ||
          x>fPar->GetParDouble("debug_limit_xyz",1) ||
          y<fPar->GetParDouble("debug_limit_xyz",2) ||
          y>fPar->GetParDouble("debug_limit_xyz",3) ||
          z<fPar->GetParDouble("debug_limit_xyz",4) ||
          z>fPar->GetParDouble("debug_limit_xyz",5))
        fHitArray -> Remove(hit);
    }
    fHitArray -> Compress();
  }
#endif

  fPadPlane -> ResetHitMap();
  fPadPlane -> SetHitArray(fHitArray);
  fPadPlane -> ResetEvent();

  fTrackArray -> Clear("C");
  fTrackHits -> Clear();
  fCandHits -> Clear();
  fGoodHits -> Clear();
  fBadHits -> Clear(); 

  if (fPar -> CheckPar("laptf_firstNeighborRange"))
    fFirstNeighborRange = fPar -> GetParInt("laptf_firstNeighborRange");
  else
    fFirstNeighborRange = 1;

  return kStepNewTrack;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int LAPTrackFindingTask::StepNewTrack()
{
  fTrackHits -> Clear();
  fCandHits -> Clear();
  fGoodHits -> Clear();
  ReturnBadHitsToPadPlane();

  KBTpcHit *hit = fPadPlane -> PullOutNextFreeHit();
  if (hit == nullptr) {
    return kStepNextPhase;
  }

  Int_t idx = fTrackArray -> GetEntries();
  fCurrentTrack = (KBLinearTrack *) fTrackArray -> ConstructedAt(idx);
  fCurrentTrack -> SetTrackID(idx);
  fCurrentTrack -> AddHit(hit);
  fCurrentTrack -> Fit();
  fGoodHits -> AddHit(hit);
  hit -> SetUsedHit();

  return kStepInitTrack;
}

int LAPTrackFindingTask::StepRemoveTrack()
{
  fGoodHits -> Clear();
  fTrackHits -> Clear();
  fCandHits -> Clear();

  ReturnBadHitsToPadPlane();

  auto trackHits = fCurrentTrack -> GetHitArray();
  Int_t numTrackHits = trackHits -> GetNumHits();
  for (Int_t iTrackHit = 0; iTrackHit < numTrackHits; ++iTrackHit) {
    auto trackHit = (KBTpcHit *) trackHits -> GetHit(iTrackHit);
    fPadPlane -> AddHit(trackHit);
  }
  fTrackArray -> Remove(fCurrentTrack);

  fCurrentTrack = nullptr;

  return kStepNewTrack;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int LAPTrackFindingTask::StepInitTrack()
{
  fCandHits -> Clear();
  fPadPlane -> PullOutNeighborHits(fGoodHits, fCandHits, fFirstNeighborRange);
  fGoodHits -> MoveHitsTo(fTrackHits);
  fNumCandHits = fCandHits -> GetEntriesFast();
  if (fNumCandHits == 0)
    return kStepRemoveTrack;

  fCandHits -> SortByDistanceTo(fCurrentTrack -> GetCenter(), true);
  return kStepInitTrackAddHit;
}

int LAPTrackFindingTask::StepInitTrackAddHit()
{
  auto candHit = (KBTpcHit *) fCandHits -> GetLastHit();
  fCandHits -> RemoveLastHit();

  Double_t quality = 0;
  if (!(candHit->IsTrackHit()||candHit->IsTrackHitCand()))
    quality = CorrelateHitWithTrack(fCurrentTrack, candHit, true);

  if (quality > 0) {
    fGoodHits -> AddHit(candHit);
    fCurrentTrack -> AddHit(candHit);
    //candHit -> SetTrackID(fCurrentTrack->GetTrackID());
    fCurrentTrack -> Fit();
  }
  else {
    fBadHits -> AddHit(candHit);
  }

  auto numHitsInTrack = fCurrentTrack -> GetNumHits();
  if (fNumCandHits==1 && numHitsInTrack >= fMinHitsToFitInitTrack) {
    fCurrentTrack -> Fit();
    if (fCurrentTrack -> TrackLength() > fTrackLengthCutScale * fCurrentTrack -> GetTrackWidth()) {
      return kStepContinuum;
    }
    else {
      fCurrentTrack -> Fit();
    }
  }


  fNumCandHits = fCandHits -> GetEntriesFast();
  if (fNumCandHits==0)
    return kStepInitTrack;

  return kStepInitTrackAddHit;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int LAPTrackFindingTask::StepContinuum()
{ 
  fPadPlane -> PullOutNeighborHits(fGoodHits, fCandHits, fFirstNeighborRange);
  fGoodHits -> MoveHitsTo(fTrackHits);

  fNumCandHits = fCandHits -> GetEntries();
  if (fNumCandHits==0)
    return kStepExtrapolation;


  fCandHits -> SortByCharge(false);
  return kStepContinuumAddHit;
}

int LAPTrackFindingTask::StepContinuumAddHit()
{
  for (Int_t iHit = 0; iHit < fNumCandHits; iHit++) {
    KBTpcHit *candHit = (KBTpcHit *) fCandHits -> GetLastHit();
    fCandHits -> RemoveLastHit();

    Double_t quality = 0; 
    if (!(candHit->IsTrackHit()||candHit->IsTrackHitCand()))
      quality = CorrelateHitWithTrack(fCurrentTrack, candHit);

    if (quality > 0) {
      fGoodHits -> AddHit(candHit);
      fCurrentTrack -> AddHit(candHit);
      //candHit -> SetTrackID(fCurrentTrack->GetTrackID());
      fCurrentTrack -> Fit();
    } else {
      fBadHits -> AddHit(candHit);
    }
  }

  return kStepContinuum;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int LAPTrackFindingTask::StepExtrapolation()
{ 
  ReturnBadHitsToPadPlane();

  return kStepExtrapolationAddHit;
}

int LAPTrackFindingTask::StepExtrapolationAddHit()
{
  Int_t countIteration = 0;
  bool buildFromHeadOrTail = true;
  Double_t extrapolationLength = 0;
  while (AutoBuildByExtrapolation(fCurrentTrack, buildFromHeadOrTail, extrapolationLength)) {
    if (++countIteration > 200)
      break;
  }

  countIteration = 0;
  buildFromHeadOrTail = !buildFromHeadOrTail;
  extrapolationLength = 0;
  while (AutoBuildByExtrapolation(fCurrentTrack, buildFromHeadOrTail, extrapolationLength)) {
    if (++countIteration > 200)
      break;
  }

  ReturnBadHitsToPadPlane();

  bool isGood = CheckTrackQuality(fCurrentTrack);

  if (isGood) {
    return kStepConfirmation;
  }


  return kStepRemoveTrack;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int LAPTrackFindingTask::StepConfirmation()
{
  bool tailToHead = false;
  KBVector3 pTail(fCurrentTrack -> PositionAtTail(), fReferenceAxis);
  KBVector3 pHead(fCurrentTrack -> PositionAtHead(), fReferenceAxis);
  if (pHead.K() > pHead.K()) tailToHead = true;

  ReturnBadHitsToPadPlane();
  if (BuildAndConfirmTrack(fCurrentTrack, tailToHead) == false)
    return kStepRemoveTrack;

  tailToHead = !tailToHead; 

  ReturnBadHitsToPadPlane();
  if (BuildAndConfirmTrack(fCurrentTrack, tailToHead) == false)
    return kStepRemoveTrack;

  ReturnBadHitsToPadPlane();

  return kStepFinalizeTrack;
}


int LAPTrackFindingTask::StepFinalizeTrack()
{
  auto trackHits = fCurrentTrack -> GetHitArray();
  Int_t trackID = fCurrentTrack -> GetTrackID();
  Int_t numTrackHits = trackHits -> GetNumHits();
  for (Int_t iTrackHit = 0; iTrackHit < numTrackHits; ++iTrackHit) {
    auto trackHit = (KBTpcHit *) trackHits -> GetHit(iTrackHit);
    trackHit -> SetTrackHitCand(trackID);
    fPadPlane -> AddHit(trackHit);
  }

  fGoodHits -> MoveHitsTo(fTrackHits);
  fGoodHits -> Clear();

  return kStepNewTrack;
}

int LAPTrackFindingTask::StepNextPhase()
{
  if (fAngleDivisionIndex<fNumAngleDivision-1) {
    fAngleDivisionIndex++;

    fPadPlane -> ResetEvent();

    fTrackHits -> Clear();
    fCandHits -> Clear();
    fGoodHits -> Clear();
    ReturnBadHitsToPadPlane();

    Int_t numHits2 = fHitArray -> GetEntriesFast();
    for (Int_t iHit = 0; iHit < numHits2; ++iHit) {
      KBTpcHit *hit = (KBTpcHit *) fHitArray -> At(iHit);
      if (!(hit->IsTrackHit()||hit->IsTrackHitCand()))
        hit -> SetFreeHit();
    }

    return kStepNewTrack;
  }

  /*
  if (fFirstNeighborRange<2)
  {
    fFirstNeighborRange++;
    fAngleDivisionIndex = 0;

    fPadPlane -> ResetEvent();

    fTrackHits -> Clear();
    fCandHits -> Clear();
    fGoodHits -> Clear();
    ReturnBadHitsToPadPlane();

    //??
    //Int_t numHits2 = fHitArray -> GetEntriesFast();
    //for (Int_t iHit = 0; iHit < numHits2; ++iHit) {
      //KBHit *hit = (KBHit *) fHitArray -> At(iHit);
      //hit -> ClearTrackCands();
    //}

    return kStepNewTrack;
  }
  */


  return kStepEndEvent;
}

int LAPTrackFindingTask::StepEndEvent()
{
  fTrackArray -> Compress();

  Int_t numTracks = fTrackArray -> GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < numTracks; ++iTrack) {
    KBLinearTrack *track = (KBLinearTrack *) fTrackArray -> At(iTrack);
    track -> SetTrackID(iTrack);
    track -> FinalizeHits();
  }

  kb_info << fTrackArray -> GetEntriesFast() << " linear tracks found from " << fHitArray -> GetEntriesFast() << " hits " << endl;

  return kStepEndOfEvent;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void LAPTrackFindingTask::ReturnBadHitsToPadPlane()
{
  Int_t numBadHits = fBadHits -> GetEntriesFast();
  for (Int_t iBad = 0; iBad < numBadHits; ++iBad) {
    auto hit = (KBTpcHit *) fBadHits -> GetHit(iBad);
    fPadPlane -> AddHit(hit);
    //hit -> ClearTrackCands();
  }
  fBadHits -> Clear();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

double LAPTrackFindingTask::CorrelateHitWithTrack(KBLinearTrack *track, KBTpcHit *hit, bool initTrack)
{
  double quality = 0;

  auto section = hit -> GetSection();
  auto dist = track -> DistanceToLine(hit->GetPosition());

  double minAngle = fAngleDivisionArray[fAngleDivisionIndex];
  double maxAngle = fAngleDivisionArray[fAngleDivisionIndex+1];

  auto trackWCutLL = fMinGeomDistanceInPlane[section] / TMath::Cos(TMath::DegToRad()*minAngle);
  auto trackWCutHL = fMaxGeomDistanceInPlane[section] / TMath::Cos(TMath::DegToRad()*maxAngle);
  auto trackWCutML = fMaxGeomDistanceInPlane[section] / TMath::Cos(TMath::DegToRad()*30);

  //if (dist > trackWCutLL && dist < trackWCutHL)
  if (initTrack) {
    if (dist < trackWCutHL)
      quality = 1;
  }
  else {
    if (dist < trackWCutML)
      quality = 1;
  }

#ifdef DEBUG_CORR
  kb_debug << "i-" << right << setw(2) << hit -> GetHitID() << ": angle = " << minAngle << " ~ " << maxAngle << ", dist = " << dist << " <? " << trackWCutHL << "(" << fMaxGeomDistanceInPlane[section] << "/" <<  TMath::Cos(TMath::DegToRad()*maxAngle) << "), quality = " << quality << endl;
#endif

  return quality;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool LAPTrackFindingTask::BuildAndConfirmTrack(KBLinearTrack *track, bool &tailToHead)
{
  track -> SortHits(!tailToHead);
  auto trackHits = track -> GetHitArray();
  Int_t numHits = trackHits -> GetNumHits();

  TVector3 q, m;

  Double_t extrapolationLength = 10.;
  for (Int_t iHit = 1; iHit < numHits; iHit++) 
  {
    auto trackHit = (KBTpcHit *) trackHits -> GetLastHit();

    Double_t quality = CorrelateHitWithTrack(track, trackHit);

    if (quality <= 0) {
      track -> RemoveHit(trackHit);
      trackHit -> RemoveTrackCand(trackHit -> GetTrackID());
      track -> Fit();
      fPadPlane -> AddHit(trackHit);
    }
  }

  extrapolationLength = 0;
  while (AutoBuildByExtrapolation(track, tailToHead, extrapolationLength)) {}

  if (track -> GetNumHits() < fCutMinNumHitsFinalTrack)
    return false;

  return true;
}

bool LAPTrackFindingTask::AutoBuildByExtrapolation(KBLinearTrack *track, bool &buildHead, Double_t &extrapolationLength)
{
  if (track -> GetNumHits() < fCutMinNumHitsFinalTrack)
    return false;

  TVector3 p;
  if (buildHead) p = track -> ExtrapolateHead(extrapolationLength);
  else           p = track -> ExtrapolateTail(extrapolationLength);

  return AutoBuildAtPosition(track, p, buildHead, extrapolationLength);
}

bool LAPTrackFindingTask::AutoBuildAtPosition(KBLinearTrack *track, TVector3 p, bool &tailToHead, Double_t &extrapolationLength, Double_t rScale)
{
  KBVector3 p2(p,fReferenceAxis);
  if (fPadPlane -> IsInBoundary(p2.I(), p2.J()) == false)
    return false;

  Double_t rms = 3*track -> GetRMS();
  if (rms < 25) 
    rms = 25;

  Int_t range = Int_t(rms/8);
  fPadPlane -> PullOutNeighborHits(p2.I(), p2.J(), range, fCandHits);

  fNumCandHits = fCandHits -> GetEntriesFast();
  Bool_t foundHit = false;

  if (fNumCandHits != 0) 
  {
    fCandHits -> SortByCharge(false);

    for (Int_t iHit = 0; iHit < fNumCandHits; iHit++) {
      KBTpcHit *candHit = (KBTpcHit *) fCandHits -> GetLastHit();
      fCandHits -> RemoveLastHit();
      TVector3 pos = candHit -> GetPosition();

      Double_t quality = 0; 
      if (!(candHit->IsTrackHit()||candHit->IsTrackHitCand()))
        quality = CorrelateHitWithTrack(track, candHit);

      if (quality > 0) {
        track -> AddHit(candHit);
        //candHit -> SetTrackID(fCurrentTrack->GetTrackID());
        track -> Fit();
        foundHit = true;
      } else
        fBadHits -> AddHit(candHit);
    }
  }

  if (foundHit) {
    extrapolationLength = 10; 
  }
  else {
    extrapolationLength += 10; 
    if (extrapolationLength > 3 * track -> TrackLength()) {
      return false;
    }
  }

  return true;
}

bool LAPTrackFindingTask::CheckTrackQuality(KBLinearTrack *track)
{
  // TODO
  return true;
}

double LAPTrackFindingTask::CheckTrackContinuity(KBLinearTrack *track)
{
  // TODO
  return 1;
}
