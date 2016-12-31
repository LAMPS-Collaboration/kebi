#ifndef KBPSATASK_HH
#define KBPSATASK_HH

#include "KBTask.hh"
#include "KBPSA.hh"
#include "KBPadPlane.hh"

#include "TClonesArray.h"

class KBPSATask : public KBTask
{ 
  public:
    KBPSATask();
    virtual ~KBPSATask() {};

    bool Init();
    void Exec(Option_t*);

  private:
    TClonesArray* fPadArray;
    TClonesArray* fHitArray;

    Int_t fNPlanes;
    Int_t fTbStart;
    Int_t fNTbs;
    Double_t fDriftVelocity;
    Double_t fTbTime;
    Double_t fADCThreshold;

    KBPadPlane *fPadPlane[2];

    KBPSA *fPSA;

  ClassDef(KBPSATask, 1)
};

#endif
