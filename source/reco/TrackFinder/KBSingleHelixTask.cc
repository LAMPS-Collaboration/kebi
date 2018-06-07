#include "KBSingleHelixTask.hh"

#include "KBRun.hh"

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

  fTrackArray = new TClonesArray("KBHelixTrack");
  run -> RegisterBranch("Tracklet", fTrackArray, fPersistency);

  fFitter = new KBHelixTrackFitter();
  fFitter -> SetReferenceAxis(fReferenceAxis);

  return true;
}

void KBSingleHelixTask::Exec(Option_t*)
{
  fTrackArray -> Clear("C");

  KBHelixTrack *track = (KBHelixTrack *) fTrackArray -> ConstructedAt(0);
  track -> SetReferenceAxis(fReferenceAxis);

  Int_t numHits = fHitArray -> GetEntries();
  for (auto iHit = 0; iHit < numHits; ++iHit) {
    auto hit = (KBHit *) fHitArray -> At(iHit);
    track -> AddHit(hit);
  }

  fFitter -> Fit(track);
  track -> FinalizeHits();

  kb_info << "All hits are used to make helix track!" << endl;

  return;
}

void KBSingleHelixTask::SetTrackPersistency(bool val) { fPersistency = val; }
