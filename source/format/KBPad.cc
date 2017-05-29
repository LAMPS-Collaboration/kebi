#include "KBPad.hh"

#include <iostream>
using namespace std;

ClassImp(KBPad)

void KBPad::Clear(Option_t *)
{
  fActive = false;

  memset(fBufferIn, 0, sizeof(Double_t)*512);
  memset(fBufferRaw, 0, sizeof(Short_t)*512);
  memset(fBufferOut, 0, sizeof(Double_t)*512);

  fGrabed = false;
}

void KBPad::Print(Option_t *option) const
{
  cout << "[KBPad]" << endl;
  cout << "  Pad-ID(Plane-ID)      : " << fID << "(" << fPlaneID << ")" << endl;
  cout << "  AsAd(1)AGET(1)CH(2)   : " << Form("%d%d%02d",fAsAdID,fAGETID,fChannelID) << endl;
  cout << "  (Section, Row, Layer) : (" << fSection << ", " << fRow << ", " << fLayer << ")" << endl;
  cout << "  Noise-Amp | BaseLine  : " << fNoiseAmp << " | " << fBaseLine << endl;
  cout << "  Position              : (" << fI << ", " << fJ << ") " << endl;
}

void KBPad::Draw(Option_t *option)
{
}

void KBPad::SetPad(KBPad *pad)
{
  fID = pad -> GetPadID();
  fPlaneID = pad -> GetPlaneID();
  fAsAdID = pad -> GetAsAdID();
  fAGETID = pad -> GetAGETID();
  fChannelID = pad -> GetChannelID();

  fI = pad -> GetI();
  fJ = pad -> GetJ();

  fSection = pad -> GetSection();
  fRow = pad -> GetRow();
  fLayer = pad -> GetLayer();

  fBaseLine = pad -> GetBaseLine();
  fNoiseAmp = pad -> GetNoiseAmplitude();
}

void KBPad::CopyPadData(KBPad* pad)
{
  SetBufferIn(pad->GetBufferIn());
  SetBufferRaw(pad->GetBufferRaw());
  SetBufferOut(pad->GetBufferOut());
}

void KBPad::SetActive(bool active) { fActive = active; }
bool KBPad::IsActive() const { return fActive; }

void KBPad::SetPadID(Int_t id) { fID = id; }
Int_t KBPad::GetPadID() const { return fID; }

void KBPad::SetPlaneID(Int_t id) { fPlaneID = id; }
Int_t KBPad::GetPlaneID() const { return fPlaneID; }

void KBPad::SetAsAdID(Int_t id) { fAsAdID = id; }
Int_t KBPad::GetAsAdID() const { return fAsAdID; }

void KBPad::SetAGETID(Int_t id) { fAGETID = id; }
Int_t KBPad::GetAGETID() const { return fAGETID; }

void KBPad::SetChannelID(Int_t id) { fChannelID = id; }
Int_t KBPad::GetChannelID() const { return fChannelID; }

void KBPad::SetBaseLine(Double_t baseLine) { fBaseLine = baseLine; }
Double_t KBPad::GetBaseLine() const { return fBaseLine; }

void KBPad::SetNoiseAmplitude(Double_t gain) { fNoiseAmp = gain; }
Double_t KBPad::GetNoiseAmplitude() const { return fNoiseAmp; }

void KBPad::SetPosition(Double_t i, Double_t j) 
{
  fI = i;
  fJ = j;
}

void KBPad::GetPosition(Double_t &i, Double_t &j) const 
{
  i = fI;
  j = fJ;
}

Double_t KBPad::GetI() const { return fI; }
Double_t KBPad::GetJ() const { return fJ; }

void KBPad::AddPadCorner(Double_t i, Double_t j) { fPadCorners.push_back(TVector2(i,j)); }
vector<TVector2> *KBPad::GetPadCorners() { return &fPadCorners; }

void KBPad::SetSectionRowLayer(Int_t section, Int_t row, Int_t layer) 
{
  fSection = section;
  fRow = row;
  fLayer = layer;
}

