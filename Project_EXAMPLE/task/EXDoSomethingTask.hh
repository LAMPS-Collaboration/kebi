#ifndef EXDOSOMETHINGTASK_HH
#define EXDOSOMETHINGTASK_HH

#include "KBTask.hh"
#include "TClonesArray.h"

class EXDoSomethingTask : public KBTask
{ 
  public:
    EXDoSomethingTask();
    virtual ~EXDoSomethingTask() {};

    bool Init();
    void Exec(Option_t*);

  private:
    TClonesArray* fChannelArray;

  ClassDef(EXDoSomethingTask, 1)
};

#endif
