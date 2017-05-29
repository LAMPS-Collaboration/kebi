#ifndef LADRIFTElECTRONTASK_HH
#define LADRIFTElECTRONTASK_HH

#include "KBTask.hh"
#include "KBTpc.hh"

#include "TH2D.h"
#include "TF1.h"
#include "TClonesArray.h"

class LADriftElectronTask : public KBTask
{ 
  public:
    LADriftElectronTask();
    virtual ~LADriftElectronTask() {}

    void SetTPC(KBTpc *tpc);

    bool Init();
    void Exec(Option_t*);

    void SetPadPersistency(bool persistence);

  private:
    TClonesArray* fMCStepArray;
    TClonesArray* fPadArray;
    bool fPersistency = true;

    KBTpc *fTpc = nullptr;
    KBPadPlane *fPadPlane = nullptr;
    Int_t fNPlanes = 0;

    Double_t fDriftVelocity = 0;
    Double_t fCoefLD = 0;
    Double_t fCoefTD = 0;
    Double_t fEIonize = 0;

    Int_t fNElInCluster = 0;

    TH2D *fDiffusionFunction = nullptr;
    TF1 *fGainFunction = nullptr;
    Double_t fGainZeroRatio = 0;

    Int_t fNTbs;
    Double_t fTbTime;

  ClassDef(LADriftElectronTask, 1)
};

#endif
