#include "KBTpcHit.hh"
#include "KBPulseGenerator.hh"
#ifdef ACTIVATE_EVE
#include "TEvePointSet.h"
#endif
#include <iostream>
#include <iomanip>

ClassImp(KBTpcHit)

void KBTpcHit::Clear(Option_t *option)
{
  KBHit::Clear(option);

  fPadID = -1;
  fSection = -999;
  fRow = -999;
  fLayer = -999;
  fTb = -1;

  fTrackCandArray.clear();
}

void KBTpcHit::Print(Option_t *option) const
{
  TString opts = TString(option);

  TString title;
  if (opts.Index(">")>=0) title += "> ";
  if (opts.Index("t")>=0) title += "HTMP-ID|XYZ|Q|SRL|Tb: ";

  if (opts.Index("s")>=0)
    kc_info << title
      << fHitID << ", " << fTrackID << ", " << fMCID << ", " << fPadID << " |"
      << fX << ", " << fY << ", " << fZ << " |" << fW << " |"
      << fSection << ", "<< fRow << ", "<< fLayer << " |" << fTb << endl;
  else //if (opts.Index("a")>=0)
    kc_info << title
      << setw(4)  << fHitID
      << setw(4)  << fTrackID
      << setw(4)  << fMCID
      << setw(4)  << fPadID << " |"
      << setw(12) << fX
      << setw(12) << fY
      << setw(12) << fZ << " |"
      << setw(12) << fW << " |"
      << setw(4) << fSection
      << setw(4) << fRow
      << setw(4) << fLayer << " |"
      << setw(4) << fTb << endl;

  if (opts.Index("d")>=0) {
    TString opts2 = opts;
    opts2.ReplaceAll("d","");
    if (opts.Index("t")>=0)
      opts2.ReplaceAll("t",">");
    else
      opts2 = opts + ">";
    fHitList.Print(opts2);
  }
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
