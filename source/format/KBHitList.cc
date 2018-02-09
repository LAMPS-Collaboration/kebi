#include "KBHitList.hh"

ClassImp(KBHitList)

KBHitList::KBHitList()
{
  Clear();
}

void KBHitList::Clear(Option_t *option)
{
  fHitArray.clear();
  fHitIDArray.clear();
}

void KBHitList::AddHit(KBHit* hit)
{
  fHitArray.push_back(hit);
  fHitIDArray.push_back(hit->GetHitID());
}

void KBHitList::RemoveHit(KBHit* hit)
{
  Int_t numHits = fHitArray.size();
  for (auto iHit = 0; iHit < numHits; iHit++) {
    if (fHitArray[iHit] == hit) {
      fHitArray.erase(fHitArray.begin()+iHit);
      fHitIDArray.erase(fHitIDArray.begin()+iHit);
      break;
    }
  }
}

vector<KBHit*> *KBHitList::GetHitArray() { return &fHitArray; }
vector<Int_t> *KBHitList::GetHitIDArray() { return &fHitIDArray; }
