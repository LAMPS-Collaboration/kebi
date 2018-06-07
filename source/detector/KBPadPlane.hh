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

    virtual Int_t FindChannelID(Double_t i, Double_t j);

    virtual Double_t PadDisplacement() const = 0; ///< Rough (maximum) value of displacements between pads

  public:
    KBPad *GetPadFast(Int_t idx);
    KBPad *GetPad(Int_t idx);

    void SetPadArray(TClonesArray *padArray);
    void SetHitArray(TClonesArray *hitArray);
    Int_t GetNumPads();

    void FillBufferIn(Double_t i, Double_t j, Double_t tb, Double_t val); 
    void FillDataToHist(Option_t *option = "out");

    void SetPlaneK(Double_t k);
    Double_t GetPlaneK();

    virtual void ResetHitMap();
    void AddHit(KBHit *hit);

    virtual KBHit *PullOutNextFreeHit();
    void PullOutNeighborHits(vector<KBHit*> *hits, vector<KBHit*> *neighborHits);
    void PullOutNeighborHits(TVector3 p, Int_t range, vector<KBHit*> *neighborHits);

    void GrabNeighborPads(vector<KBPad*> *pads, vector<KBPad*> *neighborPads);
    TObjArray *GetPadArray();

    bool PadPositionChecker();

  protected:
    Int_t fEFieldAxis = -1;
    Double_t fPlaneK = -999;

    Int_t fFreePadIdx = 0;

  ClassDef(KBPadPlane, 1)
};

#endif
