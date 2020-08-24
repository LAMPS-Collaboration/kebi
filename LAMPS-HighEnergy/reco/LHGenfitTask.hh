#ifndef LHGENFITTASK_HH
#define LHGENFITTASK_HH

#include "KBTask.hh"

#include "TClonesArray.h"

#include "KalmanFitterRefTrack.h"

class LHGenfitTask : public KBTask
{ 
  public:
    LHGenfitTask();
    virtual ~LHGenfitTask() {}

    bool Init();
    void Exec(Option_t*);

  private:
    TClonesArray* fTrackArray = nullptr;

    genfit::KalmanFitterRefTrack *fKalmanFitter;

  ClassDef(LHGenfitTask, 1)
};

#endif
