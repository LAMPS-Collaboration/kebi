#include "KBTpc.hh"

#include <iostream>
using namespace std;

ClassImp(KBTpc)

KBTpc::KBTpc()
:KBTpc("KBTpc","TPC")
{
}

KBTpc::KBTpc(const char *name, const char *title)
:KBDetector(name, title)
{
}

bool KBTpc::Init()
{
  fEFieldAxis = fPar -> GetParAxis("tpcEFieldAxis");

  fNPlanes = fPar -> GetParInt("tpcNPadPlanes");
  fCathodeK = fPar -> GetParDouble("tpcCathodePlaneK");
  for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++)
    fPlaneK[iPlane] = fPar -> GetParDouble(Form("tpcPadPlaneK%d",iPlane));

  if (BuildGeometry() == false)
    return false;

  if (BuildDetectorPlane() == false)
    return false;

  return true;
}

KBPadPlane *KBTpc::GetPadPlane(Int_t idx) { return (KBPadPlane *) GetDetectorPlane(idx); }
KBVector3::Axis KBTpc::GetEFieldAxis() { return fEFieldAxis; }

void KBTpc::GetDriftPlane(Double_t k, Int_t &planeID, Double_t &kPlane)
{
  if (fNPlanes == 1) {
    planeID = 0;
    kPlane = fPlaneK[0];
  }
  else if (fNPlanes == 2) {
    if (fPlaneK[0] > fPlaneK[1]) {
      if (k > fCathodeK && k < fPlaneK[0]) {
        planeID = 0;
        kPlane = fPlaneK[0];
      } else if (k < fCathodeK && k > fPlaneK[1]) {
        planeID = 1;
        kPlane = fPlaneK[1];
      } else {
        planeID = -1;
      }
    } else {
      if (k > fCathodeK && k < fPlaneK[1]) {
        planeID = 1;
        kPlane = fPlaneK[1];
      } else if (k < fCathodeK && k > fPlaneK[0]) {
        planeID = 0;
        kPlane = fPlaneK[0];
      } else {
        planeID = -1;
      }
    }
  }
}
