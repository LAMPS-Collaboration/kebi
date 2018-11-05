#include "KBPulseGenerator.hh"
#include "TSystem.h"
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

ClassImp(KBPulseGenerator)

KBPulseGenerator* KBPulseGenerator::fInstance = nullptr;

KBPulseGenerator* KBPulseGenerator::GetPulseGenerator(TString fileName) {
  if (fInstance != nullptr)
    return fInstance;
  return new KBPulseGenerator(fileName);
}

KBPulseGenerator::KBPulseGenerator()
{
  TString kebi = gSystem -> Getenv("KEBIPATH");
  TString fileName = "pulser_464ns.dat";
  Initialize(fileName);
}

KBPulseGenerator::KBPulseGenerator(TString fileName)
{
  Initialize(fileName);
}

void KBPulseGenerator::Initialize(TString fileName)
{
  fInstance = this;

  TString kebi = gSystem -> Getenv("KEBIPATH");

  fileName = kebi + "/input/" + fileName;

  cout << "[KBPulseGenerator::Initialize] Using " << fileName << endl;
  ifstream file(fileName);
  string line;

  while (getline(file, line) && line.find("#") == 0) {}
  istringstream ss(line);
  ss >> fShapingTime >> fNumDataPoints >> fStepSize >> fNumAscending >> fNDFTbs;

  if (fNumDataPoints < 20 || fStepSize > 1)
  {
    cout << "*** Error occured while initializing the pulse!" << endl;
    cout << "*** Check file: " << fileName << endl;
    return;
  }

  fPulseData = new KBSamplePoint[fNumDataPoints];

  Double_t max = 0;
  for (Int_t iData = 0; iData < fNumDataPoints; iData++)
  {
    getline(file, line);
    if (line.find("#") == 0) {
      iData--;
      continue;
    }

    fPulseData[iData].Init(line);
    Double_t value = fPulseData[iData].fValue;
    if (iData == 0)
      fThresholdRatio = value;

    if (value > max) {
      max = value;
      fTbAtMax = iData * fStepSize;
    }
  }

  Double_t c = 1./max;
  Double_t valuePre = 0, valueCur = 0;
  fTbAtThreshold = 0;
  fTbAtTail = 0;

  for (Int_t iData = 0; iData < fNumDataPoints; iData++)
  {
    fPulseData[iData].fValue = c * fPulseData[iData].fValue;

    valuePre = valueCur;
    valueCur = fPulseData[iData].fValue;

    if (fTbAtThreshold == 0 && valueCur > fThresholdRatio)
    {
      fTbAtThreshold = iData * fStepSize;
      Int_t next = iData + 1/fStepSize;
      fThresholdTbStep = fPulseData[next].fValue - fPulseData[iData].fValue;
    }

    if (fTbAtTail == 0 && valueCur < valuePre && valueCur < 0.1)
      fTbAtTail = iData * fStepSize;
  }

  file.close();
}

Double_t 
KBPulseGenerator::Pulse(Double_t x, Double_t amp, Double_t tb0)
{
  Double_t tb = x - tb0;
  if (tb < 0) 
    return 0;

  Int_t tbInStep = tb / fStepSize;
  if (tbInStep > fNumDataPoints - 2) 
    return 0;

  Double_t r = (tb / fStepSize) - tbInStep;
  Double_t val = r * fPulseData[tbInStep + 1].fValue + (1 - r) * fPulseData[tbInStep].fValue;

  return amp * val;
}

Double_t 
KBPulseGenerator::PulseF1(Double_t *x, Double_t *par)
{
  Double_t tb = x[0] - par[1];
  if (tb < 0) 
    return 0;

  Int_t tbInStep = tb / fStepSize;
  if (tbInStep > fNumDataPoints - 2)
    return 0;

  Double_t r = (tb / fStepSize) - tbInStep;
  Double_t val = r * fPulseData[tbInStep + 1].fValue + (1 - r) * fPulseData[tbInStep].fValue;

  return par[0] * val;
}

