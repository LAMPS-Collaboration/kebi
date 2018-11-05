#include "KBHitList.hh"
#include "KBHit.hh"

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

void KBHitList::Print(Option_t *option) const
{
  TString ref = "HTM-ID|XYZ|Charge: ";
  if (TString(option).Index("r")>=0)
    ref = "                 > ";

  if (fHitArray.size()!=0) {
    for (auto hit : fHitArray) {
      cout << ref
        << setw(4)  << hit -> GetHitID()
        << setw(4)  << hit -> GetTrackID()
        << setw(4)  << hit -> GetMCID() << " |"
        << setw(12) << hit -> X()
        << setw(12) << hit -> Y()
        << setw(12) << hit -> Z() << " |"
        << setw(12) << hit -> W() << endl;
    }
  }
  else {
    ref = "              IDs: ";
    cout << ref;
    for (auto hitID : fHitIDArray)
      cout << setw(4) << hitID;
    cout << endl;
  }
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

Int_t KBHitList::GetNumHits() const { return fHitIDArray.size(); }
KBHit *KBHitList::GetHit(Int_t idx) const { return fHitArray.at(idx); }
