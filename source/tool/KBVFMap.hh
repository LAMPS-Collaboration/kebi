#ifndef KBVFMAP_HH
#define KBVFMAP_HH

/**
 * Vector Field Map
*/

#include "TH2.h"
#include "TF1.h"

#include "KBVFPoint.hh"

class KBVFFitResult
{
  public:
    Int_t fNDF;
};

class KBVFMap
{
  public:
    KBVFMap() {};
    KBVFMap(TH2* sample);
    virtual ~KBVFMap() {};

    void InitializeField(TH2* sample);
    void Fit(TF1* function, Int_t itMax = 0, Option_t *opt = "");
    void DrawBox(Int_t x, Int_t y);

    void DrawField();

    void SetNBinsX(Int_t n);
    void SetLowLimitX(Double_t v);
    void SetHighLimitX(Double_t v);
    void SetBinWidthX(Double_t v);

    Int_t    GetNBinsX() const;
    Double_t GetLowLimitX() const;
    Double_t GetHighLimitX() const;
    Double_t GetBinWidthX() const;

    void SetNBinsY(Int_t n);
    void SetLowLimitY(Double_t v);
    void SetHighLimitY(Double_t v);
    void SetBinWidthY(Double_t v);

    Int_t    GetNBinsY() const;
    Double_t GetLowLimitY() const;
    Double_t GetHighLimitY() const;
    Double_t GetBinWidthY() const;

    void SetMaxV(Double_t v);
    Double_t GetMaxV() const;

  private:
    KBVFPoint ***fVFPoints;

    Int_t    fNBinsX    = 0;
    Double_t fLowLimitX  = 0;
    Double_t fHighLimitX  = 0;
    Double_t fBinWidthX = 0;

    Int_t    fNBinsY    = 0;
    Double_t fLowLimitY  = 0;
    Double_t fHighLimitY  = 0;
    Double_t fBinWidthY = 0;

    Double_t fMaxV = 0;
    Double_t fBinWidthXY = 0;


  ClassDef(KBVFMap, 1)
};

#endif
