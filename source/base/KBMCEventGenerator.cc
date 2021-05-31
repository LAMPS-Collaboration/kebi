#include "KBMCEventGenerator.hh"
#include "KBG4RunManager.hh"
#include "KBMCTrack.hh"
#include "KBRun.hh"
#include "KBGlobal.hh"
#include "KBParameterContainer.hh"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TObjString.h"

ClassImp(KBMCEventGenerator)

KBMCEventGenerator::KBMCEventGenerator()
: KBGear()
{
  fRandom.SetSeed(time(0));
  CreateParameterContainer();
}

KBMCEventGenerator::KBMCEventGenerator(TString fileName)
: KBMCEventGenerator()
{
  AddGenFile(fileName);
}

KBMCEventGenerator::~KBMCEventGenerator()
{
}

void KBMCEventGenerator::AddGenFile(TString fileName)
{
  fInputFileNameArray.push_back(fileName);

  if (fNumEvents==0)
  {
    TObjArray *tokens = fileName.Tokenize("/");
    TString justName = ((TObjString *) tokens->At(tokens->GetEntries()-1))->GetString();
    if (tokens->GetEntries()==1)
      justName = fileName;

         if (justName.EndsWith(".gen")) justName.ReplaceAll(".gen","");
    else if (justName.EndsWith(".txt")) justName.ReplaceAll(".txt","");
    else if (justName.EndsWith(".dat")) justName.ReplaceAll(".dat","");

    fGenName = justName;

    if (fInputFile.is_open())
      fInputFile.close();

    fInputFile.open(fileName.Data());

    TString me;
    fInputFile >> me;
    me.ToLower();

         if (me == "p") fReadMomentumOrEnergy = true;
    else if (me == "e") fReadMomentumOrEnergy = false;

    fInputFile >> fNumEvents;

    cout_info << fileName << " containing " << fNumEvents << " events, initialized with " << me << endl;
  }
  else {
    ifstream inputFile(fileName.Data());

    TString me;
    int numEvents;

    inputFile >> me;
    inputFile >> numEvents;

    fNumEvents += numEvents;

    cout_info << fileName << " added(" << fInputFileNameArray.size() << ") containing " << numEvents << " events." << endl;
    cout_info << "Event generator containing " << fNumEvents << " events."<< endl;
  }
}

bool KBMCEventGenerator::ReadNextEvent(Double_t &vx, Double_t &vy, Double_t &vz)
{
  Int_t eventID;
  if (fInputFile >> eventID >> fNumTracks >> vx >> vy >> vz)
  {
    fCurrentEventID++;
    fCurrentTrackID = 0;
    return true;
  }
  else if (fCurrentEventID<fNumEvents-1)
  {
    fCurrentInputFileIndex++;

    fInputFile.close();
    fInputFile.open((fInputFileNameArray.at(fCurrentInputFileIndex)).Data());

    TString me;
    int numEvents;
    fInputFile >> me;
    fInputFile >> numEvents;
    me.ToLower();

         if (me == "p") fReadMomentumOrEnergy = true;
    else if (me == "e") fReadMomentumOrEnergy = false;

    return ReadNextEvent(vx,vy,vz);
  }

  return false;
}

bool KBMCEventGenerator::ReadNextTrack(Int_t &pdg, Double_t &px, Double_t &py, Double_t &pz)
{
  if (fCurrentTrackID >= fNumTracks)
    return false;

  fInputFile >> pdg >> px >> py >> pz;
  fCurrentTrackID++;

  return true;
}

