#include "KBTpc.hh"

#include <iostream>
using namespace std;

ClassImp(KBTpc)

KBTpc::KBTpc(const char *name, const char *title)
:KBDetector(name, title)
{
}

bool KBTpc::Init()
{
  TString axis;
  fPar -> GetParString("tpcEFieldAxis", axis);
  axis.ToLower();

  if (axis == "x") fEFieldAxis = 0;
  if (axis == "y") fEFieldAxis = 1;
  if (axis == "z") fEFieldAxis = 2;

  fPar -> GetParInt("tpcNPadPlanes", fNPlanes);
  fPar -> GetParDouble("tpcCathodePlaneK", fCathodeK);
  for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++)
    fPar -> GetParDouble(Form("tpcPadPlaneK%d",iPlane), fPlaneK[iPlane]);

  if (BuildGeometry() == false)
    return false;

  if (BuildDetectorPlane() == false)
    return false;

  return true;
}

KBPadPlane *KBTpc::GetPadPlane(Int_t idx) { return (KBPadPlane *) GetDetectorPlane(idx); }
Int_t KBTpc::GetEFieldAxis() { return fEFieldAxis; }

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

TVector3 KBTpc::XYZToIJK(TVector3 xyz)
{
  TVector3 ijk;

       if (fEFieldAxis == 0) ijk.SetXYZ(xyz.Y(), xyz.Z(), xyz.X());
  else if (fEFieldAxis == 1) ijk.SetXYZ(xyz.Z(), xyz.X(), xyz.Y());
  else if (fEFieldAxis == 2) ijk.SetXYZ(xyz.X(), xyz.Y(), xyz.Z());

  return ijk;
}

TVector3 KBTpc::IJKToXYZ(TVector3 ijk)
{
  TVector3 xyz;

       if (fEFieldAxis == 0) xyz.SetXYZ(ijk.Z(), ijk.X(), ijk.Y());
  else if (fEFieldAxis == 1) xyz.SetXYZ(ijk.Y(), ijk.Z(), ijk.X());
  else if (fEFieldAxis == 2) xyz.SetXYZ(ijk.X(), ijk.Y(), ijk.Z());

  return xyz;
}

void KBTpc::XYZToIJK(Double_t x, Double_t y, Double_t z, Double_t &i, Double_t &j, Double_t &k)
{
       if (fEFieldAxis == 0) { i = y; j = z; k = x; }
  else if (fEFieldAxis == 1) { i = z; j = x; k = y; }
  else if (fEFieldAxis == 2) { i = x; j = y; k = z; }
}

void KBTpc::IJKToXYZ(Double_t i, Double_t j, Double_t k, Double_t &x, Double_t &y, Double_t &z)
{
       if (fEFieldAxis == 0) { x = k; y = i; z = j; }
  else if (fEFieldAxis == 1) { x = j; y = k; z = i; }
  else if (fEFieldAxis == 2) { x = i; y = j; z = k; }
}
