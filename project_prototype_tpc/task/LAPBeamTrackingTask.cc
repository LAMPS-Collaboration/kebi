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
  fBeamTrackArray = new TClonesArray("KBLinearTrack");
  fHitListArray = new TClonesArray("KBHitList");
  fHitClusterListArray = new TClonesArray("KBHitList");

  run -> RegisterBranch("HitCluster", fHitClusterArray, fHitClusterPersistency);
  run -> RegisterBranch("Beam", fBeamTrackArray, fLinearTrackPersistency);
  run -> RegisterBranch("BeamHitList", fHitListArray, fHitListPersistency);
  run -> RegisterBranch("BeamHitClusterList", fHitClusterListArray, fHitListPersistency);

  fODRFitter = new KBODRFitter();

  return true;
}

void LAPBeamTrackingTask::Exec(Option_t*)
{
  fBeamTrackArray -> Delete();
  fHitListArray -> Delete();
  fHitClusterListArray -> Delete();
  fHitClusterArray -> Delete();

  fODRFitter -> Reset();

  auto hitList = new ((*fHitListArray)[0]) KBHitList();
  auto hitClusterList = new ((*fHitClusterListArray)[0]) KBHitList();

  Int_t nHits = fHitArray -> GetEntries();
  if (nHits < 4) {
    cout << "  [" << this -> GetName() << "] no beam track" << endl;
    return;
  }

  for (auto iHit = 0; iHit < nHits; iHit++) {
    auto hit = (KBHit *) fHitArray -> At(iHit);
    auto tb = hit -> GetTb();
    if (tb < fTb1 || tb > fTb2 || hit -> GetCharge() > fHitChargeThreshold)
      continue;
    hitList -> AddHit(hit);
  }

  Int_t numSections = 2;
  Int_t numLayers = 7;

  for (auto iSection = 0; iSection < numSections; ++iSection) {
    for (auto iLayer = 0; iLayer < numLayers; ++iLayer) {
      auto cluster = (KBHit *) fHitClusterArray -> ConstructedAt(fHitClusterArray -> GetEntriesFast());
      cluster -> SetSection(iSection);
      cluster -> SetLayer(iLayer);
    }
  }

  auto hitArrayFromList = hitList -> GetHitArray();
  for (auto hit : *hitArrayFromList) {
    auto idxCluster = (hit -> GetSection() * numLayers) + hit -> GetLayer();
    auto cluster = (KBHit *) fHitClusterArray -> At(idxCluster);
    cluster -> AddHit(hit);
  }

  Int_t numClusters = numLayers * numSections;
  for (auto iCluster = 0; iCluster < numClusters; ++iCluster) {
    auto cluster = (KBHit *) fHitClusterArray -> At(iCluster);
    if (cluster -> GetCharge() <= 0)
      fHitClusterArray -> Remove(cluster);
    else
      hitClusterList -> AddHit(cluster);
  }
  fHitClusterArray -> Compress();
  if (fHitClusterArray -> GetEntries() < 4) {
    cout << "  [" << this -> GetName() << "] no beam track" << endl;
    return;
  }

  Double_t xMean = 0, yMean = 0, zMean = 0, chargeSum = 0;
  auto hitClusterArrayFromList = hitClusterList -> GetHitArray();
  for (auto cluster : *hitClusterArrayFromList) {
    xMean += cluster -> GetCharge() * cluster -> GetX();
    yMean += cluster -> GetCharge() * cluster -> GetY();
    zMean += cluster -> GetCharge() * cluster -> GetZ();
    chargeSum += cluster -> GetCharge();
  }

  xMean = xMean / chargeSum;
  yMean = yMean / chargeSum;
  zMean = zMean / chargeSum;

  fODRFitter -> SetCentroid(xMean, yMean, zMean);
  for (auto cluster : *hitClusterArrayFromList)
    fODRFitter -> AddPoint(cluster -> GetX(), cluster -> GetY(), cluster -> GetZ(), cluster -> GetCharge());

  fODRFitter -> FitLine();
  auto centroid = fODRFitter -> GetCentroid();
  auto direction = fODRFitter -> GetDirection();

  auto beamTrack = new ((*fBeamTrackArray)[0]) KBLinearTrack(centroid, centroid+direction);

  Int_t idxMin = -1;
  Int_t idxMax = -1;
  Double_t lengthMin = DBL_MAX;
  Double_t lengthMax = 0;
  Double_t quality = 0;

  numClusters = hitClusterArrayFromList -> size();
  for (auto iCluster = 0; iCluster < numClusters; iCluster++) {
    auto cluster = hitClusterArrayFromList -> at(iCluster);
    auto posHit = cluster -> GetPosition();

    auto lengthOnTrack = beamTrack -> Length(posHit);
    if (lengthOnTrack > lengthMax) {
      lengthMax = lengthOnTrack;
      idxMax = iCluster;
    }
    if (lengthOnTrack < lengthMin) {
      lengthMin = lengthOnTrack;
      idxMin = iCluster;
    }

    auto poca = beamTrack -> ClosestPointOnLine(posHit);
    auto toTrack = poca - posHit;

    cluster -> SetDX(toTrack.X());
    cluster -> SetDY(toTrack.Y());
    cluster -> SetDZ(toTrack.Z());

    quality += chargeSum*toTrack.Mag();
  }

  quality = quality/chargeSum;
  beamTrack -> SetQuality(quality);

  auto hitMin = hitClusterArrayFromList -> at(idxMin);
  TVector3 positionMin = beamTrack -> ClosestPointOnLine(hitMin -> GetPosition());

  auto hitMax = hitClusterArrayFromList -> at(idxMax);
  TVector3 positionMax = beamTrack -> ClosestPointOnLine(hitMax -> GetPosition());

  beamTrack -> SetLine(positionMin, positionMax);

  cout << "  [" << this -> GetName() << "] Beam track with quality " << quality << endl;
  
  return;
}

void LAPBeamTrackingTask::SetHitClusterPersistency(bool persistency) { fHitClusterPersistency = persistency; }
void LAPBeamTrackingTask::SetLinearTrackPersistency(bool persistency) { fLinearTrackPersistency = persistency; }
void LAPBeamTrackingTask::SetHitListPersistency(bool persistency) { fHitListPersistency = persistency; }

void LAPBeamTrackingTask::SetBeamTbRegion(Double_t tb1, Double_t tb2)
{
  fTb1 = tb1;
  fTb2 = tb2;
}

void LAPBeamTrackingTask::SetHitChargeThreshold(Double_t val) { fHitChargeThreshold = val; }