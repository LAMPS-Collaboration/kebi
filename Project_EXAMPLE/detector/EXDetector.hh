#ifndef EXDETECTOR_HH
#define EXDETECTOR_HH

//#include "KBTpc.hh"
#include "KBDetector.hh"

//class EXDetector : public KBTpc
class EXDetector : public KBDetector
{
  public:
    EXDetector();
    virtual ~EXDetector() {};

    virtual bool Init();

  protected:
    bool BuildGeometry();
    bool BuildDetectorPlane();

  ClassDef(EXDetector, 1)
};

#endif
