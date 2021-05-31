#include "LHDriftElectronTask.hh"

#include "KBRun.hh"
#include "KBMCStep.hh"

#include "TCanvas.h"
#include "TRandom.h"
#include "TMath.h"
#include "TObjString.h"

#include <iostream>
using namespace std;

ClassImp(LHDriftElectronTask)

LHDriftElectronTask::LHDriftElectronTask()
:KBTask("LADriftElectronTask","")
{
}

bool LHDriftElectronTask::Init()
{
  KBRun *run = KBRun::GetRun();

  fMCStepArray = (TClonesArray *) run -> GetBranch("MCStep0");
  fTpc = (LHTpc *) run -> GetDetectorSystem() -> GetTpc();

  fNPlanes = fTpc -> GetNumPlanes();

  KBParameterContainer *par = run -> GetParameterContainer();
  fDriftVelocity = par -> GetParDouble("gasDriftVelocity");
  fCoefLD = par -> GetParDouble("gasCoefLongDiff");
  fCoefTD = par -> GetParDouble("gasCoefTranDiff");
  fEIonize = par -> GetParDouble("gasEIonize");
  fNElInCluster = par -> GetParInt("nElInCluster");

  if (par->CheckPar("drifte_numEleCluster"))  fNumEleCluster  = par -> GetParInt("drifte_numEleCluster");
  if (par->CheckPar("drifte_numGainCluster")) fNumGainCluster = par -> GetParInt("drifte_numGainCluster");

  TString gemDataFile;
  gemDataFile = par -> GetParString("tpcGEMDataFile");
  TFile *gemFile = new TFile(gemDataFile, "read");
  fGainFunction = (TF1*) gemFile -> Get("gainFit");
  fGainZeroRatio = (((TObjString *) ((TList *) gemFile -> GetListOfKeys()) -> At(2)) -> GetString()).Atof();
  fDiffusionFunction = (TH2D*) ((TCanvas*) gemFile -> Get("diffusion")) -> FindObject("distHist");

  fNTbs = par -> GetParInt("nTbs");
  fTbTime = par -> GetParDouble("tbTime");

  if (par -> CheckPar("selectMCTrack"))
    fSelectedTrackID = par -> GetParInt("selectMCTrack");

  fPadArray = new TClonesArray("KBPad");
  run -> RegisterBranch("Pad", fPadArray, fPersistency);

  return true;
}

void LHDriftElectronTask::Exec(Option_t*)
{
  fPadArray -> Delete();
  for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++)
    fTpc -> GetPadPlane(iPlane) -> Clear();

  Long64_t nMCSteps = fMCStepArray -> GetEntries();
  for (Long64_t iStep = 0; iStep < nMCSteps; ++iStep) {
    KBMCStep* step = (KBMCStep*) fMCStepArray -> At(iStep);
    if (fSelectedTrackID != -1 && fSelectedTrackID != step -> GetTrackID())
      continue;

    Int_t trackID = step -> GetTrackID();
    Double_t edep = step -> GetEdep();
    KBVector3 posMC(step -> GetX(),step -> GetY(),step -> GetZ());
    posMC.SetReferenceAxis(fTpc -> GetEFieldAxis());

    auto plane = fTpc -> GetDriftPlane(posMC.GetXYZ());
    if (plane == nullptr)
      continue;

    Int_t planeID = plane -> GetPlaneID();
    Double_t kPlane = plane -> GetPlaneK();

    Double_t lDrift = std::abs(kPlane - posMC.K());
    Double_t tDrift = lDrift/fDriftVelocity;
    Double_t sigmaLD = fCoefLD * sqrt(lDrift);
    Double_t sigmaTD = fCoefTD * sqrt(lDrift);

    Int_t nElectrons = Int_t(edep/fEIonize);
    Int_t numEleInCluster = nElectrons/fNumEleCluster;
    Int_t numEleInExtCluster = nElectrons - numEleInCluster * fNumEleCluster;

    for (Int_t iEleCluster = 0; iEleCluster < fNumEleCluster+1; iEleCluster++)
    {
      Int_t numEleInCluster0 = numEleInCluster;
      if (iEleCluster==0)
        numEleInCluster0 = numEleInExtCluster;

      Double_t dr    = gRandom -> Gaus(0, sigmaTD);
      Double_t angle = gRandom -> Uniform(2*TMath::Pi());

      Double_t di = dr*TMath::Cos(angle);
      Double_t dj = dr*TMath::Sin(angle);
      Double_t dt = gRandom -> Gaus(0,sigmaLD)/fDriftVelocity;

      Double_t tDriftTotal = tDrift + std::abs(dt);
      Int_t tb = (Int_t)(tDriftTotal/fTbTime);

      if (tb > fNTbs) 
        continue;

      Int_t gain = numEleInCluster0 * fGainFunction -> GetRandom() * (1 - fGainZeroRatio);
      if (gain <= 0)
        continue;

      Int_t numGainInCluster = gain/fNumGainCluster;
      Int_t numGainInExtCluster = gain - numGainInCluster * fNumGainCluster;

      Double_t iDiffGEM, jDiffGEM; 
      if (numGainInCluster>0) {
        for (Int_t iElCluster = 0; iElCluster < fNumGainCluster; iElCluster++) {
          fDiffusionFunction -> GetRandom2(iDiffGEM, jDiffGEM);
          fTpc -> GetPadPlane(planeID) -> FillBufferIn(posMC.I()+di+iDiffGEM*10, posMC.J()+dj+jDiffGEM*10, tb, numGainInCluster, trackID);
        }
      }
      if (numGainInExtCluster>0) {
        fDiffusionFunction -> GetRandom2(iDiffGEM, jDiffGEM);
        fTpc -> GetPadPlane(planeID) -> FillBufferIn(posMC.I()+di+iDiffGEM*10, posMC.J()+dj+jDiffGEM*10, tb, numGainInExtCluster, trackID);
      }
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

    if (fSelectedTrackID != -1)
      kb_info << "From selected MCTrack:" << fSelectedTrackID << ", Number of fired pads in plane-" << iPlane << ": " << idx - idxLast << endl;
    else
      kb_info << "Number of fired pads in plane-" << iPlane << ": " << idx - idxLast << endl;
    idxLast = idx;
  }
  
  return;
}

void LHDriftElectronTask::SetPadPersistency(bool persistence) { fPersistency = persistence; }
