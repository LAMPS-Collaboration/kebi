#ifndef LATRACKFINDINGTASK_HH
#define LATRACKFINDINGTASK_HH

#include "KBTask.hh"
#include "LATrackFinder.hh"
#include "TClonesArray.h"

class LATrackFindingTask : public KBTask
{ 
  public:
    LATrackFindingTask();
    virtual ~LATrackFindingTask() {}

    bool Init();
    void Exec(Option_t*);

    void SetTrackPersistency(bool val);

  private:
    TClonesArray* fHitArray;
    TClonesArray* fTrackArray;

    bool fPersistency = true;

    LATrackFinder *fTrackFinder = nullptr;

  ClassDef(LATrackFindingTask, 1)
};

#endif