TF1*
KBPulseGenerator::GetPulseFunction(TString name)
{
  if (name.IsNull()) 
    name = Form("STPulse_%d", fNumF1++);
  TF1* f1 = new TF1(name, this, &KBPulseGenerator::PulseF1, 0, 512, 2, "KBPulseGenerator", "PulseF1");
  return f1;
}

   Int_t  KBPulseGenerator::GetShapingTime()     { return fShapingTime;     }
Double_t  KBPulseGenerator::GetTbAtThreshold()   { return fTbAtThreshold;   }
Double_t  KBPulseGenerator::GetTbAtTail()        { return fTbAtTail;        }
Double_t  KBPulseGenerator::GetTbAtMax()         { return fTbAtMax;         }
   Int_t  KBPulseGenerator::GetNumAscending()    { return fNumAscending;    }
Double_t  KBPulseGenerator::GetThresholdTbStep() { return fThresholdTbStep; }
   Int_t  KBPulseGenerator::GetNumDataPoints()   { return fNumDataPoints;   }
Double_t  KBPulseGenerator::GetStepSize()        { return fStepSize;        }

KBSamplePoint **KBPulseGenerator::GetPulseData()  { return &fPulseData; }

void
KBPulseGenerator::Print()
{
  cout << "[KBPulseGenerator INFO]" << endl;
  cout << " == Shaping time : " << fShapingTime << " ns" << endl;
  cout << " == Number of data points : " << fNumDataPoints << endl;
  cout << " == Step size between data points : " << fStepSize << endl;
  cout << " == Threshold for one timebucket step : " << fThresholdTbStep << endl; 
  cout << " == Number of timebucket while rising : " << fNumAscending << endl;
  cout << " == Timebucket at threshold (" << setw(3) << fThresholdRatio 
       << " of peak) : " << fTbAtThreshold << endl; 
  cout << " == Timebucket at peak : " << fTbAtMax << endl; 
  cout << " == Timebucket difference from threshold to peak : " 
       << fTbAtMax - fTbAtThreshold << endl; 
  cout << " == Number of degree of freedom : " << fNDFTbs << endl;
}

void
KBPulseGenerator::SavePulseData(TString name, Bool_t smoothTail)
{
  Double_t max = 0;
  for (Int_t iData = 0; iData < fNumDataPoints; iData++) {
    if (fPulseData[iData].fValue > max)
      max = fPulseData[iData].fValue;
  }

  Double_t c = 1/max;
  if (max != 1) {
    for (Int_t iData = 0; iData < fNumDataPoints; iData++)
      fPulseData[iData].fValue = c * fPulseData[iData].fValue;
  }

  if (smoothTail) {
    if (fTailFunction == nullptr)
      fTailFunction = new TF1("tail", "x*landau(0)", fTbAtTail - 1, fNumDataPoints - 1);

    fTailFunction -> SetParameters(1, 4, 1);

    if (fTailGraph == nullptr)
      fTailGraph = new TGraph();

    fTailGraph -> Clear();
    fTailGraph -> Set(0);

    Int_t iTailStart = (Int_t)(fTbAtTail/fStepSize);
    for (Int_t iData = iTailStart; iData < fNumDataPoints; iData++)
      fTailGraph -> SetPoint(fTailGraph -> GetN(), iData * fStepSize, fPulseData[iData].fValue);

    fTailGraph -> Fit(fTailFunction, "R");

    for (Int_t iData = iTailStart; iData < fNumDataPoints; iData++)
      fPulseData[iData].fValue = fTailFunction -> Eval(iData * fStepSize);
  }

  ofstream file(name.Data());
  file << "#(shaping time) (number of data points) (step size) (rising tb threshold number) (default ndf)" << endl;
  file << fShapingTime << " " << fNumDataPoints << " " << fStepSize << " " << fNumAscending << " " << fNDFTbs << endl;

  file << "#(value) (rms) (total weight)" << endl;
  for (Int_t iData = 0; iData < fNumDataPoints; iData++) {
    KBSamplePoint sample = fPulseData[iData];
    file << sample.GetSummary() << endl;
  }

  file.close();
}
