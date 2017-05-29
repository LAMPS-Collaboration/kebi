#include "KBRun.hh"
#include "LAElectronicsTask.hh"

#include <iostream>
using namespace std;

ClassImp(LAElectronicsTask)

LAElectronicsTask::LAElectronicsTask()
:KBTask("LAElectronicsTask","")
{
}

bool LAElectronicsTask::Init()
{
  KBRun *run = KBRun::GetRun();

  auto par = run -> GetParameterContainer();
  auto det = run -> GetDetector();

  fNPlanes = det -> GetNPlanes();
  par -> GetParInt("nTbs", fNTbs);

  fPadArray = (TClonesArray *) run -> GetBranch("Pad");

  auto pulseGen = new KBPulseGenerator();
  fPulseFunction = pulseGen -> GetPulseFunction();
  fPulseFunction -> SetParameters(0.0001,0);

  return true;
}

void LAElectronicsTask::Exec(Option_t*)
{
  Int_t nPads = fPadArray -> GetEntries();
  for (auto iPad = 0; iPad < nPads; iPad++) {
    auto pad = (KBPad *) fPadArray -> At(iPad);
    Double_t *in = pad -> GetBufferIn();
    Double_t out[512] = {0};

    for (auto iTb = 0; iTb < fNTbs; iTb++) {
      if (in[iTb] == 0)
        continue;

      for (auto iTb2 = 0; iTb2 < 100; iTb2++) {
        Int_t tb = iTb+iTb2;
        if (tb >= fNTbs)
          break;

        out[tb] += in[iTb] * fPulseFunction -> Eval(iTb2);
        if (out[tb] > 3500.)
          out[tb] = 3500;
      }
    }
    pad -> SetBufferOut(out);
  }

  cout << "  [" << this -> GetName() << "]" << endl;
  
  return;
}
