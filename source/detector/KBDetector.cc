#include "KBDetector.hh"

#include <iostream>
using namespace std;

ClassImp(KBDetector)

KBDetector::KBDetector()
:KBDetector("KBDetector","default detector class")
{
}

KBDetector::KBDetector(const char *name, const char *title)
:TNamed(name, title), fDetectorPlaneArray(new TObjArray())
{
}

void KBDetector::Print(Option_t *option) const
{
  kb_info << fName << ", " << fTitle << endl;

  for (auto iPlane = 0; iPlane < fNPlanes; ++iPlane) {
    auto plane = (KBDetectorPlane *) fDetectorPlaneArray -> At(iPlane);
    plane -> Print();
  }
}

TGeoManager *KBDetector::GetGeoManager()
{
  return fGeoManager;
}

void KBDetector::SetTransparency(Int_t transparency)
{
  TObjArray* listVolume = gGeoManager -> GetListOfVolumes();
  Int_t nVolumes = listVolume -> GetEntries();
  for (Int_t iVolume = 0; iVolume < nVolumes; iVolume++)
    ((TGeoVolume*) listVolume -> At(iVolume)) -> SetTransparency(transparency);
}


void KBDetector::AddPlane(KBDetectorPlane *plane) {
  plane -> SetRank(fRank+1);
  fDetectorPlaneArray -> Add(plane);
}

Int_t KBDetector::GetNPlanes() { return fNPlanes; }

KBDetectorPlane *KBDetector::GetDetectorPlane(Int_t idx) { return (KBDetectorPlane *) fDetectorPlaneArray -> At(idx); }
