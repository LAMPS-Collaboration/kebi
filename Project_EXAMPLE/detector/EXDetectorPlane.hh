#ifndef EXDETECTORPLANE_HH
#define EXDETECTORPLANE_HH

//#include "KBPadPlane.hh"
#include "KBDetectorPlane.hh"

//class EXDetectorPlane : public KBPadPlane
class EXDetectorPlane : public KBDetectorPlane
{
  public:
    EXDetectorPlane();
    virtual ~EXDetectorPlane() {}

    virtual bool Init();
    virtual bool IsInBoundary(Double_t i, Double_t j);
    virtual Int_t FindChannelID(Double_t i, Double_t j);
    virtual TCanvas *GetCanvas(Option_t *option = "");
    virtual void DrawFrame(Option_t *option = "");
    virtual TH2* GetHist(Option_t *option = "");

  ClassDef(EXDetectorPlane, 1)
};

#endif
