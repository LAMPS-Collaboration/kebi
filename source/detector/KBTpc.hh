#ifndef KBTPC_HH
#define KBTPC_HH

#include "KBDetector.hh"
#include "KBPadPlane.hh"

#include "TVector3.h"

class KBTpc : public KBDetector
{
  public:
    KBTpc(const char *name, const char *title);
    virtual ~KBTpc() {};

    virtual bool Init();

    KBPadPlane *GetPadPlane(Int_t idx = 0);
    Int_t GetEFieldAxis();

    void GetDriftPlane(Double_t k, Int_t &planeID, Double_t &kPlane);

    TVector3 XYZToIJK(TVector3 xyz);
    TVector3 IJKToXYZ(TVector3 ijk);

    void XYZToIJK(Double_t x, Double_t y, Double_t z, Double_t &i, Double_t &j, Double_t &k);
    void IJKToXYZ(Double_t i, Double_t j, Double_t k, Double_t &x, Double_t &y, Double_t &z);

  protected:
    Int_t fEFieldAxis = -1;

    Double_t fCathodeK = -999;
    Double_t fPlaneK[2];


  ClassDef(KBTpc, 1)
};

#endif
