#ifndef LHGENFITTASK_HH
#define LHGENFITTASK_HH

#include "KBTask.hh"
#include "KBHelixTrack.hh"
#include "KBHit.hh"

#include "TClonesArray.h"

#include "LHSpacepointMeasurement.hh"
#include "KalmanFitterRefTrack.h"
#include "Track.h"
#include "RKTrackRep.h"
#include "MeasurementFactory.h"
#include "MeasurementProducer.h"

class LHGenfitTask : public KBTask
{ 
  public:
    LHGenfitTask();
    virtual ~LHGenfitTask() {}

    bool Init();
    void Exec(Option_t*);

    genfit::Track* FitTrack(KBHelixTrack *helixTrack, Int_t pdg);

  private:
    TClonesArray* fTrackArray = nullptr;

    TClonesArray *fGFTrackHitClusterArray;
    TClonesArray *fGenfitTrackArray;

    Int_t fDetectorID = 0;

    genfit::KalmanFitterRefTrack *fKalmanFitter;
    genfit::MeasurementProducer<KBHit, genfit::LHSpacepointMeasurement> *fMeasurementProducer;
    genfit::MeasurementFactory<genfit::AbsMeasurement> *fMeasurementFactory;
    genfit::SharedPlanePtr fTargetPlane;

    // FitTrack output
    genfit::RKTrackRep *fCurrentTrackRep;
    genfit::FitStatus *fCurrentFitStatus;
    genfit::MeasuredStateOnPlane fCurrentFitState;
    TVector3 fCurrentMomTargetPlane;
    TVector3 fCurrentPosTargetPlane;

  ClassDef(LHGenfitTask, 1)
};

#endif
