#ifndef LAELECTRONICSTASK_HH
#define LAELECTRONICSTASK_HH

#include "KBTask.hh"
#include "KBTpc.hh"
#include "KBPulseGenerator.hh"

#include "TH2D.h"
#include "TF1.h"
#include "TClonesArray.h"

class LAElectronicsTask : public KBTask
{ 
  public:
    LAElectronicsTask();
    virtual ~LAElectronicsTask() {}

    bool Init();
    void Exec(Option_t*);

  private:
    TClonesArray* fPadArray;

    Int_t fNPlanes;
    Int_t fNTbs;
    Double_t feVToADC;

    TF1 *fPulseFunction;

  ClassDef(LAElectronicsTask, 1)
};

#endif
