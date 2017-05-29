#ifndef LAMPSTPC_HH
#define LAMPSTPC_HH

#include "KBTpc.hh"

class LATpc : public KBTpc
{
  public:
    LATpc();
    virtual ~LATpc() {};

  protected:
    bool BuildGeometry();
    bool BuildDetectorPlane();

  ClassDef(LATpc, 1)
};

#endif
