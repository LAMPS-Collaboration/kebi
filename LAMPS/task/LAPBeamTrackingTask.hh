#ifndef LAMPSPROTOTYPEBEAMTRACKINGTASK_HH
#define LAMPSPROTOTYPEBEAMTRACKINGTASK_HH

#include "KBTask.hh"
#include "KBODRFitter.hh"
#include "TClonesArray.h"

class LAPBeamTrackingTask : public KBTask
{ 
  public:
    LAPBeamTrackingTask();
    virtual ~LAPBeamTrackingTask() {}

    bool Init();
    void Exec(Option_t*);

    void SetHitClusterPersistency(bool persistency);
    void SetLinearTrackPersistency(bool persistency);
    void SetHitListPersistency(bool persistency);

    void SetBeamTbRegion(Double_t tb1, Double_t tb2);
    void SetHitChargeThreshold(Double_t val);

  private:
    TClonesArray* fHitArray; // only input
    TClonesArray* fHitListArray;
    TClonesArray* fHitClusterArray;
    TClonesArray* fHitClusterListArray;
    TClonesArray* fBeamTrackArray;

    bool fHitClusterPersistency = true;
    bool fLinearTrackPersistency = true;
    bool fHitListPersistency = true;

    KBODRFitter *fODRFitter;

    Double_t fTb1 = 0;
    Double_t fTb2 = 511;
    Double_t fHitChargeThreshold = 5000;

  ClassDef(LAPBeamTrackingTask, 1)
};

#endif
