#include "KBRun.hh"
#include "LAPNoiseSubtractionTask.hh"

#include "TFile.h"

#include <iostream>
#include <fstream>
using namespace std;

#include "dirent.h"

ClassImp(LAPNoiseSubtractionTask)

LAPNoiseSubtractionTask::LAPNoiseSubtractionTask()
:KBTask("LAPNoiseSubtractionTask","")
{
} 

bool LAPNoiseSubtractionTask::Init()
{
  fPadArray = (TClonesArray *) KBRun::GetRun() -> GetBranch("Pad");

  return true;
}

void LAPNoiseSubtractionTask::Exec(Option_t*)
{
  Int_t nPads = fPadArray -> GetEntries();

  FindReferencePad();

  if (fIdxPadRef == -1) {
    cout << "  [" << this -> GetName() << "] Could not find reference pad." << endl;
    return;
  }

  auto padRef = (KBPad *) fPadArray -> At(fIdxPadRef);
  auto rawRef = padRef -> GetBufferRaw(); 

  Double_t outRef[512] = {0};
  padRef -> SetBufferOut(outRef);

  CopyRaw(rawRef, outRef);
  Double_t baseLineRef = BaseLineCorrection(outRef, fTbNoiseStart, fTbNoiseEnd);

  padRef -> SetBaseLine(baseLineRef);
  padRef -> SetNoiseAmplitude(1);



  for (auto iPad = 0; iPad < nPads; iPad++) {
    if (iPad == fIdxPadRef)
      continue;

    auto pad = (KBPad *) fPadArray -> At(iPad);
    auto raw = pad -> GetBufferRaw(); 
    Double_t out[512] = {0};

    CopyRaw(raw, out);
    Double_t baseLine = BaseLineCorrection(out, fTbNoiseStart, fTbNoiseEnd);
    Double_t amp = NoiseAmplitudeCorrection(out, outRef, fTbNoiseStart, fTbNoiseEnd);

    pad -> SetBufferOut(out);
    pad -> SetBaseLine(baseLine);
    pad -> SetNoiseAmplitude(amp);
  }

  cout << "  [" << this -> GetName() << "]" << endl;
  
  return;
}

void LAPNoiseSubtractionTask::SetNoiseRange(Int_t tbi, Int_t tbf)
{
  fTbNoiseStart = tbi;
  fTbNoiseEnd = tbf;
}

void LAPNoiseSubtractionTask::SetGainThreshold(Double_t val) { fGainThreshold = val; }

void LAPNoiseSubtractionTask::FindReferencePad()
{
  auto yDiffMax = 0;
  auto yDiffMin = DBL_MAX;

  Int_t nPads = fPadArray -> GetEntries();

  for (auto iPad = 0; iPad < nPads; iPad++) {
    auto pad = (KBPad *) fPadArray -> At(iPad);
    auto raw = pad -> GetBufferRaw(); 

    auto yMax = 0.;
    auto yMin = DBL_MAX;
    for (auto tb = 1; tb < 510; tb++) {
      auto val = Double_t(raw[tb]);
      if (val > yMax) yMax = val;
      if (val < yMin) yMin = val;
    }

    auto yDiff = yMax - yMin;

    if (TMath::Abs(pad -> GetRow()) < 8)
      continue;

    if (yMin != 0 && yMax < 2000 && yDiff > yDiffMax) {
      yDiffMax = yDiff;
      fIdxPadRef = iPad;
      //cout << "max " << fIdxPadRef << " " << yDiffMax << " " << yDiffMin << " / " << yDiff << " " << yMax << " " << yMin << endl;
    }
    else if (yDiffMax == 0 && yDiff < yDiffMin) {
      yDiffMin = yDiff;
      fIdxPadRef = iPad;
      //cout << "min " << fIdxPadRef << " " << yDiffMax << " " << yDiffMin << " / " << yDiff << " " << yMax << " " << yMin << endl;
    }
  }
}

void LAPNoiseSubtractionTask::CopyRaw(Short_t *in, Double_t *out)
{
  for (auto tb = 0; tb < 512; tb++)
    out[tb] = Double_t(in[tb]);
}

Double_t LAPNoiseSubtractionTask::BaseLineCorrection(Double_t *out, Int_t tbi, Int_t tbf)
{
  Double_t baseLine = 0.;
  for (auto tb = tbi; tb < tbf; tb++)
    baseLine +=  out[tb];

  baseLine = baseLine/(tbf - tbi + 1);
  for (auto tb = 0; tb < 512; tb++)
    out[tb] = out[tb] - baseLine;

  return baseLine;
}

Double_t LAPNoiseSubtractionTask::NoiseAmplitudeCorrection(Double_t *out, Double_t *ref, Int_t tbi, Int_t tbf)
{
  Double_t sum1 = 0.;
  Double_t sum2 = 0.;

  for (auto tb = tbi; tb < tbf; tb++) {
    auto valRef = ref[tb];
    auto val = out[tb];
    sum1 += valRef * val;
    sum2 += valRef * valRef;
  }

  Double_t amp = sum2/sum1;

  for (auto tb = 0; tb < 512; tb++)
    out[tb] = out[tb] - ref[tb]/amp;

  if (amp < fGainThreshold && amp > 0.1)
    for (auto tb = 0; tb < 512; tb++)
      out[tb] = amp * out[tb];
  else
    amp = 0;

  return amp;
}
