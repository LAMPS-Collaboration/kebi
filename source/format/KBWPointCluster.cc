#include "KBWPointCluster.hh"
#include "TString.h"
#include <iostream>
#include <iomanip>
using namespace std;

ClassImp(KBWPointCluster)

KBWPointCluster::KBWPointCluster() { Clear(); }
KBWPointCluster::~KBWPointCluster() {}

void KBWPointCluster::Print(Option_t *opt) const
{
  KBWPoint::Print();

  auto optstr = TString(opt);
  optstr.ToLower();

  if (optstr.Index("c")>=0) {
    cout << "CovXX: "
         << setw(12) << fCov[0][0]
         << setw(12) << fCov[1][1]
         << setw(12) << fCov[2][2] << endl;

    cout << "CovXY: "
         << setw(12) << fCov[0][1]
         << setw(12) << fCov[1][2]
         << setw(12) << fCov[2][0] << endl;
  }

  if (optstr.Index("e")>=0) {
    cout << "ErrXX: "
         << setw(12) << fCov[0][0]/sqrt(fW)
         << setw(12) << fCov[1][1]/sqrt(fW)
         << setw(12) << fCov[2][2]/sqrt(fW) << endl;

    cout << "ErrXY: "
         << setw(12) << fCov[0][1]/sqrt(fW)
         << setw(12) << fCov[1][2]/sqrt(fW)
         << setw(12) << fCov[2][0]/sqrt(fW) << endl;
  }
}

void KBWPointCluster::Clear(Option_t *opt)
{
  KBWPoint::Clear(opt);

  for (auto i = 0; i < 3; ++i)
  for (auto j = 0; j < 3; ++j)
    fCov[i][j] = 0;
}

void KBWPointCluster::Copy(TObject &obj) const
{
  KBWPoint::Copy(obj);
  auto wpc = (KBWPointCluster &) obj;

  for (auto i = 0; i < 3; ++i)
  for (auto j = 0; j < 3; ++j)
    wpc.SetCov(i, j, fCov[i][j]);
}

void KBWPointCluster::Add(KBWPoint wp)
{
  auto w = wp.w();
  auto ww = fW + w;

  for (int i = 0; i < 3; ++i)
    operator[](i) = (fW*operator[](i) + w*wp[i])/ww;

  if (fW == 0) {
    for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      fCov[i][j] = 0;
  }

  else {
    for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      fCov[i][j] = fW*fCov[i][j]/ww + w*(operator[](i)-wp[i])*(operator[](j)-wp[j])/fW;
  }

  fW = ww;
}

void KBWPointCluster::SetCov(Double_t cov[][3])
{
  for (int i = 0; i < 3; ++i)
  for (int j = 0; j < 3; ++j)
    fCov[i][j] = cov[i][j];
}

void KBWPointCluster::SetCov(Int_t i, Int_t j, Double_t cov)
{
  fCov[i][j] = cov;
}

void KBWPointCluster::SetPosSigma(Double_t sigx, Double_t sigy, Double_t sigz)
{
  fCov[0][0] = sigx * sigx;
  fCov[1][1] = sigy * sigy;
  fCov[2][2] = sigz * sigz;
}

void KBWPointCluster::SetPosSigma(TVector3 sig)
{
  fCov[0][0] = sig.X() * sig.X();
  fCov[1][1] = sig.Y() * sig.Y();
  fCov[2][2] = sig.Z() * sig.Z();
}

TVector3 KBWPointCluster::GetPosSigma()
{
  return TVector3(sqrt(fCov[0][0]), sqrt(fCov[1][1]), sqrt(fCov[2][2]));
}

#ifdef ACTIVATE_EVE
bool KBWPointCluster::DrawByDefault() { return false; }
bool KBWPointCluster::IsEveSet() { return true; }

TEveElement *KBWPointCluster::CreateEveElement()
{
  auto pointSet = new TEvePointSet("WPointCluster");
  pointSet -> SetMarkerColor(kViolet);
  pointSet -> SetMarkerSize(1);
  pointSet -> SetMarkerStyle(20);

  return pointSet;
}

void KBWPointCluster::SetEveElement(TEveElement *)
{
}

void KBWPointCluster::AddToEveSet(TEveElement *eveSet)
{
  auto pointSet = (TEvePointSet *) eveSet;
  pointSet -> SetNextPoint(fX, fY, fZ);
}
#endif
