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

  private:
    TClonesArray* fHitArray;
    TClonesArray* fHitClusterArray;
    TClonesArray* fLinearTrackArray;
    TClonesArray* fHitListArray;

    bool fHitClusterPersistency = true;
    bool fLinearTrackPersistency = true;
    bool fHitListPersistency = true;

    KBODRFitter *fODRFitter;

  ClassDef(LAPBeamTrackingTask, 1)
};

#endif
