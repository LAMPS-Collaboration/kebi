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

  KBParameterContainer *par = run -> GetParameterContainer();
  KBDetector *det = run -> GetDetector();

  fNPlanes = det -> GetNPlanes();
  par -> GetParInt("nTbs", fNTbs);
  par -> GetParDouble("eVToADC", feVToADC);

  fPadArray = (TClonesArray *) run -> GetBranch("Pad");

  KBPulseGenerator *pulseGen = new KBPulseGenerator();
  fPulseFunction = pulseGen -> GetPulseFunction();
  fPulseFunction -> SetParameters(feVToADC,0);

  return true;
}

void LAElectronicsTask::Exec(Option_t*)
{
  Int_t nPads = fPadArray -> GetEntries();
  for (Int_t iPad = 0; iPad < nPads; iPad++) {
    KBPad *pad = (KBPad *) fPadArray -> At(iPad);
    Double_t *in = pad -> GetBufferIn();
    Double_t out[512] = {0};

    for (Int_t iTb = 0; iTb < fNTbs; iTb++) {
      if (in[iTb] == 0)
        continue;

      for (Int_t iTb2 = 0; iTb2 < 100; iTb2++) {
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
