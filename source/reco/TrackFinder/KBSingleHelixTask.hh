#ifndef KBSINGLEHELIXTASK_HH
#define KBSINGLEHELIXTASK_HH

#include "KBTask.hh"
#include "KBHelixTrack.hh"
#include "TClonesArray.h"
#include "KBVector3.hh"

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

    bool fPersistency = true;

    KBVector3::Axis fReferenceAxis;

  ClassDef(KBSingleHelixTask, 1)
};

#endif
