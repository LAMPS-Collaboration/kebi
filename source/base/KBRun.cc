#include "KBRun.hh"

#include "TEnv.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TApplication.h"
#include "TRandom.h"
#include "TGraph.h"

#ifdef ACTIVATE_EVE
#include "TEveViewer.h"
#include "TGLViewer.h"
#include "TEveGeoNode.h"
#include "TEveManager.h"
#include "TEveScene.h"
#include "TEveWindow.h"
#include "TEveWindowManager.h"
#include "TEveGedEditor.h"
#include "TEveBrowser.h"
#include "TRootBrowser.h"
#include "TBrowser.h"
#include "TGTab.h"
#include "TVirtualX.h"
#include "TGWindow.h"
#include "TGeoManager.h"
#include "TRootEmbeddedCanvas.h"
#include "TEvePointSet.h"
#include "TEveLine.h"
#endif

#include "KBHit.hh"
#include "KBContainer.hh"
#include "KBTracklet.hh"
#include "KBMCStep.hh"

#include <unistd.h>
#include <iostream>
#include <ctime>

ClassImp(KBRun)

KBRun* KBRun::fInstance = nullptr;

KBRun* KBRun::GetRun() {
  if (fInstance != nullptr)
    return fInstance;
  return new KBRun();
}

KBRun::KBRun()
:KBTask("KBRun", "KBRun")
{
  fInstance = this;
  fFriendTrees = new TObjArray();
  fPersistentBranchArray = new TObjArray();
  fCvsDetectorPlaneArray = new TObjArray();
  fTemporaryBranchArray = new TObjArray();
  fBranchPtr = new TObject*[1000];
  for (Int_t iBranch = 0; iBranch < 1000; ++iBranch)
    fBranchPtr[iBranch] = nullptr;

  fDetectorSystem = new KBDetectorSystem();
}

TString KBRun::GetKEBIVersion()       { return TString(KEBI_VERSION); }
TString KBRun::GetGETDecoderVersion() { return TString(GETDECODER_VERSION); }
TString KBRun::GetKEBIHostName()      { return TString(KEBI_HOSTNAME); }
TString KBRun::GetKEBIUserName()      { return TString(KEBI_USERNAME); }
TString KBRun::GetKEBIPath()          { return TString(KEBI_PATH); }

void KBRun::PrintKEBI()
{
  cout << "===========================================================================================" << endl;
  cout << "[KEBI] Compiled Information" << endl;
  cout << "-------------------------------------------------------------------------------------------" << endl;
  cout << "  KEBI Version       : " << KBRun::GetKEBIVersion() << endl;
  cout << "  GETDecoder Version : " << KBRun::GetGETDecoderVersion() << endl;
  cout << "  KEBI Host Name     : " << KBRun::GetKEBIHostName() << endl;
  cout << "  KEBI User Name     : " << KBRun::GetKEBIUserName() << endl;
  cout << "  KEBI Path          : " << KBRun::GetKEBIPath() << endl;
  cout << "===========================================================================================" << endl;
}

void KBRun::SetRunName(TString name) { fRunName = name; }
TString KBRun::GetRunName() const { return fRunName; }

void KBRun::SetRunID(Int_t id) { fRunID = id; }
Int_t KBRun::GetRunID() const { return fRunID; }

void KBRun::Print(Option_t *option) const
{
  TString option_print = TString(option);
  option_print.ToLower();

  if (option_print.Index("all") >= 0)
    option_print = "cpdio";

  if (option_print.Index("c") >= 0)
    KBRun::PrintKEBI();

  kb_out << endl;
  kb_out << "===========================================================================================" << endl;
  kb_info << "Run" << endl;
  if (fPar != nullptr && option_print.Index("p") >= 0) {
    kb_out << "-------------------------------------------------------------------------------------------" << endl;
    fPar -> Print();
  }

  if (option_print.Index("d") >= 0) {
  kb_out << "-------------------------------------------------------------------------------------------" << endl;
    fDetectorSystem -> Print();
  }

  kb_out << "-------------------------------------------------------------------------------------------" << endl;
  kb_info << "Input: " << fInputFileName << endl;
  if (fInputTree != nullptr && option_print.Index("i") >= 0) {
    fInputTree -> Print("toponly");

    for (Int_t iFriend = 0; iFriend < fNumFriends; iFriend++)
      GetFriendChain(iFriend) -> Print("toponly");
  }

  kb_out << "-------------------------------------------------------------------------------------------" << endl;
  kb_info << "Output: " << fOutputFileName << endl;
  if (fOutputTree != nullptr && option_print.Index("o") >= 0)
    fOutputTree -> Print("toponly");

  if (!fInitialized) {
    kb_out << "-------------------------------------------------------------------------------------------" << endl;
    kb_warning << "Print() is recommanded to be called after the initialization." << endl;
    kb_warning << "This run is not initialized. Please call Init() before Print()." << endl;

  }

  kb_out << "===========================================================================================" << endl;
}

TString KBRun::ConfigureDataPath(TString name, bool search)
{
  name = ConfigureEnv(name);

  TString pathPWD = getenv("PWD"); pathPWD = pathPWD + "/";
  TString pathKEBI = TString(KEBI_PATH) + "/data/";

  TString fullName;

  if (name == "last") {
    fullName = TString(KEBI_PATH) + "/data/LAST_OUTPUT";
    return fullName;
  }

  if (name.Index(".root") != name.Sizeof()-6)
    name = name + ".root";

  if (search)
  {
    if (name[0] != '.' && name[0] != '/' && name[0] != '$' && name != '~') {
      bool found = false;

      TString pathList[] = {fDataPath, pathPWD, pathPWD+"data/", pathKEBI};

      for (auto path : pathList) {
        fullName = path + name;
        if (CheckFileExistence(fullName,1)) {
          found = true;
          break;
        }
      }
      if (found)
        return fullName;
      else
        return TString();
    }
    else {
      fullName = name;
      if (CheckFileExistence(fullName,1))
        return fullName;
      else
        return TString();
    }
  }
  else {
    if (name[0] != '.' && name[0] != '/' && name[0] != '$' && name != '~') {
      if (fDataPath.IsNull())
        fullName = pathKEBI + name;
      else
        fullName = fDataPath + "/" + name;
    }
    else
      fullName = name;
    return fullName;
  }
}

