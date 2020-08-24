#include "LHGenfitTask.hh"

#include "KBRun.hh"

#include <iostream>
using namespace std;

ClassImp(LHGenfitTask)

LHGenfitTask::LHGenfitTask()
:KBTask("LHGenfitTask","")
{
}

bool LHGenfitTask::Init()
{
  auto run = KBRun::GetRun();
  fTrackArray = (TClonesArray *) run -> GetBranch("Tracklet");

  fKalmanFitter = new genfit::KalmanFitterRefTrack();

  return true;
}

void LHGenfitTask::Exec(Option_t*)
{
 return;
}
