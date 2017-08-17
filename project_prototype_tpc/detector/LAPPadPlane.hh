#ifndef LAMPSPROTOTYPEPADPLANE_HH
#define LAMPSPROTOTYPEPADPLANE_HH

#include "KBPadPlane.hh"
#include "TF1.h"

class LAPPadPlane : public KBPadPlane
{
  public:
    LAPPadPlane();
    virtual ~LAPPadPlane() {}

    virtual bool Init();

    virtual Int_t FindPadID(Double_t i, Double_t j);
    virtual Int_t FindPadID(Int_t section, Int_t row, Int_t layer);

    virtual Double_t PadDisplacement() const;

    virtual bool IsInBoundary(Double_t i, Double_t j);

    virtual void DrawFrame(Option_t *option = "");
    virtual TH2* GetHist(Option_t *option = "");

    virtual TCanvas *GetCanvas(Option_t *optiont = "");

  private:
    Int_t FindSection(Double_t i, Double_t j);

    Double_t fRMin;
    Double_t fRMax;

    Double_t fTanPi1o8;
    Double_t fTanPi3o8;
    Double_t fTanPi5o8;
    Double_t fTanPi7o8;

    Double_t fSectionAngle[4];
    Double_t fDW[4];
    Double_t fDR[4];
    std::vector<Int_t> fNRowsInLayer[4];
    Double_t fRBase[4];

    Int_t fNPadsTotal = 0;

    std::map<std::vector<Int_t>, Int_t> fPadMap;

  ClassDef(LAPPadPlane, 1)
};

#endif