TString KBRun::ConfigureEnv(TString name)
{
  TString head;
  if (name[0] == '/') head = "/";
  TString fullName = head;

  TObjArray *tokens = name.Tokenize("/");

  for (auto iToken = 0; iToken < tokens -> GetEntries(); ++iToken) {
    TString token = ((TObjString *) tokens -> At(iToken)) -> GetString();
    if (token[0] == '$') {
      TString envName = token(1,token.Sizeof()-2);
      token = gSystem -> Getenv(envName);
    }
    fullName += token + "/";
  }
  fullName = fullName(0,fullName.Sizeof()-2);
  if (name[name.Sizeof()-2] == '/')
    fullName = fullName + "/";

  if (fullName.Index("$") >= 0)
    return ConfigureEnv(fullName);

  return fullName;
}

void KBRun::SetDataPath(TString path) { fDataPath = path; }
TString KBRun::GetDataPath() { return fDataPath; }

void KBRun::SetInputFile(TString fileName, TString treeName) {
  fInputFileName = ConfigureDataPath(fileName,true);
  fInputTreeName = treeName;
}

void KBRun::AddInput(TString fileName) { fInputFileNameArray.push_back(ConfigureDataPath(fileName,true)); }
void KBRun::AddFriend(TString fileName) { fFriendFileNameArray.push_back(ConfigureDataPath(fileName,true)); }
void KBRun::SetInputTreeName(TString treeName) { fInputTreeName = treeName; }

TChain *KBRun::GetInputChain()  const { return fInputTree; }
TChain *KBRun::GetFriendChain(Int_t iFriend)  const { return ((TChain *) fFriendTrees -> At(iFriend)); }

void KBRun::SetOutputFile(TString name) { fOutputFileName = name; }
TTree *KBRun::GetOutputTree() { return fOutputTree; }
void KBRun::SetTag(TString tag) { fTag = tag; }
void KBRun::SetSplit(Int_t split, Long64_t numSplitEntries)
{
  fSplit = split;
  fNumSplitEntries = numSplitEntries;
}

void KBRun::SetIOFile(TString inputName, TString outputName, TString treeName)
{
  SetInputFile(inputName, treeName);
  SetOutputFile(outputName);
}

bool KBRun::Init()
{
  fInitialized = false;

  kb_print << "Initializing" << endl;

  GetDatabasePDG();

  Int_t idxInput = 0;
  if (fInputFileName.IsNull() && fInputFileNameArray.size() != 0) {
    fInputFileName = fInputFileNameArray[0];
    idxInput = 1;
  }

  if (fInputFileName.IsNull() == false) {
    kb_out << endl;
    if (!CheckFileExistence(fInputFileName)) {
      kb_print << "given input file deos not exist!" << endl;
      return false;
    }
    fInputFile = new TFile(fInputFileName, "read");

    if (fInputTreeName.IsNull())
      fInputTreeName = "data";

    fInputTree = new TChain(fInputTreeName);
    fInputTree -> AddFile(fInputFileName);
    kb_print << "Input file : " << fInputFileName << endl;

    Int_t nInputs = fInputFileNameArray.size();
    for (Int_t iInput = idxInput; iInput < nInputs; iInput++) {
      fInputTree -> AddFile(fInputFileNameArray[iInput]);
      kb_print << "Input file : " << fInputFileNameArray[iInput] << endl;
    }

    fNumFriends = fFriendFileNameArray.size();
    for (Int_t iFriend = 0; iFriend < fNumFriends; iFriend++) {
      TChain *friendTree = new TChain(fInputTreeName);
      friendTree -> AddFile(fFriendFileNameArray[iFriend]);
      fInputTree -> AddFriend(friendTree);
      fFriendTrees -> Add(friendTree);
    }

    fNumEntries = fInputTree -> GetEntries();
    kb_info << fInputTree -> GetName() << " tree containing " << fInputTree -> GetEntries() << " entries." << endl;

    TObjArray *branchArray = fInputTree -> GetListOfBranches();
    Int_t nBranches = branchArray -> GetEntries();
    for (Int_t iBranch = 0; iBranch < nBranches; iBranch++) {
      TBranch *branch = (TBranch *) branchArray -> At(iBranch);
      fInputTree -> SetBranchStatus(branch -> GetName(), 1);
      fInputTree -> SetBranchAddress(branch -> GetName(), &fBranchPtr[fNBranches]);
      fBranchPtrMap[branch -> GetName()] = fBranchPtr[fNBranches];
      fNBranches++;
      kb_info << "Input branch " << branch -> GetName() << " found" << endl;
    }

    for (Int_t iFriend = 0; iFriend < fNumFriends; iFriend++) {
      auto friendTree = GetFriendChain(iFriend);
      branchArray = friendTree -> GetListOfBranches();
      nBranches = branchArray -> GetEntries();
      for (Int_t iBranch = 0; iBranch < nBranches; iBranch++)
      {
        TBranch *branch = (TBranch *) branchArray -> At(iBranch);
        friendTree -> SetBranchStatus(branch -> GetName(), 1);
        friendTree -> SetBranchAddress(branch -> GetName(), &fBranchPtr[fNBranches]);
        fBranchPtrMap[branch -> GetName()] = fBranchPtr[fNBranches];
        fNBranches++;
        kb_info << "Input friend branch " << branch -> GetName() << " found" << endl;
      }
    }
  }

  if (fPar == nullptr) {
    if (fInputFile != nullptr && fInputFile -> Get("ParameterContainer") != nullptr) {
      fPar = (KBParameterContainer *) fInputFile -> Get("ParameterContainer");
      kb_info << "Parameter container found in " << fInputFileName << endl;
    }
    else {
      kb_error << "FAILED to load parameter container." << endl;
      return false;
    }
  }

  if (fInputFile != nullptr && fInputFile -> Get("RunHeader") != nullptr) {
    KBParameterContainer *runHeaderIn = (KBParameterContainer *) fInputFile -> Get("RunHeader");

    TString runName = runHeaderIn -> GetParString("RunName");
    if (fRunName.IsNull())
      fRunName = runName;
    else if (!fRunName.IsNull() && fRunName != runName) {
      kb_error << "Run name for input and output file do not match!" << endl;
      return false;
    }

    Int_t runID = runHeaderIn -> GetParInt("RunID");
    if (fRunID == -1)
      fRunID = runID;
    else if (runID != -1 && fRunID != runID) {
      kb_error << "RunID for input and output file do not match!" << endl;
      return false;
    }
  }

  gRandom -> SetSeed(time(0));
  TString clist = "QWERTYUIOPASDFGHJKLZXCVBNM1234567890qwertyuiopsadfghjklzxcvbnm";
  for (auto iHash=0; iHash<7; ++iHash) fHash = fHash + clist[((Int_t) gRandom -> Uniform(62))];

  fRunHeader = new KBParameterContainer();
  fRunHeader -> SetName("RunHeader");
  fRunHeader -> SetPar("Hash",fHash);
  fRunHeader -> SetPar("KEBIVersion",KBRun::GetKEBIVersion());
  fRunHeader -> SetPar("GETDecoderVersion",KBRun::GetGETDecoderVersion());
  fRunHeader -> SetPar("KEBIHostName",KBRun::GetKEBIHostName());
  fRunHeader -> SetPar("KEBIUserName",KBRun::GetKEBIUserName());
  fRunHeader -> SetPar("KEBIPath",KBRun::GetKEBIPath());
  fRunHeader -> SetPar("RunName",fRunName);
  fRunHeader -> SetPar("RunID",fRunID);
  if (fInputFileName.IsNull() == false)
    fRunHeader -> SetPar("InputFile",fInputFileName);

  if (fDetectorSystem -> GetEntries() != 0) {
    fDetectorSystem -> SetParameterContainer(fPar);
    fDetectorSystem -> Init();
    kb_print << fDetectorSystem -> GetName() << " initialized" << endl;
    kb_out << endl;
  }

  if (fOutputFileName.IsNull()) {
    if (fRunID != -1) {
      if (fRunName.IsNull())
        fRunName = "run";
      fOutputFileName = fRunName + Form("%04d", fRunID);

      if (!fTag.IsNull())
        fOutputFileName = fOutputFileName + "." + fTag;
      else
        fOutputFileName = fOutputFileName + ".out";

      if (fSplit != -1)
        fOutputFileName = fOutputFileName + Form(".%d",fSplit);

      fOutputFileName = ConfigureDataPath(fOutputFileName);
    }
    else {
      kb_warning << "Output file is not set!" << endl;
      kb_warning << "Set output-file-name(SetOutputFile) or runID(SetRunID)." << endl;
    }
  }
  else
    fOutputFileName = ConfigureDataPath(fOutputFileName);

  fKBLogFileName = TString(KEBI_PATH) + "/data/kbrun.log";

  if (!fOutputFileName.IsNull())
  {
    if (CheckFileExistence(fOutputFileName)) {}

    kb_out << endl;
    kb_info << "Output file : " << fOutputFileName << endl;
    fOutputFile = new TFile(fOutputFileName, "recreate");
    fOutputTree = new TTree("data", "");
  }

  fInitialized = InitTasks();

  kb_out << endl;
  if (fInitialized) {
    kb_info << fNumEntries << " input entries" << endl;
    kb_print << "KBRun initialized" << endl;
  }
  else
    kb_error << "[KBRun] FAILED initializing tasks." << endl;

  fCurrentEventID = -1;

  return fInitialized;
}

