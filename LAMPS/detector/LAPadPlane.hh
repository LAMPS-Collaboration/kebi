#ifndef LAMPSPADPLANE_HH
#define LAMPSPADPLANE_HH

#include "KBPadPlane.hh"
#include "TF1.h"

class LAPadPlane : public KBPadPlane
{
  public:
    LAPadPlane();
    virtual ~LAPadPlane() {}

    virtual bool Init();

    virtual Int_t FindPadID(Double_t i, Double_t j);
    virtual Int_t FindPadID(Int_t section, Int_t row, Int_t layer);

    virtual bool IsInBoundary(Double_t i, Double_t j);

    virtual void DrawFrame(Option_t *option = "");
    virtual TH2* GetHist(Option_t *option = "-1");

    virtual TCanvas *GetCanvas(Option_t *optiont = "");

  private:
    void AddPad(Int_t section, Int_t layer, Int_t row, Double_t i, Double_t j);
    Int_t FindSection(Double_t i, Double_t j);
    Int_t FindDivisionIndex(Int_t layer);
    Int_t FindDivisionIndex(Double_t r);

    TF1* fFuncXRightBound = nullptr;

    Double_t fRMin = 0;
    Double_t fRMax = 0;

    Int_t fNLayerDivision = 0;

    Double_t *fArcLength;
    Double_t *fRadius;
    Int_t *fNLayers;

    Double_t fTanPi1o8;
    Double_t fTanPi3o8;
    Double_t fTanPi5o8;
    Double_t fTanPi7o8;
    Double_t fCosPiNo4[8];
    Double_t fSinPiNo4[8];

    std::vector<Int_t> fNHalfRowsInLayer;
    Int_t fNLayersTotal = 0;

    std::vector<Int_t> fLayerDivI;
    std::vector<Double_t> fRDivI;

    Int_t fNPadsTotal = 0;

    std::map<std::vector<Int_t>, Int_t> fPadMap;

  ClassDef(LAPadPlane, 1)
};

#endif
