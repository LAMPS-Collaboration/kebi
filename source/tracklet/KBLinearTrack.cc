#include "KBLinearTrack.hh"
#include "KBLinearTrackFitter.hh"

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
