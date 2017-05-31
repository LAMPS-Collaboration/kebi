#include "LADriftElectronTask.hh"

#include "KBRun.hh"
#include "KBMCStep.hh"

#include "TCanvas.h"
#include "TRandom.h"
#include "TMath.h"

#include <iostream>
using namespace std;

ClassImp(LADriftElectronTask)

LADriftElectronTask::LADriftElectronTask()
:KBTask("LADriftElectronTask","")
{
}

bool LADriftElectronTask::Init()
{
  KBRun *run = KBRun::GetRun();

  fMCStepArray = (TClonesArray *) run -> GetBranch("MCStep");
  fTpc = (KBTpc *) run -> GetDetector();

  fNPlanes = fTpc -> GetNPlanes();

  auto par = run -> GetParameterContainer();
  par -> GetParDouble("gasDriftVelocity", fDriftVelocity);
  par -> GetParDouble("gasCoefLongDiff", fCoefLD);
  par -> GetParDouble("gasCoefTranDiff", fCoefTD);
  par -> GetParDouble("gasEIonize", fEIonize);
  par -> GetParInt("nElInCluster", fNElInCluster);

  TString gemDataFile;
  par -> GetParString("tpcGEMDataFile", gemDataFile);
  auto gemFile = new TFile(gemDataFile, "read");
  fGainFunction = (TF1*) gemFile -> Get("gainFit");
  fGainZeroRatio = (((TObjString *) ((TList *) gemFile -> GetListOfKeys()) -> At(2)) -> GetString()).Atof();
  fDiffusionFunction = (TH2D*) ((TCanvas*) gemFile -> Get("diffusion")) -> FindObject("distHist");

  par -> GetParInt("nTbs", fNTbs);
  par -> GetParDouble("tbTime", fTbTime);

  fPadArray = new TClonesArray("KBPad");
  run -> RegisterBranch("Pad", fPadArray, fPersistency);

  return true;
}

void LADriftElectronTask::Exec(Option_t*)
{
  fPadArray -> Delete();
  for (auto iPlane = 0; iPlane < fNPlanes; iPlane++)
    fTpc -> GetPadPlane(iPlane) -> Clear();

  Int_t nMCSteps = fMCStepArray -> GetEntries();
  for (Int_t iStep = 0; iStep < nMCSteps; iStep++) {
    KBMCStep* step = (KBMCStep*) fMCStepArray -> At(iStep);

    auto xMC = step -> GetX();
    auto yMC = step -> GetY();
    auto zMC = step -> GetZ();
    auto edep = step -> GetEdep();

    Double_t iMC, jMC, kMC;
    fTpc -> XYZToIJK(xMC, yMC, zMC, iMC, jMC, kMC);

    Int_t planeID;
    Double_t kPlane;
    fTpc -> GetDriftPlane(kMC, planeID, kPlane);
    if (planeID == -1)
      continue;

    auto lDrift = std::abs(kPlane - kMC);
    auto tDrift = lDrift/fDriftVelocity;
    auto sigmaLD = fCoefLD * sqrt(lDrift);
    auto sigmaTD = fCoefTD * sqrt(lDrift);

    Int_t nElectrons = Int_t(edep/fEIonize);

    for (auto iElectron = 0; iElectron < nElectrons; iElectron++) {
      Double_t dr    = gRandom -> Gaus(0, sigmaTD);
      Double_t angle = gRandom -> Uniform(2*TMath::Pi());

      auto di = dr*TMath::Cos(angle);
      auto dj = dr*TMath::Sin(angle);
      auto dt = gRandom -> Gaus(0,sigmaLD)/fDriftVelocity;

      auto tDriftTotal = tDrift + std::abs(dt);
      Int_t tb = (Int_t)(tDriftTotal/fTbTime);

      if (tb > fNTbs) 
        continue;

      Int_t gain = fGainFunction -> GetRandom() * (1 - fGainZeroRatio);
      if (gain <= 0)
        continue;

      Int_t nElClusters = gain/fNElInCluster;
      Int_t gainRemainder = gain%fNElInCluster;

      Double_t iDiffGEM, jDiffGEM;
      for (Int_t iElCluster = 0; iElCluster < nElClusters; iElCluster++) {
        fDiffusionFunction -> GetRandom2(iDiffGEM, jDiffGEM);
        fTpc -> GetPadPlane(planeID) -> FillBufferIn(iMC+di+iDiffGEM*10, jMC+dj+jDiffGEM*10, tb, fNElInCluster);
      }
      fDiffusionFunction -> GetRandom2(iDiffGEM, jDiffGEM);
      fTpc -> GetPadPlane(planeID) -> FillBufferIn(iMC+di+iDiffGEM*10, jMC+dj+jDiffGEM*10, tb, gainRemainder);
    }
  }

  Int_t idx = 0;
  Int_t idxLast = 0;
  for (auto iPlane = 0; iPlane < fNPlanes; iPlane++) {
    KBPad *pad;
    TIter itChannel(fTpc -> GetPadPlane(iPlane) -> GetChannelArray());
    while ((pad = (KBPad *) itChannel.Next())) {
      if (pad -> IsActive() == false)
        continue;

      auto padSave = new ((*fPadArray)[idx]) KBPad();
      padSave -> SetPad(pad);
      padSave -> CopyPadData(pad);
      idx++;
    }
    cout << "  [" << this -> GetName() << "] Number of fired pads in plane-" << iPlane << ": " << idx - idxLast << endl;
    idxLast = idx;
  }
  
  return;
}

void LADriftElectronTask::SetPadPersistency(bool persistence) { fPersistency = persistence; }
