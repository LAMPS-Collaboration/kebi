#include "KBVertex.hh"
#ifdef ACTIVATE_EVE
#include "TEvePointSet.h"
#endif

ClassImp(KBVertex)

KBVertex::KBVertex()
{
  Clear();
}

void KBVertex::Clear(Option_t *)
{
  KBHit::Clear();

  fTrackArray.clear();
  fTrackIDArray.clear();
}

void KBVertex::Copy(TObject &obj) const
{
  KBHit::Copy(obj);

  auto vertex = (KBVertex &) obj;

  for (auto track : fTrackArray)
    vertex.AddTrack(track);
}

void KBVertex::AddTrack(KBTracklet* track)
{
  fTrackArray.push_back(track);
  fTrackIDArray.push_back(track->GetTrackID());
}

#ifdef ACTIVATE_EVE
bool KBVertex::DrawByDefault() { return true; }

bool KBVertex::IsEveSet() { return true; }

TEveElement *KBVertex::CreateEveElement() {
  auto pointSet = new TEvePointSet("Vertex");
  pointSet -> SetMarkerColor(kBlack);
  pointSet -> SetMarkerSize(2.5);
  pointSet -> SetMarkerStyle(20);
  return pointSet;
}

void KBVertex::AddToEveSet(TEveElement *eveSet) {
  auto pointSet = (TEvePointSet *) eveSet;
  pointSet -> SetNextPoint(fX, fY, fZ);
}
#endif
