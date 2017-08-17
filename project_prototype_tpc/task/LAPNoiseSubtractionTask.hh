#ifndef LAMPSPROTOTYPENOISESUBTRACTIONTASK_HH
#define LAMPSPROTOTYPENOISESUBTRACTIONTASK_HH

#include "KBTask.hh"
#include "KBTpc.hh"
#include "TTree.h"

#include "TClonesArray.h"
#include "GETDecoder.hh"
#include "GETCoboFrame.hh"
#include "GETBasicFrame.hh"

class LAPNoiseSubtractionTask : public KBTask
{ 
  public:
    LAPNoiseSubtractionTask();
    virtual ~LAPNoiseSubtractionTask() {}

    bool Init();
    void Exec(Option_t*);

    void SetNoiseRange(Int_t tbi, Int_t tbf);
    void SetNoiseAmplitudeThreshold(Double_t val);

  private:
    void FindReferencePad();
    void CopyRaw(Short_t *in, Double_t *out);
    Double_t BaseLineCorrection(Double_t *out, Int_t tbi, Int_t tbf);
    Double_t NoiseAmplitudeCorrection(Double_t *out, Double_t *ref, Int_t tbi, Int_t tbf);
    void SaturationCorrection(Double_t *out, Short_t *raw, Double_t baseLine);

  private:
    TClonesArray* fPadArray;

    Int_t fIdxPadRef = -1;

    Int_t fTbNoiseStart = 0;
    Int_t fTbNoiseEnd = 180;

  ClassDef(LAPNoiseSubtractionTask, 1)
};

#endif
