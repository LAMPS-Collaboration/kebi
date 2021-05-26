#ifndef LAPTRACKFINDINGTASK_HH
#define LAPTRACKFINDINGTASK_HH

#include "TClonesArray.h"
#include "TGraphErrors.h"

#include "KBTask.hh"
#include "KBLinearTrack.hh"
#include "KBTpcHit.hh"
#include "KBHitArray.hh"

#include "KBTpc.hh"
#include "KBPadPlane.hh"

#include <vector>
using namespace std;

class LAPTrackFindingTask : public KBTask
{
  public:
    LAPTrackFindingTask() : KBTask("LAPTrackFindingTask","LAPTrackFindingTask") {}
    virtual ~LAPTrackFindingTask() {}

    virtual bool Init();
    virtual void Exec(Option_t*);

    void SetTrackPersistency(bool val) { fPersistency = val; }

    enum StepNo : int {
      kStepInitArray,
      kStepNewTrack,
      kStepRemoveTrack,
      kStepInitTrack,
      kStepInitTrackAddHit,
      kStepContinuum,
      kStepContinuumAddHit,
      kStepExtrapolation,
      kStepExtrapolationAddHit,
      kStepConfirmation,
      kStepFinalizeTrack,
      kStepNextPhase,
      kStepEndEvent,
      kStepEndOfEvent,
    };

    virtual bool ExecStep(Option_t *opt="");

    bool ExecStepUptoTrackNum(Int_t numTracks);

    KBLinearTrack *GetCurrentTrack() const { return fCurrentTrack; }

    void SetHitBranchName(TString name) { fBranchNameHit = name; }
    void SetTrackletBranchName(TString name) { fBranchNameTracklet = name; }

  private:
    int StepInitArray();
    int StepNewTrack();
    int StepRemoveTrack();
    int StepInitTrack();
    int StepInitTrackAddHit();
    int StepContinuum();
    int StepContinuumAddHit();
    int StepExtrapolation();
    int StepExtrapolationAddHit();
    int StepConfirmation();
    int StepFinalizeTrack();
    int StepNextPhase();
    int StepEndEvent();

    void ReturnBadHitsToPadPlane();

    double CorrelateHitWithTrack(KBLinearTrack *track, KBTpcHit *hit, bool initTrack=false);

    bool CheckTrackQuality(KBLinearTrack *track);
    double CheckTrackContinuity(KBLinearTrack *track);

    bool BuildAndConfirmTrack(KBLinearTrack *track, bool &tailToHead);
    bool AutoBuildByExtrapolation(KBLinearTrack *track, bool &buildHead, Double_t &extrapolationLength);
    bool AutoBuildAtPosition(KBLinearTrack *track, TVector3 p, bool &tailToHead, Double_t &extrapolationLength, Double_t scale=1);

  private:
    KBTpc *fTpc = nullptr;
    KBPadPlane *fPadPlane = nullptr;
    TClonesArray *fHitArray = nullptr;
    TClonesArray *fTrackArray = nullptr;

    TString fBranchNameHit = "Hit";
    TString fBranchNameTracklet = "Tracklet";

    bool fPersistency = true;

    KBHitArray *fTrackHits = nullptr;
    KBHitArray *fCandHits = nullptr;
    KBHitArray *fGoodHits = nullptr;
    KBHitArray *fBadHits  = nullptr;

    KBVector3::Axis fReferenceAxis = KBVector3::kY;

    Double_t fFirstNeighborRange = 1;

    Int_t fNumAngleDivision = 2;
    Int_t fAngleDivisionIndex = 0;
    vector<Double_t> fAngleDivisionArray = {0,1};

    Int_t fMinHitsToFitInitTrack = 7; ///< try track fit if track has more than this number of hits in track
    Int_t fCutMinNumHitsFinalTrack = 15; ///< remove track if track has smaller than this number of hits within initialization stage
    Double_t fTrackLengthCutScale = 2.5; ///< track length cut for initialization stage is [this_var] * track -> GetRMSR()

    KBLinearTrack *fCurrentTrack = nullptr;

    Double_t fMinGeomDistanceInPlane[4] = {0};
    Double_t fMaxGeomDistanceInPlane[4] = {0};

    Int_t fNextStep = StepNo::kStepInitArray;
    Int_t fNumCandHits;

    //TCanvas *fCvsCurrentTrack = nullptr;
    //TGraphErrors *fGraphCurrentTrackPoint = nullptr;

  ClassDef(LAPTrackFindingTask, 1)
};

#endif
