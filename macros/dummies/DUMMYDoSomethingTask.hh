#ifndef DUMMYDOSOMETHINGTASK_HH
#define DUMMYDOSOMETHINGTASK_HH

#include "KBTask.hh"
#include "TClonesArray.h"

class DUMMYDoSomethingTask : public KBTask
{ 
  public:
    DUMMYDoSomethingTask();
    virtual ~DUMMYDoSomethingTask() {};

    bool Init();
    void Exec(Option_t*);

  private:
    TClonesArray* fChannelArray;

  ClassDef(DUMMYDoSomethingTask, 1)
};

#endif
