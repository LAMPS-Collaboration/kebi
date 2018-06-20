#include "KBTpcHit.hh"
#include "KBPulseGenerator.hh"
#ifdef ACTIVATE_EVE
#include "TEvePointSet.h"
#endif
#include <iostream>
#include <iomanip>

ClassImp(KBTpcHit)

void KBTpcHit::Clear(Option_t *)
{
  KBHit::Clear();

  fPadID = -1;
  fSection = -999;
  fRow = -999;
  fLayer = -999;
  fTb = -1;

  fTrackCandArray.clear();
}

void KBTpcHit::Print(Option_t *option) const
{
  cout << "ID|XYZ|Charge|SRL|Tb: "
    << setw(4)  << fHitID << " |"
    << setw(12) << fX
    << setw(12) << fY
    << setw(12) << fZ << " |"
    << setw(12) << fW << " |"
    << setw(4) << fSection
    << setw(4) << fRow
    << setw(4) << fLayer << " |"
    << setw(4) << fTb << endl;
}

void KBTpcHit::Copy(TObject &obj) const
{
  KBHit::Copy(obj);
  auto tpchit = (KBTpcHit &) obj;

  tpchit.SetPadID(fPadID);
  tpchit.SetSection(fSection);
  tpchit.SetRow(fRow);
  tpchit.SetLayer(fLayer);
  tpchit.SetTb(fTb);
}

TF1 *KBTpcHit::GetPulseFunction(Option_t *)
{
  auto pulseGen = KBPulseGenerator::GetPulseGenerator();
  auto f1 = pulseGen -> GetPulseFunction("pulse");
  f1 -> SetParameters(fW, fTb);
  return f1;
}

void KBTpcHit::SetPadID(Int_t id) { fPadID = id; }
void KBTpcHit::SetSection(Int_t section) { fSection = section; }
void KBTpcHit::SetRow(Int_t row) { fRow = row; }
void KBTpcHit::SetLayer(Int_t layer) { fLayer = layer; }
void KBTpcHit::SetTb(Double_t tb) { fTb = tb; }

Int_t KBTpcHit::GetPadID() const { return fPadID; }
Int_t KBTpcHit::GetSection() const { return fSection; }
Int_t KBTpcHit::GetRow() const { return fRow; }
Int_t KBTpcHit::GetLayer() const { return fLayer; }
Double_t KBTpcHit::GetTb() const { return fTb; }

#ifdef ACTIVATE_EVE
TEveElement *KBTpcHit::CreateEveElement()
{
  auto pointSet = new TEvePointSet("TpcHit");
  pointSet -> SetMarkerColor(kAzure-8);
  pointSet -> SetMarkerSize(0.4);
  pointSet -> SetMarkerStyle(38);

  return pointSet;
}
#endif