TDatabasePDG *KBRun::GetDatabasePDG()
{
  TDatabasePDG *db = TDatabasePDG::Instance();

  if (db->GetParticle("deuteron")==nullptr)
  {
    kb_print << "Adding ions to TDatabasePDG" << endl;

    db -> AddParticle("deuteron","", 1.87561 ,1,0, 3,"Ion",1000010020);
    db -> AddParticle("triton"  ,"", 2.80892 ,1,0, 3,"Ion",1000010030);
    db -> AddParticle("He3"     ,"", 2.80839 ,1,0, 6,"Ion",1000020030);
    db -> AddParticle("He4"     ,"", 3.72738 ,1,0, 6,"Ion",1000020040);
    db -> AddParticle("Li6"     ,"", 5.6     ,1,0, 9,"Ion",1000030060);
    db -> AddParticle("Li7"     ,"", 6.5     ,1,0, 9,"Ion",1000030070);
    db -> AddParticle("Be7"     ,"", 6.5     ,1,0,12,"Ion",1000040070);
    db -> AddParticle("Be9"     ,"", 8.4     ,1,0,12,"Ion",1000040090);
    db -> AddParticle("Be10"    ,"", 9.3     ,1,0,12,"Ion",1000040100);
    db -> AddParticle("Bo10"    ,"", 9.3     ,1,0,15,"Ion",1000050100);
    db -> AddParticle("Bo11"    ,"",10.2     ,1,0,15,"Ion",1000050110);
    db -> AddParticle("C11"     ,"",10.2     ,1,0,18,"Ion",1000060110);
    db -> AddParticle("C12"     ,"",11.17793 ,1,0,18,"Ion",1000060120);
    db -> AddParticle("C13"     ,"",12.11255 ,1,0,18,"Ion",1000060130);
    db -> AddParticle("C14"     ,"",13.04394 ,1,0,18,"Ion",1000060140);
    db -> AddParticle("N13"     ,"",12.1     ,1,0,21,"Ion",1000070130);
    db -> AddParticle("N14"     ,"",13.0     ,1,0,21,"Ion",1000070140);
    db -> AddParticle("N15"     ,"",14.0     ,1,0,21,"Ion",1000070150);
    db -> AddParticle("O16"     ,"",14.89917 ,1,0,24,"Ion",1000080160);
    db -> AddParticle("O17"     ,"",15.83459 ,1,0,24,"Ion",1000080170);
    db -> AddParticle("O18"     ,"",16.76611 ,1,0,24,"Ion",1000080180);
  }

  return db;
}

TParticlePDG *KBRun::GetParticle(Int_t pdg)        { return GetDatabasePDG() -> GetParticle(pdg); }
TParticlePDG *KBRun::GetParticle(const char *name) { return GetDatabasePDG() -> GetParticle(name); }

void KBRun::CreateParameterFile(TString name)
{
  kb_out  << "===========================================================================================" << endl;
  kb_info << "  CreateParameterFile -> " << name << endl;
  kb_info << endl;
  kb_info << "  Note:" << endl;
  kb_info << "  1. This method will create skeleton parameter file with given name." << endl;
  kb_info << "  2. You must set input file as usual." << endl;
  kb_info << "  3. KBRun will only run Init() method to collect parameters." << endl;
  kb_info << "  3. This method will not work if program stops due to missing parameters. " << endl;
  kb_out  << "===========================================================================================" << endl;

  fPar -> SetDebugMode(true);
  Init();
  if (name.Index(".par") < 0)
    name = name + ".par";
  fPar -> SaveAs(name);

  if (fAutoTerminate) Terminate(this);
}

