#ifndef LAVERTEXFINDINGTASK_HH
#define LAVERTEXFINDINGTASK_HH

#include "KBTask.hh"
#include "KBHelixTrackFitter.hh"

#include "TClonesArray.h"

class LAVertexFindingTask : public KBTask
{ 
  public:
    LAVertexFindingTask();
    virtual ~LAVertexFindingTask() {}

    bool Init();
    void Exec(Option_t*);

    void SetVertexPersistency(bool val);

  private:
    TClonesArray* fTrackArray;
    TClonesArray* fVertexArray;

    KBHelixTrackFitter *fTrackFitter;

    bool fPersistency = true;

  ClassDef(LAVertexFindingTask, 1)
};

#endif
