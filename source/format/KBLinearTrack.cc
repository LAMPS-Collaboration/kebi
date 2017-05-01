#include "KBLinearTrack.hh"

ClassImp(KBLinearTrack)

KBLinearTrack::KBLinearTrack()
{
}

KBLinearTrack::KBLinearTrack(Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2)
:KBGeoLine(x1, y1, z1, x2, y2, z2)
{
}

KBLinearTrack::KBLinearTrack(TVector3 pos1, TVector3 pos2)
:KBGeoLine(pos1, pos2)
{
}
