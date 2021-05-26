#ifndef LAMPSPROTOTYPEDETECTOR_HH
#define LAMPSPROTOTYPEDETECTOR_HH

#include "KBTpc.hh"
#include "LAPPadPlane.hh"

class LAPTpc : public KBTpc
{
  public:
    LAPTpc();
    virtual ~LAPTpc() {};

    virtual bool Init();

    virtual KBVector3::Axis GetEFieldAxis() { return fEFieldAxis; }
    virtual TVector3 GetEField(TVector3 pos);
    virtual KBPadPlane *GetDriftPlane(TVector3 pos);

    /*
    virtual KBPadPlane *GetPadPlane(Int_t idx = 0) {
      kb_debug << "GetPadPlane" << endl;
      LAPPadPlane *padplane = new LAPPadPlane();
      padplane -> SetParameterContainer(fPar);
      padplane -> SetPlaneID(0);
      padplane -> SetPlaneK(fPar->GetParDouble("tpcPadPlaneK"));
      padplane -> Init();
      return (KBPadPlane *) padplane;
    }
    */

  protected:
    bool BuildGeometry();
    bool BuildDetectorPlane();

    KBVector3::Axis fEFieldAxis = KBVector3::kNon;

  ClassDef(LAPTpc, 1)
};

#endif
