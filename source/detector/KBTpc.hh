#ifndef KBTPC_HH
#define KBTPC_HH

#include "KBDetector.hh"
#include "KBPadPlane.hh"

#include "TVector3.h"
#include "KBVector3.hh"

class KBTpc : public KBDetector
{
  public:
    KBTpc();
    KBTpc(const char *name, const char *title);
    virtual ~KBTpc() {};

    virtual bool Init();

    virtual KBPadPlane *GetPadPlane(Int_t idx = 0);
    KBVector3::Axis GetEFieldAxis();

    void GetDriftPlane(Double_t k, Int_t &planeID, Double_t &kPlane);

  protected:
    KBVector3::Axis fEFieldAxis = KBVector3::kNon;

    Double_t fCathodeK = -999;
    Double_t fPlaneK[2];


  ClassDef(KBTpc, 1)
};

#endif