void KBMCEventGenerator::Summary()
{
  int pdg;
  double vx, vy, vz, px, py, pz;

  auto fileName = Form("%s.gen.root",fGenName.Data());
  cout_info << "Creating summary file " << fileName << endl;

  TH1D *histPhiTheta[3][2];
  histPhiTheta[0][0] = new TH1D("hist_phi_x",";#phi_x",100,-180,180);   histPhiTheta[0][0] -> SetMinimum(0);
  histPhiTheta[0][1] = new TH1D("hist_theta_x",";#theta_x",100,0,180);  histPhiTheta[0][1] -> SetMinimum(0);
  histPhiTheta[1][0] = new TH1D("hist_phi_y",";#phi_y",100,-180,180);   histPhiTheta[1][0] -> SetMinimum(0);
  histPhiTheta[1][1] = new TH1D("hist_theta_y",";#theta_y",100,0,180);  histPhiTheta[1][1] -> SetMinimum(0);
  histPhiTheta[2][0] = new TH1D("hist_phi_z",";#phi_z",100,-180,180);   histPhiTheta[2][0] -> SetMinimum(0);
  histPhiTheta[2][1] = new TH1D("hist_theta_z",";#theta_z",100,0,180);  histPhiTheta[2][1] -> SetMinimum(0);

  auto fileGen = new TFile(fileName,"recreate");
  auto treeEvent = new TTree("event","");
       treeEvent -> Branch("vx",&vx);
       treeEvent -> Branch("vy",&vy);
       treeEvent -> Branch("vz",&vz);
       treeEvent -> Branch("ntracks",&fNumTracks);
  auto treeTrack = new TTree("track","");
       treeTrack -> Branch("pdg",&pdg);
       treeTrack -> Branch("px",&px);
       treeTrack -> Branch("py",&py);
       treeTrack -> Branch("pz",&pz);


  while (ReadNextEvent(vx,vy,vz)) {
    treeEvent -> Fill();
    while (ReadNextTrack(pdg, px, py, pz)) {
      treeTrack -> Fill();

      KBVector3 momentum(px,py,pz);
      for (auto iaxis : {0,1,2}) {
        auto axis = KBVector3::kX;
        if (iaxis==1) axis = KBVector3::kY;
        if (iaxis==2) axis = KBVector3::kZ;
        momentum.SetReferenceAxis(axis);
        auto momentumA = momentum.GetIJK();
        histPhiTheta[iaxis][0] -> Fill(momentumA.Phi()*TMath::RadToDeg());
        histPhiTheta[iaxis][1] -> Fill(momentumA.Theta()*TMath::RadToDeg());
      }
    }
  }

  treeEvent -> Write();
  treeTrack -> Write();

  auto cvs_event = new TCanvas("event",fGenName+" event",20*1,20*1,800,700);
  cvs_event -> Divide(2,2);
  cvs_event -> cd(1); treeEvent -> Draw("vx");
  cvs_event -> cd(2); treeEvent -> Draw("vy");
  cvs_event -> cd(3); treeEvent -> Draw("vz");
  cvs_event -> cd(4); treeEvent -> Draw("ntracks");
  cvs_event -> Write();

  auto cvs_track = new TCanvas("track",fGenName+" track",20*2,20*2,800,700);
  cvs_track -> Divide(2,2);
  cvs_track -> cd(1); treeTrack -> Draw("px");
  cvs_track -> cd(2); treeTrack -> Draw("py");
  cvs_track -> cd(3); treeTrack -> Draw("pz");
  cvs_track -> cd(4); treeTrack -> Draw("sqrt(px*px+py*py+pz*pz)");
  cvs_track -> Write();

  auto cvs_track_axis = new TCanvas("track_axis",fGenName+" track_axis",20*3,20*3,1200,700);
  cvs_track_axis -> Divide(3,2);
  cvs_track_axis -> cd(1); histPhiTheta[0][0] -> Draw();
  cvs_track_axis -> cd(2); histPhiTheta[1][0] -> Draw();
  cvs_track_axis -> cd(3); histPhiTheta[2][0] -> Draw();
  cvs_track_axis -> cd(4); histPhiTheta[0][1] -> Draw();
  cvs_track_axis -> cd(5); histPhiTheta[1][1] -> Draw();
  cvs_track_axis -> cd(6); histPhiTheta[2][1] -> Draw();
  cvs_track_axis -> Write();

  fileGen -> Close();
}


