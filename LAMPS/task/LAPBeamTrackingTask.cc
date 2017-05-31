#include "KBRun.hh"
#include "LAPBeamTrackingTask.hh"
#include "KBLinearTrack.hh"
#include "KBHitList.hh"

#include <iostream>
using namespace std;

ClassImp(LAPBeamTrackingTask)

LAPBeamTrackingTask::LAPBeamTrackingTask()
:KBTask("LAPBeamTrackingTask","")
{
} 

bool LAPBeamTrackingTask::Init()
{
  auto run = KBRun::GetRun();

  fHitArray = (TClonesArray *) run -> GetBranch("Hit");

  fHitClusterArray = new TClonesArray("KBHit");
  fLinearTrackArray = new TClonesArray("KBLinearTrack");
  fHitListArray = new TClonesArray("KBHitList");

  run -> RegisterBranch("HitCluster", fHitClusterArray, fHitClusterPersistency);
  run -> RegisterBranch("Beam", fLinearTrackArray, fLinearTrackPersistency);
  run -> RegisterBranch("BeamHitList", fHitListArray, fHitListPersistency);

  fODRFitter = new KBODRFitter();

  return true;
}

void LAPBeamTrackingTask::Exec(Option_t*)
{
  fLinearTrackArray -> Delete();
  fHitListArray -> Delete();

  fODRFitter -> Reset();

  auto hitList = new ((*fHitListArray)[0]) KBHitList();

  Int_t nHits = fHitArray -> GetEntries();
  if (nHits < 4)
    return;

  for (auto iHit = 0; iHit < nHits; iHit++) {
    auto hit = (KBHit *) fHitArray -> At(iHit);
    hitList -> AddHit(hit);
  }

  auto hitArray = hitList -> GetHitArray();
  Double_t xMean = 0, yMean = 0, zMean = 0, chargeSum = 0;
  for (auto hit : *hitArray) {
    xMean += hit -> GetCharge() * hit -> GetX(); 
    yMean += hit -> GetCharge() * hit -> GetY(); 
    zMean += hit -> GetCharge() * hit -> GetZ(); 
    chargeSum += hit -> GetCharge();
  }

  xMean = xMean / chargeSum;
  yMean = yMean / chargeSum;
  zMean = zMean / chargeSum;

  fODRFitter -> SetCentroid(xMean, yMean, zMean);
  for (auto hit : *hitArray)
    fODRFitter -> AddPoint(hit -> GetX(), hit -> GetY(), hit -> GetZ(), hit -> GetCharge());

  fODRFitter -> FitLine();
  auto centroid = fODRFitter -> GetCentroid();
  auto direction = fODRFitter -> GetDirection();

  auto beamTrack = new ((*fLinearTrackArray)[0]) KBLinearTrack(centroid, centroid+direction);

  Int_t idxMin = -1;
  Int_t idxMax = -1;
  Double_t lengthMin = DBL_MAX;
  Double_t lengthMax = 0;

  Int_t nHitsBeam = hitArray -> size();
  for (auto iHit = 0; iHit < nHitsBeam; iHit++) {
    auto hit = hitArray -> at(iHit);
    auto length = beamTrack -> Length(hit -> GetPosition());
    if (length > lengthMax) {
      lengthMax = length;
      idxMax = iHit;
    }
    if (length < lengthMin) {
      lengthMin = length;
      idxMin = iHit;
    }
  }

  auto hitMin = hitArray -> at(idxMin);
  TVector3 positionMin = beamTrack -> ClosestPointOnLine(hitMin -> GetPosition());

  auto hitMax = hitArray -> at(idxMax);
  TVector3 positionMax = beamTrack -> ClosestPointOnLine(hitMax -> GetPosition());

  beamTrack -> SetLine(positionMin, positionMax);

  cout << "  [" << this -> GetName() << "]" << endl;
  
  return;
}

void LAPBeamTrackingTask::SetHitClusterPersistency(bool persistency) { fHitClusterPersistency = persistency; }
void LAPBeamTrackingTask::SetLinearTrackPersistency(bool persistency) { fLinearTrackPersistency = persistency; }
void LAPBeamTrackingTask::SetHitListPersistency(bool persistency) { fHitListPersistency = persistency; }
