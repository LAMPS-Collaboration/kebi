#ifndef KBLINEARTRACK_HH
#define KBLINEARTRACK_HH

#include "KBGeoLine.hh"

class KBLinearTrack : public KBGeoLine
{
  public:
    KBLinearTrack();
    KBLinearTrack(Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2);
    KBLinearTrack(TVector3 pos1, TVector3 pos2);
    virtual ~KBLinearTrack() {}

  ClassDef(KBLinearTrack, 1)
};

#endif
