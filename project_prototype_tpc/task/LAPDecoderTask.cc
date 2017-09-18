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

  KBTpc *tpc = (KBTpc *) run -> GetDetector();
  fPadPlane = tpc -> GetPadPlane();

  KBParameterContainer *par = run -> GetParameterContainer();

  fPadArray = new TClonesArray("KBPad");
  run -> RegisterBranch("Pad", fPadArray, fPersistency);

  TString padMapFileName = par -> GetParString("padMap");
  fPadMap.open(padMapFileName.Data());

  if (fDecoder -> GetNumData() == 0) {
    cout << "[LAPDecoderTask] Input file is not set!" << endl;
    return false;
  }

  fDecoder -> SetData(0);

  if (fNumEvents == -1) {
    fDecoder -> GoToEnd();
    fNumEvents = fDecoder -> GetNumFrames();
  }

  run -> SetEntries(fNumEvents);
  
  return true;
}

void LAPDecoderTask::Exec(Option_t*)
{
  fPadArray -> Delete();

  Long64_t currentEntry = KBRun::GetRun() -> GetCurrentEventID();
  Int_t idx = 0;

  GETCoboFrame *cobo = fDecoder -> GetCoboFrame(currentEntry);

  for (Int_t iAsAd = 0; iAsAd < 4; iAsAd++) {
    GETBasicFrame *frame = cobo -> GetFrame(iAsAd);
    Int_t AsAdID = frame -> GetAsadID();
    for (Int_t iAGET = 0; iAGET < 4; iAGET++) {
      for (Int_t iChannel = 0; iChannel < 68; iChannel++) {
        if (AsAdID == 0 && iAGET == 2)
          continue;

        Int_t asad, aget, channel, padID;
        bool foundPad = false;
        fPadMap.clear();
        fPadMap.seekg(0, ios::beg);
        while (fPadMap >> asad >> aget >> channel >> padID) {
          if (asad == AsAdID && aget == iAGET && channel == iChannel) {
            foundPad = true;
            break;
          }
        }

        if (!foundPad)
          padID = -90000-(1000*0+100*iAGET+iChannel);

        Int_t *sample = frame -> GetSample(iAGET, iChannel);

        Short_t copy[512] = {0};
        Double_t copy2[512] = {0};
        for (Int_t iTb = 0; iTb < 512; iTb++) {
          Short_t value = sample[iTb];
          copy[iTb] = value;
          copy2[iTb] = (Double_t) value;
        }

        KBPad *pad = fPadPlane -> GetPad(padID);
        if (pad == nullptr)
          continue;

        KBPad *padSave = new ((*fPadArray)[idx]) KBPad();
        padSave -> SetPad(pad);
        padSave -> SetBufferRaw(copy);
        padSave -> SetBufferOut(copy2);
        idx++;
      }
    }
  }

  cout << "  [" << this -> GetName() << "] Event-" << currentEntry << ": " << idx << " channels." << endl;
  
  return;
}

void LAPDecoderTask::SetPadPersistency(bool persistence) { fPersistency = persistence; }

void LAPDecoderTask::ReadDirectory(TString pathToData)
{
  if (pathToData[pathToData.Sizeof()-2] != '/')
    pathToData = pathToData + "/";

  Int_t runID = KBRun::GetRun() -> GetRunID();

  vector<TString> fileList;

  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(pathToData.Data())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      TString fileName = ent -> d_name;
      if (fileName.Index(Form("run_%04d.",runID)) == 0)
        fileList.push_back(fileName);
    }
    closedir (dir);
  }
  if(fileList.size() == 0) {
    cout << Form("No matching run_%04d",runID) << endl;
    return;
  }

  sort(fileList.begin(), fileList.end(), less<TString>());

  for (auto fileName : fileList)
    AddData(pathToData+fileName);

  pathToData = pathToData + Form("run_%04d/metadata/",runID);
  if ((dir = opendir (pathToData.Data())) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      TString fileName = ent -> d_name;
      if (fileName.Index(Form("run_%04d",runID)) >= 0 && fileName.Index(".root") >= 0) {
        cout << "Meta data : " << pathToData+fileName << endl;
        LoadMetaData(pathToData+fileName);
        break;
      }
    }
    closedir (dir);
  }
}

void LAPDecoderTask::LoadMetaData(TString name)
{
  fDecoder -> SetData(0);
  fDecoder -> LoadMetaData(name);
  fNumEvents = fDecoder -> GetNumFrames();
}

void LAPDecoderTask::AddData(TString name) { fDecoder -> AddData(name); }
void LAPDecoderTask::SetNumEvents(Long64_t numEvents) { fNumEvents = numEvents; }
