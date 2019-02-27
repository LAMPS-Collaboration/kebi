#include "KBSingleHelixTask.hh"
#include "KBRun.hh"
#include "KBTpcHit.hh"
#include <iostream>
using namespace std;

ClassImp(KBSingleHelixTask)

KBSingleHelixTask::KBSingleHelixTask()
:KBTask("KBSingleHelixTask","")
{
}

bool KBSingleHelixTask::Init()
{
  KBRun *run = KBRun::GetRun();

  fHitArray = (TClonesArray *) run -> GetBranch("Hit");
  fReferenceAxis = run -> GetParameterContainer() -> GetParAxis("tpcBFieldAxis");

  fTrackArray = new TClonesArray("KBHelixTrack");
  run -> RegisterBranch("Tracklet", fTrackArray, fPersistency);

  return true;
}

void KBSingleHelixTask::Exec(Option_t*)
{
  fTrackArray -> Clear("C");

  KBHelixTrack *track = (KBHelixTrack *) fTrackArray -> ConstructedAt(0);
  track -> SetTrackID(0);
  track -> SetReferenceAxis(fReferenceAxis);

  Int_t numHits = fHitArray -> GetEntries();
  for (auto iHit=0; iHit<numHits; ++iHit) {
    auto hit = (KBTpcHit *) fHitArray -> At(iHit);
    track -> AddHit(hit);
  }
  track -> Fit();
  track -> FinalizeHits();

  track -> Print("s");

  return;
}

void KBSingleHelixTask::SetTrackPersistency(bool val) { fPersistency = val; }
