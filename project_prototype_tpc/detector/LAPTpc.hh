#ifndef LAMPSPROTOTYPEDETECTOR_HH
#define LAMPSPROTOTYPEDETECTOR_HH

#include "KBTpc.hh"

class LAPTpc : public KBTpc
{
  public:
    LAPTpc();
    virtual ~LAPTpc() {};

  protected:
    bool BuildGeometry();
    bool BuildDetectorPlane();

  ClassDef(LAPTpc, 1)
};

#endif