bool KBRun::RegisterBranch(TString name, TObject *obj, bool persistent)
{
  if (fBranchPtrMap[name] != nullptr)
    return false;

  fBranchPtr[fNBranches] = obj;
  fBranchPtrMap[name] = fBranchPtr[fNBranches];
  fNBranches++;

  if (persistent) {
    if (fOutputTree != nullptr)
      fOutputTree -> Branch(name, &obj);
    fPersistentBranchArray -> Add(obj);
    kb_info << "Output branch " << name << " (persistent)" << endl;
  } else {
    fTemporaryBranchArray -> Add(obj);
    kb_info << "Output branch " << name << " (temporary)" << endl;
  }

  return true;
}

TObject *KBRun::GetBranch(TString name)
{
  TObject *dataContainer = fBranchPtrMap[name];
  return dataContainer;
}

TClonesArray *KBRun::GetBranchA(TString name)
{
  TObject *dataContainer = fBranchPtrMap[name];
  if (dataContainer -> InheritsFrom("TClonesArray"))
    return (TClonesArray *) dataContainer;
  return nullptr;
}

void KBRun::AddDetector(KBDetector *detector) { fDetectorSystem -> AddDetector(detector); }
KBDetector *KBRun::GetDetector(Int_t i) const { return (KBDetector *) fDetectorSystem -> At(i); }
KBDetectorSystem *KBRun::GetDetectorSystem() const { return fDetectorSystem; }

void KBRun::SetGeoManager(TGeoManager *gm) { fDetectorSystem -> SetGeoManager(gm); }
TGeoManager *KBRun::GetGeoManager() const { return fDetectorSystem -> GetGeoManager(); }

void KBRun::SetGeoTransparency(Int_t transparency)
{
  fDetectorSystem -> SetTransparency(transparency);
}

void KBRun::SetEntries(Long64_t num) { fNumEntries = num; }
Long64_t KBRun::GetEntries() const { return fNumEntries; }
Long64_t KBRun::GetNumEvents() const { return fNumEntries; }

Int_t KBRun::GetEntry(Long64_t entry, Int_t getall)
{
  if (fInputTree == nullptr)
    return -1;

  fCurrentEventID = entry;
  fEventCount = entry;

  for (Int_t iFriend = 0; iFriend < fNumFriends; iFriend++)
    GetFriendChain(iFriend) -> GetEntry(entry, getall);

  return fInputTree -> GetEntry(entry, getall);
}

Int_t KBRun::GetEvent(Long64_t entry) { return GetEntry(entry); }
bool KBRun::GetNextEvent() { return GetEntry(fCurrentEventID+1) != 0 ? true : false; }

Long64_t KBRun::GetStartEventID() const { return fStartEventID; }
Long64_t KBRun::GetEndEventID() const { return fEndEventID; }
Long64_t KBRun::GetCurrentEventID() const { return fCurrentEventID; }
Long64_t KBRun::GetEventCount() const { return fEventCount; }

bool KBRun::Event(Long64_t eventID)
{
  kb_out << endl;
  if (fInitialized == false) {
    kb_info << "KBRun is not Initialized!" << endl;
    kb_info << "Exit run" << endl;
    return false;
  }

  if (eventID < 0 || eventID > fNumEntries-1) {
    kb_info << "Event-ID(" << eventID << ") is out of range: " << endl;
    kb_info << "Exit event" << endl;
    return false;
  }

  fCurrentEventID = eventID;
  if (fInputTree != nullptr) {
    fInputTree -> GetEntry(eventID);

    for (Int_t iFriend = 0; iFriend < fNumFriends; iFriend++)
      GetFriendChain(iFriend) -> GetEntry(eventID);
  }

  kb_info << "Execute Event " << eventID << "    (X persistent)" << endl;
  ExecuteTask("");

  return true;
}

bool KBRun::NextEvent() { return Event(fCurrentEventID+1); }

void KBRun::Run()
{
  kb_out << endl;
  if (fInitialized == false) {
    kb_info << "KBRun is not initialized!" << endl;
    kb_info << "try initialization..." << endl;
    if (!Init())
      kb_error << "Exit Run() due to initialization fail." << endl;
  }

  CheckIn();

  if (fStartEventID == -1 && fEndEventID == -1) {
    fStartEventID = 0;
    fEndEventID = fNumEntries-1;
  }

  if (fSplit != -1) {
    fStartEventID = fSplit * fNumSplitEntries;
    fEndEventID = ((fSplit+1) * fNumSplitEntries) - 1 ;
    if (fEndEventID > fNumEntries - 1)
      fEndEventID = fNumEntries - 1;
  }

  Int_t numRunEntries = fEndEventID - fStartEventID + 1;

  fSignalEndOfEvent = false;

  fEventCount = 1;

  Long64_t iEntry = 0;
  for (iEntry = fStartEventID; iEntry <= fEndEventID; iEntry++) {
    fCurrentEventID = iEntry;
    if (fInputTree != nullptr) {
      fInputTree -> GetEntry(iEntry);
      ///TODO @todo fCurrentEventID = EventHeader -> GetEventID();
      for (Int_t iFriend = 0; iFriend < fNumFriends; iFriend++)
        GetFriendChain(iFriend) -> GetEntry(iEntry);
    }

    kb_out << endl;
    kb_info << "Execute Event " << iEntry << " (" << fEventCount << "/" << numRunEntries << ")" << endl;
    ExecuteTask("");

    if (fSignalEndOfEvent)
      break;

    if (fOutputTree != nullptr)
      fOutputTree -> Fill();

    ++fEventCount;
  }

  if (fOutputTree != nullptr) {
    fOutputFile -> cd();
    fOutputTree -> Write();
    fPar -> Write(fPar->GetName(),TObject::kSingleKey);
    fRunHeader -> Write(fRunHeader->GetName(),TObject::kSingleKey);
  }

  kb_out << endl;
  kb_info << "End of Run " << fStartEventID << " -> " << fEndEventID << " (" << fEndEventID - fStartEventID + 1 << ")" << endl;
  if (fSignalEndOfEvent)
    kb_info << "Run stoped at event " << iEntry - 1 << " (" << iEntry - fStartEventID << ") because EndOfEvent signal was sent" << endl;

  kb_out << endl;
  Print("cio");

  if (fOutputTree != nullptr) {
    fOutputFile -> Close();
    TString linkName = TString(KEBI_PATH) + "/data/LAST_OUTPUT";
    unlink(linkName.Data());
    symlink(fOutputFileName.Data(), linkName.Data());
  }

  CheckOut();

  if (fAutoTerminate) Terminate(this);
}

