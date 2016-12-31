#ifndef KBDETECTORPLANE_HH
#define KBDETECTORPLANE_HH

#include "KBChannel.hh"
#include "KBParameterContainerHolder.hh"

#include "TH2.h"
#include "TCanvas.h"
#include "TObject.h"
#include "TNamed.h"
#include "TObjArray.h"
#include "TClonesArray.h"

class KBDetectorPlane : public TNamed, public KBParameterContainerHolder
{
  public:
    KBDetectorPlane(const char *name, const char *title);
    virtual ~KBDetectorPlane() {};

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option = "") const;
    virtual bool Init() = 0;

    virtual bool IsInBoundary(Double_t i, Double_t j) = 0;

    virtual TCanvas *GetCanvas(Option_t *option = "");
    virtual void DrawFrame(Option_t *option = "") = 0;
    virtual TH2* GetHist(Option_t *option = "") = 0;

  public:
    void SetPlaneID(Int_t id);
    Int_t GetPlaneID() const;

    KBChannel *GetChannelFast(Int_t idx);
    KBChannel *GetChannel(Int_t idx);

    Int_t GetNChannels();
    TObjArray *GetChannelArray();

  protected:
    TObjArray *fChannelArray = nullptr;

    Int_t fPlaneID = -1;

    TCanvas *fCanvas = nullptr;
    TH2 *fH2Plane = nullptr;


  ClassDef(KBDetectorPlane, 1)
};

#endif
