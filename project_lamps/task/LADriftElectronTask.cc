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

  KBParameterContainer *par = run -> GetParameterContainer();
  par -> GetParDouble("gasDriftVelocity", fDriftVelocity);
  par -> GetParDouble("gasCoefLongDiff", fCoefLD);
  par -> GetParDouble("gasCoefTranDiff", fCoefTD);
  par -> GetParDouble("gasEIonize", fEIonize);
  par -> GetParInt("nElInCluster", fNElInCluster);

  TString gemDataFile;
  par -> GetParString("tpcGEMDataFile", gemDataFile);
  TFile *gemFile = new TFile(gemDataFile, "read");
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
  for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++)
    fTpc -> GetPadPlane(iPlane) -> Clear();

  Int_t nMCSteps = fMCStepArray -> GetEntries();
  for (Int_t iStep = 0; iStep < nMCSteps; iStep++) {
    KBMCStep* step = (KBMCStep*) fMCStepArray -> At(iStep);

    Double_t xMC = step -> GetX();
    Double_t yMC = step -> GetY();
    Double_t zMC = step -> GetZ();
    Double_t edep = step -> GetEdep();

    Double_t iMC, jMC, kMC;
    fTpc -> XYZToIJK(xMC, yMC, zMC, iMC, jMC, kMC);

    Int_t planeID;
    Double_t kPlane;
    fTpc -> GetDriftPlane(kMC, planeID, kPlane);
    if (planeID == -1)
      continue;

    Double_t lDrift = std::abs(kPlane - kMC);
    Double_t tDrift = lDrift/fDriftVelocity;
    Double_t sigmaLD = fCoefLD * sqrt(lDrift);
    Double_t sigmaTD = fCoefTD * sqrt(lDrift);

    Int_t nElectrons = Int_t(edep/fEIonize);

    for (Int_t iElectron = 0; iElectron < nElectrons; iElectron++) {
      Double_t dr    = gRandom -> Gaus(0, sigmaTD);
      Double_t angle = gRandom -> Uniform(2*TMath::Pi());

      Double_t di = dr*TMath::Cos(angle);
      Double_t dj = dr*TMath::Sin(angle);
      Double_t dt = gRandom -> Gaus(0,sigmaLD)/fDriftVelocity;

      Double_t tDriftTotal = tDrift + std::abs(dt);
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
  for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++) {
    KBPad *pad;
    TIter itChannel(fTpc -> GetPadPlane(iPlane) -> GetChannelArray());
    while ((pad = (KBPad *) itChannel.Next())) {
      if (pad -> IsActive() == false)
        continue;

      KBPad *padSave = new ((*fPadArray)[idx]) KBPad();
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