void KBRun::EndOfEvent() { fSignalEndOfEvent = true; }

void KBRun::RunSingle(Long64_t eventID)
{
  if (eventID < 0 || eventID > fNumEntries - 1) {
    kb_error << "EventID: " << eventID << ", not in proper range." << endl;
    kb_error << "Entry range : " << 0 << " -> " << fNumEntries - 1 << endl;
    kb_error << "Exit run" << endl;
    return;
  }

  fStartEventID = eventID;
  fEndEventID = eventID;
  Run();
}

void KBRun::RunInRange(Long64_t startID, Long64_t endID)
{
  if (startID > endID || startID < 0 || endID > fNumEntries - 1) {
    kb_error << "startID " << startID << " and endID " << endID << " not in proper range." << endl;
    kb_error << "entry range : " << 0 << " -> " << fNumEntries - 1 << endl;
    kb_error << "Exit run" << endl;
    return;
  }

  fStartEventID = startID;
  fEndEventID = endID;
  Run();
}

void KBRun::RunInEventRange(Long64_t, Long64_t)
{
}

#ifdef ACTIVATE_EVE
void KBRun::OpenEventDisplay()
{
  if (fDetectorSystem -> GetEntries() == 0)
    kb_warning << "Cannot open event display: detector is not set." << endl;

  if (gEve != nullptr) {
    kb_error << "gEve is nullptr" << endl;
    return;
  }

  TEveManager::Create(true, "V");
  fEveEventManager = new TEveEventManager();
  gEve -> AddEvent(fEveEventManager);

  {
    Int_t dummy;
    UInt_t w, h;
    UInt_t wMax = 1200;
    UInt_t hMax = 720;
    Double_t r = (Double_t)wMax/hMax;
    gVirtualX -> GetWindowSize(gClient -> GetRoot() -> GetId(), dummy, dummy, w, h);

    if (w > wMax) {
      w = wMax;
      h = hMax;
    } else
      h = (Int_t)(w/r);

    gEve -> GetMainWindow() -> Resize(w, h);
  }

  gEve -> GetDefaultGLViewer() -> SetClearColor(kBlack);

  TGeoNode* geoNode = gGeoManager -> GetTopNode();
  TEveGeoTopNode* topNode = new TEveGeoTopNode(gGeoManager, geoNode, 1, 3, 10000);
  gEve -> AddGlobalElement(topNode);

  gEve -> FullRedraw3D(kTRUE);

  //gEve -> GetDefaultViewer() -> GetGLViewer() -> SetClearColor(kWhite);

  //return;

  /*
  gEve -> GetBrowser() -> SetTabTitle("3D", TRootBrowser::kRight);

  auto slotOv = TEveWindow::CreateWindowInTab(gEve -> GetBrowser() -> GetTabRight()); slotOv -> SetElementName("Overview Slot");
  auto packOv = slotOv -> MakePack(); packOv -> SetElementName("Overview Pack");

  // 1st Row
  auto slotPA = packOv -> NewSlot();
  auto packPA = slotPA -> MakePack();

  // Planes in 1st Row
  packPA -> SetHorizontal();
  for (auto iPlane = 0; iPlane < fDetectorSystem -> GetNumPlanes(); iPlane++) {
    auto slotPlane = packPA -> NewSlot(); slotPlane -> SetElementName(Form("Plane%d Slot", iPlane));
    auto ecvsPlane = new TRootEmbeddedCanvas();
    auto framPlane = slotPlane -> MakeFrame(ecvsPlane); framPlane -> SetElementName(Form("Detector Plane%d Frame", iPlane));

    TCanvas *cvs = ecvsPlane -> GetCanvas();
    fCvsDetectorPlaneArray -> Add(cvs);
    cvs -> cd();
    fDetectorSystem -> GetDetectorPlane(iPlane) -> GetHist(1) -> Draw("col");
  }

  // 2nd Row
  packOv -> SetVertical();
  auto slotCh = packOv -> NewSlotWithWeight(.35); slotCh -> SetElementName("Channel Buffer Slot");
  auto ecvsCh = new TRootEmbeddedCanvas();
  auto frameCh = slotCh -> MakeFrame(ecvsCh); frameCh -> SetElementName("Channel Buffer Frame");
  fCvsChannelBuffer = ecvsCh -> GetCanvas();

  gEve -> GetBrowser() -> GetTabRight() -> SetTab(1);
  */

  for (Int_t iPlane = 0; iPlane < fDetectorSystem -> GetNumPlanes(); iPlane++) {
    KBDetectorPlane *plane = fDetectorSystem -> GetDetectorPlane(iPlane);
    TCanvas *cvs = plane -> GetCanvas();
    cvs -> AddExec("ex", "KBRun::ClickSelectedPadPlane()");
    fCvsDetectorPlaneArray -> Add(cvs);
    cvs -> cd();
    plane -> GetHist() -> Draw("colz");
  }

  gEve -> GetBrowser() -> HideBottomTab();
  gEve -> ElementSelect(gEve -> GetCurrentEvent());
  gEve -> GetWindowManager() -> HideAllEveDecorations();
}
#endif

