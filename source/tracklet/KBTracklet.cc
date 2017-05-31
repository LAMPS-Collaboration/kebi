#include "KBTracklet.hh"

ClassImp(KBTracklet)

void KBTracklet::AddHit(KBHit *hit)
{
  fHitList -> AddHit(hit);
}

void KBTracklet::RemoveHit(KBHit *hit)
{
  fHitList -> RemoveHit(hit);
}
