#ifndef KBPAD_HH
#define KBPAD_HH

#include "KBChannel.hh"
#include "KBHit.hh"

#include "TObject.h"
#include "TH1D.h"
#include "TVector2.h"

#include <vector>
using namespace std;

class KBPad : public KBChannel
{
  public:
    KBPad() {}
    virtual ~KBPad() {}

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option = "") const;
    virtual void Draw(Option_t *option = "");

    virtual Bool_t IsSortable() const;
    virtual Int_t Compare(const TObject *obj) const;


    void SetPad(KBPad* pad);
    void CopyPadData(KBPad* pad);

    void SetActive(bool active = true);
    bool IsActive() const;

    void SetPadID(Int_t id);
    Int_t GetPadID() const;

    void SetPlaneID(Int_t id);
    Int_t GetPlaneID() const;

    void SetAsAdID(Int_t id);
    Int_t GetAsAdID() const;

    void SetAGETID(Int_t id);
    Int_t GetAGETID() const;

    void SetChannelID(Int_t id);
    Int_t GetChannelID() const;

    void SetBaseLine(Double_t baseLine);
    Double_t GetBaseLine() const;

    void SetNoiseAmplitude(Double_t gain);
    Double_t GetNoiseAmplitude() const;

    void SetPosition(Double_t i, Double_t j);
    void GetPosition(Double_t &i, Double_t &j) const;
    Double_t GetI() const;
    Double_t GetJ() const;

    void AddPadCorner(Double_t i, Double_t j);
    vector<TVector2> *GetPadCorners();

    void SetSectionRowLayer(Int_t section, Int_t row, Int_t layer);
    void GetSectionRowLayer(Int_t &section, Int_t &row, Int_t &layer) const;
    Int_t GetSection() const;
    Int_t GetRow() const;
    Int_t GetLayer() const;

    void FillBufferIn(Int_t idx, Double_t val);
    void SetBufferIn(Double_t *buffer);
    Double_t *GetBufferIn();

    void SetBufferRaw(Short_t *buffer);
    Short_t *GetBufferRaw();

    void SetBufferOut(Double_t *buffer);
    Double_t *GetBufferOut();

    void AddNeighborPad(KBPad *pad);
    vector<KBPad *> *GetNeighborPadArray();

    void AddHit(KBHit *hit);
    Int_t GetNumHits() const;

    void ClearHits();
    KBHit *PullOutNextFreeHit();
    void PullOutHits(vector<KBHit *> *hits);

    bool IsGrabed() const;
    void Grab();
    void LetGo();

    void SetHist(TH1D *hist, Option_t *option = "");
    TH1D *GetHist(Option_t *option = "");

  private:
    bool fActive = false; //!

    Int_t fPlaneID = 0;
    Int_t fAsAdID = -1;
    Int_t fAGETID = -1;
    Int_t fChannelID = -1;

    Double_t fBaseLine = 0;
    Double_t fNoiseAmp = 0;

    Double_t fI = -999;
    Double_t fJ = -999;

    vector<TVector2> fPadCorners; //!

    Int_t fSection = -999;
    Int_t fRow = -999;
    Int_t fLayer = -999;

    Double_t fBufferIn[512]; //!
    Short_t  fBufferRaw[512];
    Double_t fBufferOut[512];

    vector<KBPad *> fNeighborPadArray; //!
    vector<KBHit *> fHitArray; //!

    bool fGrabed = false; //!

  ClassDef(KBPad, 1)
};

#endif