void KBRun::RunEve(Long64_t eventID, TString option)
{
  if (fEveOption.IsNull() || (!fEveOption.IsNull() && option.Index("0")<0))
    fEveOption = option;

  Bool_t drawEve3D = false;
  if (fEveOption.Index("e")>=0) {
    kb_info << "3D event display activated" << endl;
    drawEve3D = true;
  }

  Bool_t drawDetectorPlanes = false;
  if (fEveOption.Index("p")>=0) {
    kb_info << "Detector plane display activated" << endl;
    drawDetectorPlanes = true;
  }

  for (Int_t iGraph = 0; iGraph < 20; ++iGraph) {
    fGraphChannelBoundaryNb[iGraph] = new TGraph();
    fGraphChannelBoundaryNb[iGraph] -> SetLineColor(kGreen);
    fGraphChannelBoundaryNb[iGraph] -> SetLineWidth(2);
  }

  this -> GetEntry(eventID);

#ifdef ACTIVATE_EVE
  Int_t nEveElements = fEveElementList.size();
  for (Int_t iEl = 0; iEl < nEveElements; ++iEl) {
    TEveElement *el = fEveElementList.back();
    gEve -> RemoveElement(el,gEve->GetCurrentEvent());
    fEveElementList.pop_back();
  }
#endif

  Int_t chooseTracks = 0;
  Int_t chooseParents = 0;
  Int_t choosePDGs = 0;

  vector<Int_t> choosenTracks;
  vector<Int_t> choosenParents;
  vector<Int_t> choosenPDGs;

  while (fPar -> CheckPar("eveSelectTrackIDs"))
  {
    TString eveTIDOption = fPar -> GetParString("eveSelectTrackIDs");
    TObjArray *eveTIDs = eveTIDOption.Tokenize(":");
    auto numEveTIDs = eveTIDs -> GetEntries();
    if (numEveTIDs < 2)
      break;

    TString pm = ((TObjString *) eveTIDs -> At(0)) -> GetString();
         if (pm=="+") chooseTracks = +1;
    else if (pm=="-") chooseTracks = -1;
    else break;

    TString message = "Tracklet ID selection("+pm+"): ";
    for (Int_t iID = 1; iID < numEveTIDs; ++iID) {
      TString tID = ((TObjString *) eveTIDs -> At(iID)) -> GetString();
      choosenTracks.push_back(tID.Atoi());
      message = message + tID + " ";
    }
    kb_info << message << endl;
    break;
  }

  while (fPar -> CheckPar("eveSelectTrackParentIDs"))
  {
    TString eveTIDOption = fPar -> GetParString("eveSelectTrackParentIDs");
    TObjArray *eveTIDs = eveTIDOption.Tokenize(":");
    auto numEveTIDs = eveTIDs -> GetEntries();
    if (numEveTIDs < 2)
      break;

    TString pm = ((TObjString *) eveTIDs -> At(0)) -> GetString();
         if (pm=="+") chooseParents = +1;
    else if (pm=="-") chooseParents = -1;
    else break;

    TString message = "Tracklet Parent ID selection("+pm+"): ";
    for (Int_t iID = 1; iID < numEveTIDs; ++iID) {
      TString tID = ((TObjString *) eveTIDs -> At(iID)) -> GetString();
      choosenParents.push_back(tID.Atoi());
      message = message + tID + " ";
    }
    kb_info << message << endl;
    break;
  }

  while (fPar -> CheckPar("eveSelectTrackPDGs"))
  {
    TString eveTIDOption = fPar -> GetParString("eveSelectTrackPDGs");
    TObjArray *eveTIDs = eveTIDOption.Tokenize(":");
    auto numEveTIDs = eveTIDs -> GetEntries();
    if (numEveTIDs < 2)
      break;

    TString pm = ((TObjString *) eveTIDs -> At(0)) -> GetString();
         if (pm=="+") choosePDGs = +1;
    else if (pm=="-") choosePDGs = -1;
    else break;

    TString message = "Tracklet PDG selection("+pm+"): ";
    for (Int_t iID = 1; iID < numEveTIDs; ++iID) {
      TString tID = ((TObjString *) eveTIDs -> At(iID)) -> GetString();
      choosenPDGs.push_back(tID.Atoi());
      message = message + tID + " ";
    }
    kb_info << message << endl;
    break;
  }

  TObjArray *eveBranchNames = fEveBranches.Tokenize(":");
  auto numSelectedBranches = eveBranchNames -> GetEntries();

  auto numEveBranches = fNBranches;
  if (numSelectedBranches != 0)
    numEveBranches = numSelectedBranches;

#ifdef ACTIVATE_EVE
  if (drawEve3D)
  {
    if (gEve == nullptr)
      OpenEventDisplay();

    for (Int_t iBranch = 0; iBranch < numEveBranches; ++iBranch)
    {
      TClonesArray *branch = nullptr;
      if (numSelectedBranches != 0) {
        TString branchName = ((TObjString *) eveBranchNames -> At(iBranch)) -> GetString();
        branch = (TClonesArray *) fBranchPtrMap[branchName];
      }
      else
        branch = (TClonesArray *) fBranchPtr[iBranch];

      if (branch -> GetEntries() == 0)
        continue;

      auto objSample = branch -> At(0);
      if (objSample -> InheritsFrom("KBContainer") == false)
        continue;

      bool isTracklet = false;
      if (objSample -> InheritsFrom("KBTracklet"))
        isTracklet = true;

      KBContainer *eveObj = (KBContainer *) objSample;
      if (numSelectedBranches == 0 && !eveObj -> DrawByDefault())
        continue;

      kb_info << "Drawing " << eveObj -> ClassName() << endl;
      Int_t nObjects = branch -> GetEntries();
      if (isTracklet)
      {
        for (Int_t iObject = 0; iObject < nObjects; ++iObject) {
          KBTracklet *eveTrk = (KBTracklet *) branch -> At(iObject);

          bool isGood = true;
          if (chooseTracks==1) {
            isGood = false;
            for (auto id : choosenTracks)
              if (eveTrk->GetTrackID()==id) { isGood = true; break; }
          } else if (chooseTracks==-1) {
            isGood = true;
            for (auto id : choosenTracks)
              if (eveTrk->GetTrackID()==id) { isGood = false; break; }
          } if (chooseParents==1) {
            isGood = false;
            for (auto id : choosenParents)
              if (eveTrk->GetParentID()==id) { isGood = true; break; }
          } else if (chooseParents==-1) {
            isGood = true;
            for (auto id : choosenParents)
              if (eveTrk->GetParentID()==id) { isGood = false; break; }
          } if (choosePDGs==1) {
            isGood = false;
            for (auto id : choosenPDGs)
              if (eveTrk->GetPDG()==id) { isGood = true; break; }
          } else if (choosePDGs==-1) {
            isGood = true;
            for (auto id : choosenPDGs)
              if (eveTrk->GetPDG()==id) { isGood = false; break; }
          }
          if (!isGood)
            continue;

          TEveLine *eveElement = (TEveLine *) eveTrk -> CreateEveElement();
          eveTrk -> SetEveElement(eveElement, fEveScale);
          TString name = TString(eveElement -> GetElementName()) + Form("_%d",iObject);
          eveElement -> SetElementName(name);
          gEve -> AddElement(eveElement);
          fEveElementList.push_back(eveElement);
        }
      }
      else if (eveObj -> IsEveSet()) {
        TEveElement *eveSet = eveObj -> CreateEveElement();
        for (Int_t iObject = 0; iObject < nObjects; ++iObject) {
          eveObj = (KBContainer *) branch -> At(iObject);
          eveObj -> AddToEveSet(eveSet, fEveScale);
        }
        gEve -> AddElement(eveSet);
        fEveElementList.push_back(eveSet);
      }
      else {
        for (Int_t iObject = 0; iObject < nObjects; ++iObject) {
          eveObj = (KBContainer *) branch -> At(iObject);
          TEveElement *eveElement = eveObj -> CreateEveElement();
          eveObj -> SetEveElement(eveElement);
          TString name = TString(eveElement -> GetElementName()) + Form("_%d",iObject);
          eveElement -> SetElementName(name);
          gEve -> AddElement(eveElement);
          fEveElementList.push_back(eveElement);
        }
      }
    }
    gEve -> Redraw3D();
  }
#endif

  if (drawDetectorPlanes)
  {
    auto hitArray = (TClonesArray *) fBranchPtrMap[TString("Hit")];
    auto padArray = (TClonesArray *) fBranchPtrMap[TString("Pad")];

    auto numPlanes = fDetectorSystem -> GetNumPlanes();
    for (auto iPlane = 0; iPlane < numPlanes; ++iPlane)
    {
      auto plane = fDetectorSystem -> GetDetectorPlane(iPlane);
      kb_info << "Drawing " << plane -> GetName() << endl;

      auto histPlane = plane -> GetHist();
      histPlane -> Reset();

      if (plane -> InheritsFrom("KBPadPlane"))
      {
        auto padplane = (KBPadPlane *) plane;

        bool exist_hit = false;
        bool exist_pad = false;

        if (hitArray != nullptr)
          exist_hit = true;
        if (padArray != nullptr)
          exist_pad = true;

        if (!exist_hit && !exist_pad)
          continue;

        if (exist_hit)
        {
          kb_info << "Filling Hits to PadPlane" << endl;
          padplane -> Clear();
          padplane -> SetHitArray(hitArray);
          if (!exist_pad)
            padplane -> FillDataToHist("hit");
        }

        if (exist_pad)
        {
          kb_info << "Filling Pads to PadPlane" << endl;
          if (!exist_hit)
            padplane -> Clear();
          padplane -> SetPadArray(padArray);
          //padplane -> FillDataToHist("raw");
          padplane -> FillDataToHist("out");
        }
      }

      auto cvs = (TCanvas *) fCvsDetectorPlaneArray -> At(iPlane);
      cvs -> cd();
      histPlane -> SetMaximum(5000);
      histPlane -> SetMinimum(1);
      histPlane -> Draw("colz");
      plane -> DrawFrame();

      KBVector3::Axis axis1 = plane -> GetAxis1();
      KBVector3::Axis axis2 = plane -> GetAxis2();

      for (Int_t iBranch = 0; iBranch < numEveBranches; ++iBranch)
      {
        TClonesArray *branch = nullptr;
        if (numSelectedBranches != 0) {
          TString branchName = ((TObjString *) eveBranchNames -> At(iBranch)) -> GetString();
          branch = (TClonesArray *) fBranchPtrMap[branchName];
        }
        else
          branch = (TClonesArray *) fBranchPtr[iBranch];

        TObject *objSample = nullptr;

        Int_t numTracklets = branch -> GetEntries();
        if (numTracklets != 0) {
          objSample = branch -> At(0);
          if (objSample -> InheritsFrom("KBContainer") == false || objSample -> InheritsFrom("KBTracklet") == false)
            continue;
        }
        else
          continue;

        auto trackletSample = (KBTracklet *) objSample;
        if (trackletSample -> DoDrawOnDetectorPlane())
        {
          for (auto iTracklet = 0; iTracklet < numTracklets; ++iTracklet) {
            auto tracklet = (KBTracklet *) branch -> At(iTracklet);
            tracklet -> TrajectoryOnPlane(axis1, axis2) -> Draw("samel");
          }
        }
      }
    }

    // @todo palette is changed when drawing top node because of TGeoMan(?)
    gStyle -> SetPalette(kBird);
  }
}

