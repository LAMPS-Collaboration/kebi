#ifndef KBDETECTOR_HH
#define KBDETECTOR_HH

#include "KBGlobal.hh"
#include "KBDetectorPlane.hh"

#include "TNamed.h"
#include "TGeoManager.h"

#include "TObjArray.h"

class KBDetector : public TNamed, public KBGlobal
{
  public:
    KBDetector();
    KBDetector(const char *name, const char *title);
    virtual ~KBDetector() {}

    virtual void Print(Option_t *option="") const;
    virtual bool Init() = 0;

    TGeoManager *GetGeoManager();
    void SetTransparency(Int_t transparency);

    void AddPlane(KBDetectorPlane *plane);
    Int_t GetNPlanes();
    KBDetectorPlane *GetDetectorPlane(Int_t idx = 0);

  protected:
    virtual bool BuildGeometry() = 0;
    virtual bool BuildDetectorPlane() = 0;

    TGeoManager *fGeoManager = nullptr;

    Int_t fNPlanes = 0;
    TObjArray *fDetectorPlaneArray;

  ClassDef(KBDetector, 1)
};

#endif
