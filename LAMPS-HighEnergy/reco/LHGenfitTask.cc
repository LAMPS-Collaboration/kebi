#include "LHGenfitTask.hh"

#include "KBRun.hh"
#include "KBHitArray.hh"

#include <iostream>
using namespace std;

#include "TMatrixDSym.h"

#include "FieldManager.h"
#include "ConstField.h"
#include "MaterialEffects.h"
#include "TGeoMaterialInterface.h"
#include "TrackCand.h"

ClassImp(LHGenfitTask)

LHGenfitTask::LHGenfitTask()
:KBTask("LHGenfitTask","")
{
}

bool LHGenfitTask::Init()
{
  auto run = KBRun::GetRun();
  fTrackArray = (TClonesArray *) run -> GetBranch("Tracklet");

  fGFTrackHitClusterArray = new TClonesArray("KBHit");
  fGenfitTrackArray = new TClonesArray("genfit::Track");



  fKalmanFitter = new genfit::KalmanFitterRefTrack();
  fKalmanFitter -> setMinIterations(5);
  fKalmanFitter -> setMaxIterations(20);

  fMeasurementProducer = new genfit::MeasurementProducer<KBHit, genfit::LHSpacepointMeasurement>(fGFTrackHitClusterArray);
  fMeasurementFactory = new genfit::MeasurementFactory<genfit::AbsMeasurement>();
  fMeasurementFactory -> addProducer(fDetectorID, fMeasurementProducer);
  genfit::FieldManager::getInstance() -> init(new genfit::ConstField(0., 5., 0.));

  genfit::MaterialEffects *materialEffects = genfit::MaterialEffects::getInstance();
  materialEffects -> init(new genfit::TGeoMaterialInterface());

  TVector3 posTarget(0, 0, 0); // cm
  TVector3 normalTarget(0, 0, 1);
  fTargetPlane = genfit::SharedPlanePtr(new genfit::DetPlane(posTarget, normalTarget));

  return true;
}

void LHGenfitTask::Exec(Option_t*)
{
  Int_t numTracks = fTrackArray -> GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < numTracks; iTrack++)
  {
    KBHelixTrack *track = (KBHelixTrack *) fTrackArray -> At(iTrack);
    auto genfitTrack = FitTrack(track, 2212);
    track -> SetGenfitMomentum(fCurrentMomTargetPlane);
  }

  kb_info << "..." << endl;

  return;
}

genfit::Track* LHGenfitTask::FitTrack(KBHelixTrack *helixTrack, Int_t pdg)
{
  fGFTrackHitClusterArray -> Clear("C");
  genfit::TrackCand trackCand;

  KBHitArray *hitArray = helixTrack -> GetHitArray();
  TIter nextHit(hitArray);

  while (KBHit *hit = (KBHit *) nextHit()) {
    auto idx = fGFTrackHitClusterArray -> GetEntriesFast();
    auto gfhit = (KBHit *) fGFTrackHitClusterArray -> ConstructedAt(idx);
    gfhit -> CopyFrom(hit);
    trackCand.addHit(fDetectorID, idx);
  }
  auto refHit = (KBHit *) fGFTrackHitClusterArray -> At(0);

  TMatrixDSym covSeed(6);
  covSeed(0,0) = refHit -> GetVariance().X() / 100.;
  covSeed(1,1) = refHit -> GetVariance().X() / 100.;
  covSeed(2,2) = refHit -> GetVariance().Z() / 100.;
  covSeed(3,3) = refHit -> GetVariance().X() / 100.;
  covSeed(4,4) = refHit -> GetVariance().X() / 100.;
  covSeed(5,5) = refHit -> GetVariance().Z() / 100.;

  Double_t dip = helixTrack -> DipAngle();
  TVector3 momSeed = 0.001 * helixTrack -> Momentum(); // MeV -> GeV
  momSeed.SetTheta(dip); /// TODO

  trackCand.setCovSeed(covSeed);
  trackCand.setPosMomSeed(refHit->GetPosition(), momSeed, KBRun::GetRun()->GetParticle(pdg)->Charge()/3.); /// TODO

  auto genfitTrack = (genfit::Track *) fGenfitTrackArray -> ConstructedAt(fGenfitTrackArray -> GetEntriesFast());
  genfitTrack -> createMeasurements(trackCand, *fMeasurementFactory);
  fCurrentTrackRep = new genfit::RKTrackRep(pdg);
  genfitTrack -> addTrackRep(fCurrentTrackRep);

  try { fKalmanFitter -> processTrackWithRep(genfitTrack, fCurrentTrackRep, false); }
  catch (genfit::Exception &e) { return (genfit::Track *) nullptr; } /// TODO
  
  try { fCurrentFitStatus = genfitTrack -> getFitStatus(fCurrentTrackRep); }
  catch (genfit::Exception &e) { return (genfit::Track *) nullptr; } /// TODO

  if (fCurrentFitStatus -> isFitted() == false || fCurrentFitStatus -> isFitConverged() == false)
    return (genfit::Track *) nullptr;

  try { fCurrentFitState = genfitTrack -> getFittedState(); }
  catch (genfit::Exception &e) { (genfit::Track *) nullptr; } /// TODO

  TVector3 fCurrentMomTargetPlane;
  TVector3 fCurrentPosTargetPlane;
  try { 
    fCurrentTrackRep -> extrapolateToPlane(fCurrentFitState, fTargetPlane); 
    fCurrentMomTargetPlane = fCurrentFitState.getMom();
    fCurrentPosTargetPlane = fCurrentFitState.getPos();
  } catch (genfit::Exception &e) {
    fCurrentMomTargetPlane.SetXYZ(0, 0, 0);
    fCurrentPosTargetPlane.SetXYZ(0, 0, 0);
  }

  return genfitTrack;
}
