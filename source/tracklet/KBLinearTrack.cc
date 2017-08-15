#include "KBLinearTrack.hh"
#include "KBLinearTrackFitter.hh"
#include <iostream>
using namespace std;

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

void KBLinearTrack::SetTrack(TVector3 pos1, TVector3 pos2)
{
  KBGeoLine::SetLine(pos1, pos2);
}

void KBLinearTrack::Clear(Option_t *option)
{
  fX1 = -1;
  fY1 = -1;
  fZ1 = -1;
  fX2 = 1;
  fY2 = 1;
  fZ2 = 1;
}

void KBLinearTrack::Print(Option_t *) const
{
    cout << "[KBLinearTrack] from >" << setw(12) << fX1 << "," << setw(12) << fY1 << "," << setw(12) << fZ1 << endl;
    cout << "                  to >" << setw(12) << fX2 << "," << setw(12) << fY2 << "," << setw(12) << fZ2 << endl;

    Double_t fX1 = 0;
    Double_t fY1 = 0;
    Double_t fZ1 = 0;

    Double_t fX2 = 0;
    Double_t fY2 = 0;
    Double_t fZ2 = 0;
}

KBTrackFitter *KBLinearTrack::CreateTrackFitter() const { return new KBLinearTrackFitter(); }

TVector3 KBLinearTrack::Momentum(Double_t) const { return KBGeoLine::Direction(); } 
TVector3 KBLinearTrack::PositionAtHead()   const { return KBGeoLine::GetPoint2(); } 
TVector3 KBLinearTrack::PositionAtTail()   const { return KBGeoLine::GetPoint1(); } 
Double_t KBLinearTrack::TrackLength()      const { return KBGeoLine::Length(); }

TVector3 KBLinearTrack::ExtrapolateTo(TVector3 point) const
{
  return KBGeoLine::ClosestPointOnLine(point);
}

TVector3 KBLinearTrack::ExtrapolateHead(Double_t l) const
{
  auto direction = this -> Momentum();
  auto pos = this -> PositionAtHead();
  pos = pos + l * direction;
  return pos;
}

TVector3 KBLinearTrack::ExtrapolateTail(Double_t l) const
{
  auto direction = this -> Momentum();
  auto pos = this -> PositionAtTail();
  pos = pos - l * direction;
  return pos;
}

TVector3 KBLinearTrack::ExtrapolateByRatio(Double_t r) const
{
  auto direction = this -> Momentum();
  auto pos = this -> PositionAtTail();
  auto trackLength = this -> TrackLength();
  pos = pos + r * trackLength * direction;
  return pos;
}

TVector3 KBLinearTrack::ExtrapolateByLength(Double_t l) const
{
  auto direction = this -> Momentum();
  auto pos = this -> PositionAtTail();
  pos = pos + l * direction;
  return pos;
}

Double_t KBLinearTrack::LengthAt(TVector3 point) const
{
  auto pos = this -> ExtrapolateTo(point);
  auto tail = this -> PositionAtTail();
  auto length = (pos-tail).Mag();
  return length;
}

void KBLinearTrack::SetQuality(Double_t val) { fQuality = val; }
Double_t KBLinearTrack::GetQuality() { return fQuality; }