void KBPad::GetSectionRowLayer(Int_t &section, Int_t &row, Int_t &layer) const 
{
  section = fSection;
  row = fRow;
  layer = fLayer;
}

Int_t KBPad::GetSection() const { return fSection; }
Int_t KBPad::GetRow() const { return fRow; }
Int_t KBPad::GetLayer() const { return fLayer; } 

void KBPad::FillBufferIn(Int_t idx, Double_t val) { fActive = true; fBufferIn[idx] += val; }
void KBPad::SetBufferIn(Double_t *buffer) { memcpy(fBufferIn, buffer, sizeof(Double_t)*512); }
Double_t *KBPad::GetBufferIn() { return fBufferIn; }

void KBPad::SetBufferRaw(Short_t *buffer) { memcpy(fBufferRaw, buffer, sizeof(Short_t)*512); }
Short_t *KBPad::GetBufferRaw() { return fBufferRaw; }

void KBPad::SetBufferOut(Double_t *buffer) { memcpy(fBufferOut, buffer, sizeof(Double_t)*512); }
Double_t *KBPad::GetBufferOut() { return fBufferOut; }

void KBPad::AddNeighborPad(KBPad *pad) { fNeighborPadArray.push_back(pad); }
vector<KBPad *> *KBPad::GetNeighborPadArray() { return &fNeighborPadArray; }

void KBPad::AddHit(KBHit *hit) { fHitArray.push_back(hit); }
Int_t KBPad::GetNumHits() const { return fHitArray.size(); }
void KBPad::ClearHits() { fHitArray.clear(); }

KBHit *KBPad::PullOutNextFreeHit()
{
  Int_t n = fHitArray.size();
  if (n == 0)
    return nullptr;

  for (auto i = 0; i < n; i++) {
    auto hit = fHitArray[i];
    if (hit -> GetNTrackCands() == 0) {
      fHitArray.erase(fHitArray.begin()+i);
      return hit;
    }
  }

  return nullptr;
}

void KBPad::PullOutHits(vector<KBHit *> *hits)
{
  Int_t n = fHitArray.size();
  if (n == 0)
    return;

  for (auto i = 0; i < n; i++)
    hits -> push_back(fHitArray[i]);
  fHitArray.clear();
}

bool KBPad::IsGrabed() const { return fGrabed; }
void KBPad::Grab() { fGrabed = true; }
void KBPad::LetGo() { fGrabed = false; }

TH1D *KBPad::GetHist(Option_t *option)
{
  TH1D *hist = new TH1D(Form("Pad%03d",fID),"",512,0,512);
  SetHist(hist, option);

  return hist;
}

void KBPad::SetHist(TH1D *hist, Option_t *option)
{
  hist -> Reset();

  TString optionString = TString(option);
  optionString.ToLower();

  TString namePad = Form("Pad%03d",fID);
  TString nameID = Form("ID%d%d%02d",fAsAdID,fAGETID,fChannelID);

  bool firstNameOn = false;

  TString name;

  if (optionString.Index("p") >= 0) {
    name = name + namePad;
    firstNameOn = true;
  } if (firstNameOn) name = name + "_";

  if (optionString.Index("a") >= 0) {
    name = name + nameID;
    firstNameOn = true;
  }

  if (firstNameOn == false)
    name = namePad;

  hist -> SetNameTitle(name,name+";Time Bucket;ADC");

  if (optionString.Index("o") >= 0) {
    for (auto tb = 0; tb < 512; tb++)
      hist -> SetBinContent(tb+1, fBufferOut[tb]);
  } else if (optionString.Index("r") >= 0) {
    for (auto tb = 0; tb < 512; tb++)
      hist -> SetBinContent(tb+1, fBufferRaw[tb]);
  } else if (optionString.Index("tb") >= 0) {
    for (auto tb = 0; tb < 512; tb++)
      hist -> SetBinContent(tb+1, fBufferIn[tb]);
  }

  hist -> GetYaxis() -> SetRangeUser(0,4095);
}
