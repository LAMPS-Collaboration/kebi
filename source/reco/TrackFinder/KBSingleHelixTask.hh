#ifndef KBSINGLEHELIXTASK_HH
#define KBSINGLEHELIXTASK_HH

#include "KBTask.hh"
#include "KBHelixTrack.hh"
#include "KBHelixTrackFitter.hh"
#include "KBVector3.hh"
#include "KBHit.hh"
#include "TClonesArray.h"

class KBSingleHelixTask : public KBTask
{ 
  public:
    KBSingleHelixTask();
    virtual ~KBSingleHelixTask() {}

    bool Init();
    void Exec(Option_t*);

    void SetTrackPersistency(bool val);

  private:
    TClonesArray* fHitArray;
    TClonesArray* fTrackArray;

    KBHelixTrackFitter *fFitter = nullptr;

    bool fPersistency = true;

    KBVector3::Axis fReferenceAxis = KBVector3::kZ;

  ClassDef(KBSingleHelixTask, 1)
};

#endif
