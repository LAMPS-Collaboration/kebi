#include "LATrackFindingTask.hh"

#include "KBRun.hh"

#include <iostream>
using namespace std;

ClassImp(LATrackFindingTask)

LATrackFindingTask::LATrackFindingTask()
:KBTask("LATrackFindingTask","")
{
}

bool LATrackFindingTask::Init()
{
  KBRun *run = KBRun::GetRun();
  KBParameterContainer *par = run -> GetParameterContainer();

  fHitArray = (TClonesArray *) run -> GetBranch("Hit");

  fTrackArray = new TClonesArray("KBHelixTrack");
  run -> RegisterBranch("Tracklet", fTrackArray, fPersistency);

  fTrackFinder = new LATrackFinder();
  fTrackFinder -> SetParameterContainer(par);
  LATpc *tpc = (LATpc *) run -> GetDetector();
  fTrackFinder -> SetTpc(tpc);
  fTrackFinder -> Init();

  return true;
}

void LATrackFindingTask::Exec(Option_t*)
{
  fTrackArray -> Delete();

  fTrackFinder -> FindTrack(fHitArray, fTrackArray);

  cout << "  [" << this -> GetName() << "] Number of found tracks: " << fTrackArray -> GetEntriesFast() << endl;

  return;
}

void LATrackFindingTask::SetTrackPersistency(bool val) { fPersistency = val; }
