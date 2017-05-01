#include "KBHitList.hh"

ClassImp(KBHitList)

KBHitList::KBHitList()
{
}

void KBHitList::AddHit(KBHit* hit)
{
  fHitArray.push_back(hit);
  fHitIDArray.push_back(hit->GetHitID());
}

vector<KBHit*> *KBHitList::GetHitArray() { return &fHitArray; }
vector<Int_t> *KBHitList::GetHitIDArray() { return &fHitIDArray; }
