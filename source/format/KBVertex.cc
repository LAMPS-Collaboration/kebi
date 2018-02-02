#include "KBVertex.hh"
#include "TEvePointSet.h"

ClassImp(KBVertex)

KBVertex::KBVertex()
{
}

void KBVertex::Clear(Option_t *)
{
  fPosition = TVector3(0,0,0);
  fTrackArray.clear();
  fTrackIDArray.clear();
  fSSetArray.clear();
}

void KBVertex::AddTrack(KBTracklet* track)
{
  fTrackArray.push_back(track);
  fTrackIDArray.push_back(track->GetTrackID());
}

#ifdef ACTIVATE_EVE
bool KBVertex::DrawByDefault() { return true; }
bool KBVertex::IsEveSet() { return true; }

TEveElement *KBVertex::CreateEveElement()
{
  auto pointSet = new TEvePointSet("Vertex");
  pointSet -> SetMarkerColor(kBlack);
  pointSet -> SetMarkerSize(2.5);
  pointSet -> SetMarkerStyle(20);

  return pointSet;
}

void KBVertex::SetEveElement(TEveElement *)
{
}

void KBVertex::AddToEveSet(TEveElement *eveSet)
{
  auto pointSet = (TEvePointSet *) eveSet;
  //pointSet -> SetNextPoint(fPosition.X(), fPosition.Y(), fPosition.Z());
  pointSet -> SetNextPoint(fPosition.Z(), fPosition.X(), fPosition.Y());
}
#endif