void KBRun::SetEveScale(Double_t scale) { fEveScale = scale; }

void KBRun::SelectEveBranches(TString option) { fEveBranches = option; }

Color_t KBRun::GetColor()
{
  //Color_t colors[] = {kOrange, kTeal, kViolet, kSpring, kPink, kAzure};
  //Color_t colors[] = {kPink, kAure, kSpring, kViolet, kTeal, kGreen, kRed, kBlue, kMagenta};
  //Color_t color = colors[index%6] + ((index/6)%20);
  //Color_t colors[] = {kRed, kPink, kMagenta, kViolet, kBlue, kAzure, kCyan, kTeal, kGreen, kSpring, kYellow, kOrange};
  //Color_t color = colors[((Int_t)gRandom -> Uniform(12))] + ((Int_t) gRandom -> Uniform(10)) - 9;
  Color_t colors[] = {kRed, kPink, kBlue, kAzure, kTeal, kSpring};
  Color_t color = colors[((Int_t)gRandom -> Uniform(6))] + ((Int_t) gRandom -> Uniform(10)) - 9;
  return color;
}

void KBRun::SetAutoTermination(Bool_t val) { fAutoTerminate = val; }

void KBRun::Terminate(TObject *obj, TString message)
{
  kb_info << "Terminated from [" << obj -> GetName() << "] " << message << endl;
  gApplication -> Terminate();
}