const char *KBMCEventGenerator::SetDirectory(TString dirName) {
  fDirName = dirName;
  if (!fDirName.EndsWith("/"))
    fDirName = fDirName + "/";
  if (fDirName[0] == '$')
    fDirName = KBRun::ConfigureEnv(fDirName);
  if (fDirName[0] == '.')
    fDirName = TString(fDirName(1,fDirName.Sizeof()));
  if (fDirName[0] != '/' && fDirName != '~')
    fDirName = Form("%s/%s",gSystem->pwd(),fDirName.Data());

  cout_info << "Generator directory is set to " << fDirName << endl;
  gSystem -> mkdir(fDirName);

  return fDirName.Data();
}

const char *KBMCEventGenerator::CreateGenFile(const char *runName, int runID, int numEvents)
{
  fGenName = Form("%s%04d",runName,runID);

  auto genFileName = Form("%s%s.gen",fDirName.Data(),fGenName.Data());
  if (fOutputFile.is_open())
    fOutputFile.close();
  fOutputFile.open(genFileName);
  fOutputFile << "p" << endl;
  fOutputFile << numEvents << endl;
  fNumEvents = 0;

  cout_info << "creating " << genFileName << endl;

  fPar -> SetPar("G4MacroFile",fDirName+"geant4_run.mac",true);
  fPar -> SetPar("G4InputFile",genFileName,true);
  fPar -> SetPar("G4OutputFile",Form("$KEBIPATH/data/%s.mc.root",fGenName.Data()),true);
  fPar -> SetPar("MCSetEdepSumTree",false,true);
  fPar -> SetPar("MCStepPersistency",true,true);
  fPar -> SetPar("MCSecondaryPersistency",true,true);
  fPar -> SetPar("MCTrackVertexPersistency",true,true);
  fPar -> SetPar("RunName",runName,true);
  fPar -> SetPar("RunID",runID,true);
  fPar -> SaveAs(fDirName+fGenName);

  return genFileName;
}

void KBMCEventGenerator::AddEvent(int numTracks, double vX, double vY, double vZ)
{
  fOutputFile << fNumEvents << " " << numTracks << " " << vX << " " << vY << " " << vZ << endl;
  fNumEvents++;
}

void KBMCEventGenerator::AddTrack(int pdg, double pX, double pY, double pZ)
{
  fOutputFile << pdg << " " << pX << " " << pY << " " << pZ << endl;
}

void KBMCEventGenerator::AddTrack(TString particleName, double pX, double pY, double pZ)
{
  int pdg = KBRun::GetRun() -> GetParticle(particleName) -> PdgCode();
  AddTrack(pdg, pX, pY, pZ);
}

const char *KBMCEventGenerator::CreateGenRandom(const char *name, int runID, int numEvents, int numTracks, double vX, double vY, double vZ, TString particleNames, double pVal, double theta1, double theta2, double phi1, double phi2, KBVector3::Axis refAxis)
{
  auto genFileName = CreateGenFile(name, runID, numEvents);

  particleNames.ReplaceAll(":",",");
  auto particleNameArray = particleNames.Tokenize(",");
  int numParticles = particleNameArray -> GetEntries();

  for (auto event=0; event<numEvents; ++event) {
    TVector3 posVertex(vX, vY, vZ);
    AddEvent(numTracks, posVertex.X(), posVertex.Y(), posVertex.Z());
    for (auto iTrack=0; iTrack<numTracks; ++iTrack) {
      auto particleName = ((TObjString *) particleNameArray->At(fRandom.Integer(numParticles-1)))->GetString();
      TVector3 momentum(0,0,1);
      momentum.SetTheta(fRandom.Uniform(theta1*TMath::DegToRad(),theta2*TMath::DegToRad()));
      momentum.SetPhi(fRandom.Uniform(phi1*TMath::DegToRad(),phi2*TMath::DegToRad()));
      momentum.SetMag(pVal);
      momentum = KBVector3(refAxis,momentum).GetXYZ();
      if (particleName.IsDigit())
        AddTrack(particleName.Atoi(),momentum.X(),momentum.Y(),momentum.Z());
      else
        AddTrack(particleName,momentum.X(),momentum.Y(),momentum.Z());
    }
  }

  return genFileName;
}
