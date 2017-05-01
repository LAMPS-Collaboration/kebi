#include "KBRun.hh"
#include "LAPDecoderTask.hh"

#include "TFile.h"

#include <iostream>
#include <fstream>
using namespace std;

#include "dirent.h"

ClassImp(LAPDecoderTask)

LAPDecoderTask::LAPDecoderTask()
:KBTask("LAPDecoderTask","")
{
  fDecoder = new GETDecoder();
} 

bool LAPDecoderTask::Init()
{
  KBRun *run = KBRun::GetRun();

  auto tpc = (KBTpc *) run -> GetDetector();
  fPadPlane = tpc -> GetPadPlane();

  auto par = run -> GetParameterContainer();

  fPadArray = new TClonesArray("KBPad");
  run -> RegisterBranch("Pad", fPadArray, fPersistency);

  TString padMapFileName;
  par -> GetParString("padMap", padMapFileName);
  fPadMap.open(padMapFileName.Data());

  if (fDecoder -> GetNumData() == 0) {
    cout << "[LAPDecoderTask] Input file is not set!" << endl;
    return false;
  }

  fDecoder -> SetData(0);

  if (fNEvents == -1) {
    fDecoder -> GoToEnd();
    fNEvents = fDecoder -> GetNumFrames();
  }

  run -> SetEntries(fNEvents);
  
  return true;
}

void LAPDecoderTask::Exec(Option_t*)
{
  fPadArray -> Delete();

  Long64_t currentEntry = KBRun::GetRun() -> GetCurrentEventID();
  Int_t idx = 0;

  Int_t nChannels = 68;

  auto cobo = fDecoder -> GetCoboFrame(currentEntry);

  for (auto iAsAd = 0; iAsAd < 4; iAsAd++) {
    auto frame = cobo -> GetFrame(iAsAd);
    for (auto iAGET = 0; iAGET < 4; iAGET++) {
      for (auto iChannel = 0; iChannel < 68; iChannel++) {
        if (iAsAd == 0 && iAGET == 2)
          continue;

        Int_t asad, aget, channel, padID;
        bool foundPad = false;
        fPadMap.clear();
        fPadMap.seekg(0, ios::beg);
        while (fPadMap >> asad >> aget >> channel >> padID) {
          if (asad == iAsAd && aget == iAGET && channel == iChannel) {
            foundPad = true;
            break;
          }
        }

        if (!foundPad)
          padID = -90000-(1000*0+100*iAGET+iChannel);

        Int_t *sample = frame -> GetSample(iAGET, iChannel);

        Short_t copy[512] = {0};
        for (auto iTb = 0; iTb < 512; iTb++) {
          Short_t value = sample[iTb];
          copy[iTb] = value;
        }

        auto pad = fPadPlane -> GetPad(padID);
        if (pad == nullptr)
          continue;

        auto padSave = new ((*fPadArray)[idx]) KBPad();
        padSave -> SetPad(pad);
        padSave -> SetBufferRaw(copy);
        idx++;
      }
    }
  }

  cout << "  [" << this -> GetName() << "] Event-" << currentEntry << ": " << idx << " channels." << endl;
  
  return;
}

void LAPDecoderTask::SetPadPersistency(bool persistence) { fPersistency = persistence; }

void LAPDecoderTask::ReadDirectory(TString directoryName)
{
  if (directoryName[directoryName.Sizeof()-2] != '/')
    directoryName = directoryName + "/";

  auto runID = KBRun::GetRun() -> GetRunID();
  Int_t nEvents = 0; 
  DIR *dir;
  struct dirent *ent;

  if ((dir = opendir (directoryName.Data())) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      TString fileName = ent -> d_name;

      if (fileName.Index(Form("run_%04d",runID)) < 0)
        continue;

      if (fileName.Index(".root") >= 0) {
        cout << "Meta data : " << directoryName+fileName << endl;
        LoadMetaData(directoryName+fileName);
        continue;
      } else if (fileName.Index(".numEvents") >= 0) {
        ifstream tempFile(directoryName+fileName);
        tempFile >> nEvents;
        continue;
      }
      AddData(directoryName+fileName);
    }
    closedir (dir);
  }
  else
    cout << "Cannot read directory " << directoryName << endl;

  if (nEvents != 0)
    SetNumEvents(nEvents);
}

void LAPDecoderTask::LoadMetaData(TString name)
{
  fDecoder -> SetData(0);
  fDecoder -> LoadMetaData(name);
}

void LAPDecoderTask::AddData(TString name) { fDecoder -> AddData(name); }
void LAPDecoderTask::SetNumEvents(Long64_t nEvents) { fNEvents = nEvents; }
