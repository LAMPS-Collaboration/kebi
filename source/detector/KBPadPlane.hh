#ifndef KBPADPLANE
#define KBPADPLANE

#include "KBPad.hh"
#include "KBDetectorPlane.hh"

#include "TH2.h"
#include "TClonesArray.h"

class KBPadPlane : public KBDetectorPlane
{
  public:
    KBPadPlane(const char *name, const char *title);
    virtual ~KBPadPlane() {};

    virtual void Print(Option_t *option = "") const;
    virtual void Clear(Option_t *option = "");

    virtual Int_t FindPadID(Double_t i, Double_t j) = 0;
    virtual Int_t FindPadID(Int_t section, Int_t row, Int_t layer) = 0;

  public:
    KBPad *GetPadFast(Int_t idx);
    KBPad *GetPad(Int_t idx);

    void SetPadArray(TClonesArray *padArray);
    Int_t GetNPads();

    void FillBufferIn(Double_t i, Double_t j, Double_t tb, Double_t val); 
    void FillBufferToHist(Option_t *option = "out");

    void SetPlaneK(Double_t k);
    Double_t GetPlaneK();

    void ResetHitMap();
    void AddHit(KBHit *hit);
    KBHit *PullOutNextFreeHit();
    void PullOutNeighborHits(vector<KBHit*> *hits, vector<KBHit*> *neighborHits);
    void PullOutNeighborHits(TVector3 p, Int_t range, vector<KBHit*> *neighborHits);
    void PullOutNeighborPads(vector<KBPad*> *pads, vector<KBPad*> *neighborPads);

  protected:
    Int_t fEFieldAxis = -1;
    Double_t fPlaneK = -999;

    Int_t fFreePadIdx = -1;

  ClassDef(KBPadPlane, 1)
};

#endif
