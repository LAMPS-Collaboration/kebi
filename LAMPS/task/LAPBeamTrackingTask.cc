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
  KBRun *run = KBRun::GetRun();

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

  KBHitList *hitList = new ((*fHitListArray)[0]) KBHitList();

  Int_t nHits = fHitArray -> GetEntries();
  if (nHits < 4)
    return;

  for (Int_t iHit = 0; iHit < nHits; iHit++) {
    KBHit *hit = (KBHit *) fHitArray -> At(iHit);
    hitList -> AddHit(hit);
  }

  vector<KBHit*> *hitArray = hitList -> GetHitArray();
  Double_t xMean = 0, yMean = 0, zMean = 0, chargeSum = 0;
  for (UInt_t iHit = 0; iHit < hitArray -> size(); ++iHit) {
    KBHit *hit = hitArray -> at(iHit);
    xMean += hit -> GetCharge() * hit -> GetX(); 
    yMean += hit -> GetCharge() * hit -> GetY(); 
    zMean += hit -> GetCharge() * hit -> GetZ(); 
    chargeSum += hit -> GetCharge();
  }

  xMean = xMean / chargeSum;
  yMean = yMean / chargeSum;
  zMean = zMean / chargeSum;

  fODRFitter -> SetCentroid(xMean, yMean, zMean);
  for (UInt_t iHit = 0; iHit < hitArray -> size(); ++iHit) {
    KBHit *hit = hitArray -> at(iHit);
    fODRFitter -> AddPoint(hit -> GetX(), hit -> GetY(), hit -> GetZ(), hit -> GetCharge());
  }

  fODRFitter -> FitLine();
  TVector3 centroid = fODRFitter -> GetCentroid();
  TVector3 direction = fODRFitter -> GetDirection();

  KBLinearTrack *beamTrack = new ((*fLinearTrackArray)[0]) KBLinearTrack(centroid, centroid+direction);

  Int_t idxMin = -1;
  Int_t idxMax = -1;
  Double_t lengthMin = DBL_MAX;
  Double_t lengthMax = 0;

  Int_t nHitsBeam = hitArray -> size();
  for (Int_t iHit = 0; iHit < nHitsBeam; iHit++) {
    KBHit *hit = hitArray -> at(iHit);
    Double_t length = beamTrack -> Length(hit -> GetPosition());
    if (length > lengthMax) {
      lengthMax = length;
      idxMax = iHit;
    }
    if (length < lengthMin) {
      lengthMin = length;
      idxMin = iHit;
    }
  }

  KBHit *hitMin = hitArray -> at(idxMin);
  TVector3 positionMin = beamTrack -> ClosestPointOnLine(hitMin -> GetPosition());

  KBHit *hitMax = hitArray -> at(idxMax);
  TVector3 positionMax = beamTrack -> ClosestPointOnLine(hitMax -> GetPosition());

  beamTrack -> SetLine(positionMin, positionMax);

  cout << "  [" << this -> GetName() << "]" << endl;
  
  return;
}

void LAPBeamTrackingTask::SetHitClusterPersistency(bool persistency) { fHitClusterPersistency = persistency; }
void LAPBeamTrackingTask::SetLinearTrackPersistency(bool persistency) { fLinearTrackPersistency = persistency; }
void LAPBeamTrackingTask::SetHitListPersistency(bool persistency) { fHitListPersistency = persistency; }