void KBRun::ClickSelectedPadPlane()
{
  TObject* select = ((TCanvas*)gPad) -> GetClickSelected();

  if (select == NULL || (!(select -> InheritsFrom(TH2::Class())) && !(select -> InheritsFrom(TGraph::Class()))))
    return;

  TH2D* hist = (TH2D*) select;

  Int_t xEvent = gPad -> GetEventX();
  Int_t yEvent = gPad -> GetEventY();

  Float_t xAbs = gPad -> AbsPixeltoX(xEvent);
  Float_t yAbs = gPad -> AbsPixeltoY(yEvent);
  Double_t xOnClick = gPad -> PadtoX(xAbs);
  Double_t yOnClick = gPad -> PadtoY(yAbs);

  Int_t bin = hist -> FindBin(xOnClick, yOnClick);
  gPad -> SetUniqueID(bin);
  gPad -> GetCanvas() -> SetClickSelected(NULL);

  KBRun::GetRun() -> DrawPadByPosition(xOnClick,yOnClick);
}

void KBRun::DrawPadByPosition(Double_t x, Double_t y)
{
  if (fCvsChannelBuffer == nullptr)
    fCvsChannelBuffer = new TCanvas("channel_buffer","channel buffer",700,400);
  fCvsChannelBuffer -> cd();

  if (fHistChannelBuffer == nullptr) {
    fHistChannelBuffer = new TH1D("channel_buffer","",512,0,512);
    fHistChannelBuffer -> SetStats(0);
  }

  KBTpc *tpc = (KBTpc *) fDetectorSystem -> GetTpc();
  if (tpc == nullptr)
    return;
  KBPadPlane *padplane = tpc -> GetPadPlane();
  Int_t id = padplane -> FindPadID(x, y);
  if (id < 0) {
    kb_error << "Could not find pad at position: " << x << ", " << y << endl;
    return;
  }


  KBPad *pad = padplane -> GetPad(id);
  pad -> SetHist(fHistChannelBuffer,"pao");
  pad -> Print();
  {
    if (fGraphChannelBoundary == nullptr) {
      fGraphChannelBoundary = new TGraph();
      fGraphChannelBoundary -> SetLineColor(kRed);
      fGraphChannelBoundary -> SetLineWidth(2);
    }
    fGraphChannelBoundary -> Set(0);

    auto corners = pad -> GetPadCorners();
    for (UInt_t iCorner = 0; iCorner < corners -> size(); ++iCorner) {
      TVector2 corner = corners -> at(iCorner);
      fGraphChannelBoundary -> SetPoint(fGraphChannelBoundary -> GetN(), corner.X(), corner.Y());
    }
    TVector2 corner = corners -> at(0);
    fGraphChannelBoundary -> SetPoint(fGraphChannelBoundary -> GetN(), corner.X(), corner.Y());
  }

  for (Int_t iBLine = 0; iBLine < 20; ++iBLine)
    fGraphChannelBoundaryNb[iBLine] -> Set(0);

  auto nbs = pad -> GetNeighborPadArray();
  Int_t numNbs = nbs -> size();
  for (auto iNb = 0; iNb < numNbs; ++iNb) {
    auto nb = (KBPad *) nbs -> at(iNb);
    auto corners = nb -> GetPadCorners();
    for (UInt_t iCorner = 0; iCorner < corners -> size(); ++iCorner) {
      TVector2 corner = corners -> at(iCorner);
      fGraphChannelBoundaryNb[iNb] -> SetPoint(fGraphChannelBoundaryNb[iNb] -> GetN(), corner.X(), corner.Y());
    }
    TVector2 corner = corners -> at(0);
    fGraphChannelBoundaryNb[iNb] -> SetPoint(fGraphChannelBoundaryNb[iNb] -> GetN(), corner.X(), corner.Y());

  }

  fHistChannelBuffer -> Draw("l");

  for (auto iHit = 0; iHit < pad -> GetNumHits(); ++iHit) {
    auto hit = pad -> GetHit(iHit);
    hit -> Print();
    auto f1 = hit -> GetPulseFunction();
    f1 -> SetNpx(500);
    f1 -> Draw("samel");
  }

  pad -> DrawMCID("mc");

  fCvsChannelBuffer -> Modified();
  fCvsChannelBuffer -> Update();

  ((TCanvas *) fCvsDetectorPlaneArray -> At(0)) -> cd();
  fGraphChannelBoundary -> Draw("samel");
  for (auto iNb = 0; iNb < numNbs; ++iNb)
    fGraphChannelBoundaryNb[iNb] -> Draw("samel");
}

void KBRun::SetLogFile(TString name) {
  if (name.IsNull()) {
    name = fOutputFileName;
    name += ".log";
  }
  fRunLogFileName = name;
  //fRunLogFileStream = std::ofstream(fRunLogFileName); // @todo Do not work for the version of gcc below 5.0.0
}

TString KBRun::GetLogFile() { return fRunLogFileName; }
//std::ofstream &KBRun::GetLogFileStream() { return &fRunLogFileStream; }

bool KBRun::CheckFileExistence(TString fileName, bool print)
{
  TString name = gSystem -> Which(".", fileName.Data());
  if (print) {
    if (name.IsNull())
      kb_info << fileName << " IS NEW." << endl;
    else
      kb_info << fileName << " EXIST!" << endl;
  }

  if (name.IsNull())
    return false;
  return true;
}

void KBRun::CheckIn()
{
  fstream kblogFile(fKBLogFileName.Data(), ios::out | ios::app);

  time_t t = time(0);
  struct tm * now = localtime(&t);
  TString ldate = Form("%04d.%02d.%02d",now->tm_year+1900,now->tm_mon+1,now->tm_mday);
  TString ltime = Form("%02d:%02d",now->tm_hour,now->tm_min);
  TString lname = KBRun::GetKEBIUserName();
  TString lversion = KBRun::GetKEBIVersion();
  TString linput = fInputFileName.IsNull() ? "-" : fInputFileName;
  TString loutput = fOutputFileName.IsNull() ? "-" : fOutputFileName;

  kblogFile << fHash << "  " << ldate << "  " << ltime << "  START  " << lname << "  " << lversion << "  "
          << "in:" << linput << "  out:" << loutput << "  " << endl;

  kblogFile.close();
}

void KBRun::CheckOut()
{
  fstream kblogFile(fKBLogFileName.Data(), ios::out | ios::app);

  time_t t = time(0);
  struct tm * now = localtime(&t);
  TString ldate = Form("%04d.%02d.%02d",now->tm_year+1900,now->tm_mon+1,now->tm_mday);
  TString ltime = Form("%02d:%02d",now->tm_hour,now->tm_min);

  kblogFile << fHash << "  " << ldate << "  " << ltime << "  END" << endl;
  kblogFile.close();
}
