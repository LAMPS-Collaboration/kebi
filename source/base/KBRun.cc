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
#include "TEveArrow.h"
#endif

#include "KBHit.hh"
#include "KBContainer.hh"
#include "KBTracklet.hh"
#include "KBMCStep.hh"

#include <unistd.h>
#include <iostream>
#include <ctime>

ClassImp(KBRun);

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
  fEveEventManagerArray = new TObjArray();
  fBranchPtr = new TObject*[1000];
  for (Int_t iBranch = 0; iBranch < 1000; ++iBranch)
    fBranchPtr[iBranch] = nullptr;

  fDetectorSystem = new KBDetectorSystem();

  ifstream log_branch_list(TString(KEBI_PATH)+"/log/KBBranchList.log");
  string line;
  TString hashTag, branchName;
  int numTags = -1;
  std::vector<TString> revListOfVersionMarks;
  while (getline(log_branch_list, line)) {
    istringstream ss(line);
    ss >> hashTag;
    if (hashTag=="__BRANCH__") {
      if (numTags>=0)
        fListOfNumTagsInGitBranches.push_back(numTags);
      numTags = 0;
      ss >> branchName;
      fListOfGitBranches.push_back(branchName);
    }
    else if (hashTag.IsNull() || hashTag.Sizeof() !=8)
      continue;
    else {
      fListOfGitHashTags.push_back(hashTag);
      //revListOfVersionMarks.push_back(TString(branchName+"."+(numTags+1)+"."+hashTag));
      revListOfVersionMarks.push_back(hashTag);
      numTags++;
    }
  }
  if (numTags>=0)
    fListOfNumTagsInGitBranches.push_back(numTags);

  auto numBranches = fListOfGitBranches.size();
  auto idx1 = 0;
  auto idx2 = 0;
  for (auto iBranch=0; iBranch<numBranches; ++iBranch)
  {
    idx2 += fListOfNumTagsInGitBranches[iBranch];
    for (auto iTag=idx2-1; iTag>=idx1; --iTag) {
      fListOfVersionMarks.push_back(revListOfVersionMarks.at(iTag));
    }
    idx1 = idx2+1;
  }

  CreateParameterContainer();
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
  if (option_print.Index("p") >= 0) {
    kb_out << "-------------------------------------------------------------------------------------------" << endl;
    fPar -> Print();
  }

  if (option_print.Index("d") >= 0) {
  kb_out << "-------------------------------------------------------------------------------------------" << endl;
    fDetectorSystem -> Print();
  }

  auto printTree = [](TTree *tree) {
    kb_out << "  Entries " << tree -> GetEntries() << endl;
    auto branchList = tree -> GetListOfBranches();
    auto numBranches = branchList -> GetEntries();
    if (numBranches < 10)
      for (auto ib=0; ib<numBranches; ++ib)
        kb_out << "  Branch " << ((TBranch *) branchList -> At(ib)) -> GetName() << endl;
    else {
      kb_out << "  Branches: ";
      Int_t count = 0;
      for (auto ib=0; ib<numBranches; ++ib) {
        auto b = (TBranch *) branchList -> At(ib);
        kb_out << b -> GetName() << " ";
        if (++count == 10) {
          kb_out << endl;
          count = 0;
        }
      }
      kb_out << endl;
    }
  };

  kb_out << "-------------------------------------------------------------------------------------------" << endl;
  kb_info << "Input: " << fInputFileName << endl;
  if (fInputTree != nullptr && option_print.Index("i") >= 0) {
    //fInputTree -> Print("toponly");
    printTree(fInputTree);

    for (Int_t iFriend = 0; iFriend < fNumFriends; iFriend++)
      //GetFriendChain(iFriend) -> Print("toponly");
      printTree(GetFriendChain(iFriend));
  }

  kb_out << "-------------------------------------------------------------------------------------------" << endl;
  kb_info << "Output: " << fOutputFileName << endl;
  if (fOutputTree != nullptr && option_print.Index("o") >= 0)
    //fOutputTree -> Print("toponly");
    printTree(fOutputTree);

  if (!fInitialized) {
    kb_out << "-------------------------------------------------------------------------------------------" << endl;
    kb_warning << "Print() is recommanded to be called after the initialization." << endl;
    kb_warning << "This run is not initialized. Please call Init() before Print()." << endl;

  }

  kb_out << "===========================================================================================" << endl;
}

TString KBRun::ConfigureDataPath(TString &name, bool search)
{
  name = ConfigureEnv(name);
  TString softwareVersion;

  TString pathPWD = getenv("PWD"); pathPWD = pathPWD + "/";
  TString pathKEBI = TString(KEBI_PATH) + "/data/";

  TString fullName;

  if (name == "last") {
    fullName = TString(KEBI_PATH) + "/data/LAST_OUTPUT";
    name = fullName;
    return softwareVersion;
  }

  if (name.Index(".root") != name.Sizeof()-6)
    name = name + ".root";

  bool hasVersion = false;
  auto array = name.Tokenize(".");
  if (array->GetEntries()>=5) {
    auto numEntries = array -> GetEntries();
    auto branchName = ((TObjString *) array->At(numEntries-4)) -> GetString();
    bool foundBranch = false;
    for (auto bname : fListOfGitBranches) {
      if (branchName==bname) {
        foundBranch = true;
        //auto version1 = ((TObjString *) array->At(numEntries-4)) -> GetString();
        //auto version2 = ((TObjString *) array->At(numEntries-3)) -> GetString();
        //auto version3 = ((TObjString *) array->At(numEntries-2)) -> GetString();
        //softwareVersion = version1 + "." + version2 + "." + version3;
        softwareVersion = ((TObjString *) array->At(numEntries-2)) -> GetString();
        break;
      }
    }
    if (foundBranch)
      hasVersion = true;
  }

  if (search)
  {
    if (name[0] != '.' && name[0] != '/' && name[0] != '$' && name != '~') {
      bool found = false;

      TString pathList[] = {fDataPath, pathPWD, pathPWD+"data/", pathKEBI};

      for (auto path : pathList) {
        fullName = path + name;
        if (CheckFileExistence(fullName,0)) {
          kb_info << fullName << " is found!" << endl;
          found = true;
          softwareVersion = "";
          break;
        }

        TString vxName = fullName;
        bool breakFlag = false;
        for (auto versionMark : fListOfVersionMarks)
        {
          fullName = vxName;
          fullName.ReplaceAll(".root",TString(".")+versionMark+".root");
          if (CheckFileExistence(fullName,0)) {
            kb_info << fullName << " is found!" << endl;
            found = true;
            breakFlag = true;
            softwareVersion = versionMark;
            break;
          }
        }
        if (breakFlag)
          break;
      }


      if (found) {
        name = fullName;
        return softwareVersion;
      }
      else {
        name = "";
        return softwareVersion;
      }
    }
    else {
      fullName = name;
      if (CheckFileExistence(fullName,0)) {
        kb_info << fullName << " is found!" << endl;
        name = fullName;
        return softwareVersion;
      }

      TString vxName = fullName;
      for (TString versionMark : fListOfVersionMarks)
      {
        fullName = vxName;
        fullName.ReplaceAll(".root",TString(".")+versionMark+".root");
        if (CheckFileExistence(fullName,0)) {
          kb_info << fullName << " is found!" << endl;
          name = fullName;
          softwareVersion = versionMark;
          return softwareVersion;
        }
      }

      name = "";
      return softwareVersion;
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

    if (!hasVersion) {
      //fullName.ReplaceAll(".root",TString(".")+KEBI_VERSION+".root");
      //softwareVersion = KEBI_VERSION;
      fullName.ReplaceAll(".root",TString(".")+KEBI_VERSION_SHORT+".root");
      softwareVersion = KEBI_VERSION_SHORT;
    }

    name = fullName;
    return softwareVersion;
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
  fInputFileName = fileName;
  fInputVersion = ConfigureDataPath(fInputFileName,true);
  fInputTreeName = treeName;
}

void KBRun::AddInput(TString fileName) {
  ConfigureDataPath(fileName,true);
  fInputFileNameArray.push_back(fileName);
}
void KBRun::AddFriend(TString fileName) {
  ConfigureDataPath(fileName,true);
  fFriendFileNameArray.push_back(fileName);
}
void KBRun::SetInputTreeName(TString treeName) { fInputTreeName = treeName; }

TFile *KBRun::GetInputFile() { return fInputFile; }
TChain *KBRun::GetInputChain()  const { return fInputTree; }
TChain *KBRun::GetFriendChain(Int_t iFriend)  const { return ((TChain *) fFriendTrees -> At(iFriend)); }

void KBRun::SetOutputFile(TString name) { fOutputFileName = name; }
TFile *KBRun::GetOutputFile() { return fOutputFile; }
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
  if (fInitialized)
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
      fInputTreeName = "event";

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
    Int_t numBranches = branchArray -> GetEntries();
    vector<TString> arrMCStepIDs;
    Int_t numMCStepIDs = 0;
    for (Int_t iBranch = 0; iBranch < numBranches; iBranch++) {
      TBranch *branch = (TBranch *) branchArray -> At(iBranch);
      TString branchName = branch -> GetName();
      if (branchName.Index("EdepSum")==0) // TODO
        continue;
      fInputTree -> SetBranchStatus(branch -> GetName(), 1);
      fInputTree -> SetBranchAddress(branch -> GetName(), &fBranchPtr[fNumBranches]);
      fBranchPtrMap[branch -> GetName()] = fBranchPtr[fNumBranches];
      fBranchNames.push_back(branch -> GetName());
      fNumBranches++;
      if (branchName.Index("MCStep")==0) {
        arrMCStepIDs.push_back(branchName.ReplaceAll("MCStep","").Data());
        ++numMCStepIDs;
      } else
        kb_info << "Input branch " << branchName << " found" << endl;
    }
    if (numMCStepIDs != 0) {
      kb_info << "Input branch (" << numMCStepIDs  << ") MCStep[";
      for (Int_t iID = 0; iID < numMCStepIDs-1; iID++) {
        auto id = arrMCStepIDs[iID];
        kb_out << id << ", ";
      }
      kb_out << arrMCStepIDs.back() << "] found" << endl;
    }

    for (Int_t iFriend = 0; iFriend < fNumFriends; iFriend++) {
      auto friendTree = GetFriendChain(iFriend);
      branchArray = friendTree -> GetListOfBranches();
      numBranches = branchArray -> GetEntries();
      for (Int_t iBranch = 0; iBranch < numBranches; iBranch++)
      {
        TBranch *branch = (TBranch *) branchArray -> At(iBranch);
        friendTree -> SetBranchStatus(branch -> GetName(), 1);
        friendTree -> SetBranchAddress(branch -> GetName(), &fBranchPtr[fNumBranches]);
        fBranchPtrMap[branch -> GetName()] = fBranchPtr[fNumBranches];
        fBranchNames.push_back(branch -> GetName());
        fNumBranches++;
        kb_info << "Input friend branch " << branch -> GetName() << " found" << endl;
      }
    }
  }

  if (fPar->IsEmpty()) {
    if (fInputFile != nullptr && fInputFile -> Get("ParameterContainer") != nullptr) {
      auto par = (KBParameterContainer *) fInputFile -> Get("ParameterContainer");
      AddParameterContainer(par);
      kb_info << "Parameter container found in " << fInputFileName << endl;
    }
    else {
      kb_warning << "FAILED to load parameter container from the input file." << endl;
      //return false;
    }
  }

  if (fInputFile != nullptr) {
    fProcessTable = (KBParameterContainer *) fInputFile -> Get("ProcessTable");
    fSDTable = (KBParameterContainer *) fInputFile -> Get("SensitiveDetectors");
    fVolumeTable = (KBParameterContainer *) fInputFile -> Get("Volumes");
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
    fDetectorSystem -> SetTransparency(80);
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

      fOutputVersion = ConfigureDataPath(fOutputFileName);
    }
    else {
      kb_warning << "Output file is not set!" << endl;
      kb_warning << "Set output-file-name(SetOutputFile) or runID(SetRunID)." << endl;
    }
  }
  else
    fOutputVersion = ConfigureDataPath(fOutputFileName);

  fKBLogFileName = TString(KEBI_PATH) + "/data/kbrun.log";

  if (!fOutputFileName.IsNull())
  {
    if (CheckFileExistence(fOutputFileName)) {}

    kb_out << endl;
    kb_info << "Output file : " << fOutputFileName << endl;
    fOutputFile = new TFile(fOutputFileName, "recreate");
    fOutputTree = new TTree("event", "");
  }

  if (!fOutputFileName.IsNull() && !fInputFileName.IsNull()) {
    if (fOutputVersion != fInputVersion) {
      kb_warning << "output file version is different from input file version!" << endl;
      kb_warning << "output:" << fOutputFileName << " input:" << fInputFileName.IsNull() << endl;
    }
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

  fInterruptHandler = new InterruptHandler();

  return fInitialized;
}

KBParameterContainer *KBRun::GetProcessTable() const { return fProcessTable; }
KBParameterContainer *KBRun::GetSDTable() const { return fSDTable; }
KBParameterContainer *KBRun::GetVolumeTable() const { return fVolumeTable; }

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
  if (name.Index(".conf") < 0)
    name = name + ".conf";
  fPar -> SaveAs(name);

  if (fAutoTerminate) Terminate(this);
}

bool KBRun::RegisterBranch(TString name, TObject *obj, bool persistent)
{
  if (fBranchPtrMap[name] != nullptr)
    return false;

  fBranchPtr[fNumBranches] = obj;
  fBranchPtrMap[name] = fBranchPtr[fNumBranches];
  fBranchNames.push_back(name);
  fNumBranches++;

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

void KBRun::Run(Long64_t endEventID)
{
  kb_out << endl;
  if (fInitialized == false) {
    kb_info << "KBRun is not initialized!" << endl;
    kb_info << "try initialization..." << endl;
    if (!Init()) {
      kb_error << "Exit Run() due to initialization fail." << endl;
      return;
    }
  }

  CheckIn();

  if (endEventID > 0)
    fEndEventID = endEventID - 1;

  if (fStartEventID == -1)
    fStartEventID = 0;

  if (fEndEventID == -1)
    fEndEventID = fNumEntries-1;

  if (fSplit != -1) {
    fStartEventID = fSplit * fNumSplitEntries;
    fEndEventID = ((fSplit+1) * fNumSplitEntries) - 1 ;
    if (fEndEventID > fNumEntries - 1)
      fEndEventID = fNumEntries - 1;
  }

  Int_t numRunEntries = fEndEventID - fStartEventID + 1;

  fEventCount = 1;

  for (fIdxEntry = fStartEventID; fIdxEntry <= fEndEventID; ++fIdxEntry) {
    fCurrentEventID = fIdxEntry;
    if (fInputTree != nullptr) {
      fInputTree -> GetEntry(fIdxEntry);
      ///TODO @todo fCurrentEventID = EventHeader -> GetEventID();
      for (Int_t iFriend = 0; iFriend < fNumFriends; iFriend++)
        GetFriendChain(iFriend) -> GetEntry(fIdxEntry);
    }

    kb_out << endl;
    kb_info << "Execute Event " << fIdxEntry << " (" << fEventCount << "/" << numRunEntries << ")" << endl;
    ExecuteTask("");

    if (fSignalEndOfRun)
      break;
    if (fOutputTree != nullptr)
      fOutputTree -> Fill();

    ++fEventCount;
  }

  EndOfRunTasks();

  if (fOutputTree != nullptr) {
    fOutputFile -> cd();
    fOutputTree -> Write();
    fPar -> Write(fPar->GetName(),TObject::kSingleKey);
    fRunHeader -> Write(fRunHeader->GetName(),TObject::kSingleKey);
  }

  kb_out << endl;
  kb_info << "End of Run " << fStartEventID << " -> " << fEndEventID << " (" << fEndEventID - fStartEventID + 1 << ")" << endl;
  if (fSignalEndOfRun)
    kb_info << "Run stoped at event " << fIdxEntry << " (" << fIdxEntry - fStartEventID << ") because EndOfRun signal was sent" << endl;

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


void KBRun::SignalInterrupt()
{
  if (fCheckIn)
    return;

  if (fOutputTree != nullptr) {
    fOutputTree -> Fill();
    fOutputFile -> cd();
    fOutputTree -> Write();
    fPar -> SetPar("Interrupted",true);
    fPar -> Write(fPar->GetName(),TObject::kSingleKey);
    fRunHeader -> Write(fRunHeader->GetName(),TObject::kSingleKey);
  }

  kb_info << "Run stoped at event " << fIdxEntry << " (" << fIdxEntry - fStartEventID << ") because of Interrupt signal (SIGINT)." << endl;
  kb_out << endl;
  Print("cio");

  if (fOutputTree != nullptr) {
    fOutputFile -> Close();
    TString linkName = TString(KEBI_PATH) + "/data/LAST_OUTPUT";
    unlink(linkName.Data());
    symlink(fOutputFileName.Data(), linkName.Data());
  }

  CheckOut();

  if (fAutoTerminate) Terminate(this,Form("Interrupted at event %lld",fEventCount));
}

void KBRun::SignalEndOfRun() { fSignalEndOfRun = true; }

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
  auto eveEventManagerGlobal = new TEveEventManager("global");
  fEveEventManagerArray -> Add(eveEventManagerGlobal);
  gEve -> AddEvent(eveEventManagerGlobal);

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

  //gEve -> GetDefaultGLViewer() -> SetClearColor(kBlack);
  gEve -> GetDefaultGLViewer() -> SetClearColor(kWhite);

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
    //plane -> GetHist() -> Draw("colz");
  }

  gEve -> GetBrowser() -> HideBottomTab();
  gEve -> ElementSelect(gEve -> GetCurrentEvent());
  gEve -> GetWindowManager() -> HideAllEveDecorations();
}

void KBRun::AddEveElementToEvent(KBContainer *eveObj, bool permanent)
{
  if (eveObj -> IsEveSet()) {
    TEveElement *eveSet = eveObj -> CreateEveElement();
    eveObj -> AddToEveSet(eveSet, fEveScale);
    AddEveElementToEvent(eveSet, permanent);
  }
  else {
    TEveElement *eveElement = eveObj -> CreateEveElement();
    eveObj -> SetEveElement(eveElement, fEveScale);
    AddEveElementToEvent(eveElement, permanent);
  }
}

void KBRun::AddEveElementToEvent(TEveElement *element, bool permanent)
{
  //gEve -> AddElement(element);
  ((TEveEventManager *) fEveEventManagerArray -> At(0)) -> AddElement(element);
  if (permanent) fPermanentEveElementList.push_back(element);
  //else fEveElementList.push_back(element);
  gEve -> Redraw3D();
}
#endif

void KBRun::RunEve(Long64_t eveEventID, TString option)
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


  if (fGraphChannelBoundaryNb[0] == nullptr) // TODO
    for (Int_t iGraph = 0; iGraph < 20; ++iGraph) {
      fGraphChannelBoundaryNb[iGraph] = new TGraph();
      fGraphChannelBoundaryNb[iGraph] -> SetLineColor(kGreen);
      fGraphChannelBoundaryNb[iGraph] -> SetLineWidth(2);
    }

  if (eveEventID>=0 && fCurrentEventID!=eveEventID)
    this -> GetEntry(eveEventID);

#ifdef ACTIVATE_EVE
  /*
  Int_t nEveElements = fEveElementList.size();
  for (Int_t iEl = 0; iEl < nEveElements; ++iEl) {
    TEveElement *el = fEveElementList.back();
    gEve -> RemoveElement(el,gEve->GetCurrentEvent());
    fEveElementList.pop_back();
  }
  */
  if (gEve!=nullptr) {
    auto numEveEvents = fEveEventManagerArray -> GetEntries();
    for (auto iEveEvent=0; iEveEvent<numEveEvents; ++iEveEvent) {
      ((TEveEventManager *) fEveEventManagerArray -> At(iEveEvent)) -> RemoveElements();
    }
    //fEveElementList.clear();
  }
#endif

  Int_t filterTrackIDs = 0;
  vector<Int_t> choosenTrackIDs;
  while (fPar -> CheckPar("eveSelectTrackIDs"))
  {
    TString eveTIDOption = fPar -> GetParString("eveSelectTrackIDs");
    TObjArray *eveTIDs = eveTIDOption.Tokenize(":");
    auto numEveTIDs = eveTIDs -> GetEntries();
    if (numEveTIDs < 2)
      break;

    TString pm = ((TObjString *) eveTIDs -> At(0)) -> GetString();
         if (pm=="+") filterTrackIDs = +1;
    else if (pm=="-") filterTrackIDs = -1;
    else break;

    TString message = "Tracklet ID selection("+pm+"): ";
    for (Int_t iID = 1; iID < numEveTIDs; ++iID) {
      TString tID = ((TObjString *) eveTIDs -> At(iID)) -> GetString();
      choosenTrackIDs.push_back(tID.Atoi());
      message = message + tID + " ";
    }
    kb_info << message << endl;
    break;
  }

  Int_t filterTrackParentIDs = 0;
  vector<Int_t> choosenTrackParentIDs;
  while (fPar -> CheckPar("eveSelectTrackParentIDs"))
  {
    TString eveTIDOption = fPar -> GetParString("eveSelectTrackParentIDs");
    TObjArray *eveTIDs = eveTIDOption.Tokenize(":");
    auto numEveTIDs = eveTIDs -> GetEntries();
    if (numEveTIDs < 2)
      break;

    TString pm = ((TObjString *) eveTIDs -> At(0)) -> GetString();
         if (pm=="+") filterTrackParentIDs = +1;
    else if (pm=="-") filterTrackParentIDs = -1;
    else break;

    TString message = "Tracklet Parent ID selection("+pm+"): ";
    for (Int_t iID = 1; iID < numEveTIDs; ++iID) {
      TString tID = ((TObjString *) eveTIDs -> At(iID)) -> GetString();
      choosenTrackParentIDs.push_back(tID.Atoi());
      message = message + tID + " ";
    }
    kb_info << message << endl;
    break;
  }

  Int_t filterTrackPDGs = 0;
  vector<Int_t> choosenTrackPDGs;
  while (fPar -> CheckPar("eveSelectTrackPDGs"))
  {
    TString eveTIDOption = fPar -> GetParString("eveSelectTrackPDGs");
    TObjArray *eveTIDs = eveTIDOption.Tokenize(":");
    auto numEveTIDs = eveTIDs -> GetEntries();
    if (numEveTIDs < 2)
      break;

    TString pm = ((TObjString *) eveTIDs -> At(0)) -> GetString();
         if (pm=="+") filterTrackPDGs = +1;
    else if (pm=="-") filterTrackPDGs = -1;
    else break;

    TString message = "Tracklet PDG selection("+pm+"): ";
    for (Int_t iID = 1; iID < numEveTIDs; ++iID) {
      TString tID = ((TObjString *) eveTIDs -> At(iID)) -> GetString();
      choosenTrackPDGs.push_back(tID.Atoi());
      message = message + tID + " ";
    }
    kb_info << message << endl;
    break;
  }

  Int_t filterHitParentIDs = 0;
  vector<Int_t> choosenHitParentIDs;
  while (fPar -> CheckPar("eveSelectHitParentIDs"))
  {
    TString eveTIDOption = fPar -> GetParString("eveSelectHitParentIDs");
    TObjArray *eveTIDs = eveTIDOption.Tokenize(":");
    auto numEveTIDs = eveTIDs -> GetEntries();
    if (numEveTIDs < 2)
      break;

    TString pm = ((TObjString *) eveTIDs -> At(0)) -> GetString();
         if (pm=="+") filterHitParentIDs = +1;
    else if (pm=="-") filterHitParentIDs = -1;
    else break;

    TString message = "Hit Parent-ID(Track-ID) selection("+pm+"): ";
    for (Int_t iID = 1; iID < numEveTIDs; ++iID) {
      TString tID = ((TObjString *) eveTIDs -> At(iID)) -> GetString();
      choosenHitParentIDs.push_back(tID.Atoi());
      message = message + tID + " ";
    }
    kb_info << message << endl;
    break;
  }

  Int_t filterMCIDs = 0;
  vector<Int_t> choosenMCIDs;
  while (fPar -> CheckPar("eveSelectMCIDs"))
  {
    TString eveTIDOption = fPar -> GetParString("eveSelectMCIDs");
    TObjArray *eveTIDs = eveTIDOption.Tokenize(":");
    auto numEveTIDs = eveTIDs -> GetEntries();
    if (numEveTIDs < 2)
      break;

    TString pm = ((TObjString *) eveTIDs -> At(0)) -> GetString();
         if (pm=="+") filterMCIDs = +1;
    else if (pm=="-") filterMCIDs = -1;
    else break;

    TString message = "Hit Parent-ID(Track-ID) selection("+pm+"): ";
    for (Int_t iID = 1; iID < numEveTIDs; ++iID) {
      TString tID = ((TObjString *) eveTIDs -> At(iID)) -> GetString();
      choosenMCIDs.push_back(tID.Atoi());
      message = message + tID + " ";
    }
    kb_info << message << endl;
    break;
  }

  bool removePointTrack = false;
  if (fPar -> CheckPar("eveRemovePointTrack"))
    removePointTrack = fPar -> GetParBool("eveRemovePointTrack");

  TObjArray *eveBranchNames = fEveBranches.Tokenize(":");
  auto numSelectedBranches = eveBranchNames -> GetEntries();
  auto numEveBranches = numSelectedBranches;

  if (numSelectedBranches == 0) {
    numEveBranches = fNumBranches;
    for (auto iBranch=0; iBranch<numEveBranches; ++iBranch) {
      TString branchName = fBranchNames[iBranch];
      if (iBranch==0)
        fEveBranches = branchName;
      else
        fEveBranches = fEveBranches + ":" + branchName;
    }
    eveBranchNames = fEveBranches.Tokenize(":");
    numSelectedBranches = eveBranchNames -> GetEntries();
  }

  kb_info << "Branches: " << fEveBranches << " (" << numSelectedBranches << ")" << endl;

#ifdef ACTIVATE_EVE
  if (drawEve3D)
  {
    if (gEve == nullptr)
      OpenEventDisplay();

    for (Int_t iBranch = 0; iBranch < numEveBranches; ++iBranch)
    {
      TObjArray *branch = nullptr;
      TString branchName = ((TObjString *) eveBranchNames -> At(iBranch)) -> GetString();

      if (numSelectedBranches != 0) {
        branch = (TObjArray *) fBranchPtrMap[branchName];
        if (branch == nullptr) {
          kb_error << "No eve-branch name " << branchName << endl;
          continue;
        }
      }
      else
        branch = (TObjArray *) fBranchPtr[iBranch];

      if (branch -> GetEntries() == 0)
        continue;

      auto objSample = branch -> At(0);
      if (objSample -> InheritsFrom("KBContainer") == false)
        continue;

      bool isTracklet = false;
      if (objSample -> InheritsFrom("KBTracklet"))
        isTracklet = true;

      bool isHit = false;
      if (objSample -> InheritsFrom("KBHit"))
        isHit = true;

      KBContainer *eveObj = (KBContainer *) objSample;
      if (numSelectedBranches == 0 || !eveObj -> DrawByDefault())
        continue;

      auto eveEvent = (TEveEventManager *) fEveEventManagerArray -> FindObject(branchName);
      if (eveEvent==nullptr) {
        eveEvent = new TEveEventManager(branchName);
        fEveEventManagerArray -> Add(eveEvent);
        gEve -> AddEvent(eveEvent);
      }

      int numSelected = 0;

      Int_t nObjects = branch -> GetEntries();
      if (isTracklet)
      {
        for (Int_t iObject = 0; iObject < nObjects; ++iObject) {
          KBTracklet *eveTrk = (KBTracklet *) branch -> At(iObject);

          if (removePointTrack)
            if (eveTrk -> InheritsFrom("KBMCTrack"))
              if (((KBMCTrack *) eveTrk) -> GetNumVertices() < 2)
                continue;

          bool isGood = 1;
               if (filterTrackIDs ==  1) { isGood = 0; for (auto id : choosenTrackIDs) { if (eveTrk->GetTrackID()==id) { isGood = 1; break; } } }
          else if (filterTrackIDs == -1) { isGood = 1; for (auto id : choosenTrackIDs) { if (eveTrk->GetTrackID()==id) { isGood = 0; break; } } }
          if (!isGood)
            continue;

               if (filterTrackParentIDs==  1) { isGood = 0; for (auto id : choosenTrackParentIDs) { if (eveTrk->GetParentID()==id) { isGood = 1; break; } } }
          else if (filterTrackParentIDs== -1) { isGood = 1; for (auto id : choosenTrackParentIDs) { if (eveTrk->GetParentID()==id) { isGood = 0; break; } } }
          if (!isGood)
            continue;

               if (filterTrackPDGs   ==  1) { isGood = 0; for (auto id : choosenTrackPDGs) { if (eveTrk->GetPDG()==id) { isGood = 1; break; } } }
          else if (filterTrackPDGs   == -1) { isGood = 1; for (auto id : choosenTrackPDGs) { if (eveTrk->GetPDG()==id) { isGood = 0; break; } } }
          if (!isGood)
            continue;

               if (filterMCIDs   ==  1) { isGood = 0; for (auto id : choosenMCIDs) { if (eveTrk->GetMCID()==id) { isGood = 1; break; } } }
          else if (filterMCIDs   == -1) { isGood = 1; for (auto id : choosenMCIDs) { if (eveTrk->GetMCID()==id) { isGood = 0; break; } } }
          if (!isGood)
            continue;

          numSelected++;

          TEveLine *eveElement = (TEveLine *) eveTrk -> CreateEveElement();
          eveTrk -> SetEveElement(eveElement, fEveScale);

          TString eveLineColorPar = Form("eveLineColor__%s",branchName.Data());
          if (fPar->CheckPar(eveLineColorPar))
            eveElement -> SetLineColor(fPar->GetParColor(eveLineColorPar));
          else if (fPar->CheckPar("eveLineColorAuto"))
            eveElement -> SetLineColor(GetColor());

          if (fPar->CheckPar("eveTrackLineWidth"))
            eveElement -> SetLineWidth(fPar->GetParInt("eveTrackLineWidth"));

          eveEvent -> AddElement(eveElement);
        }
      }
      else if (eveObj -> IsEveSet())
      {
        TEveElement *eveSet = eveObj -> CreateEveElement();
        TString name = Form("%s_%s",eveSet->GetElementName(),branchName.Data());
        eveSet -> SetElementName(name);
        for (Int_t iObject = 0; iObject < nObjects; ++iObject) {
          eveObj = (KBContainer *) branch -> At(iObject);

          if (isHit)  {
            KBHit *eveHit = (KBHit *) branch -> At(iObject);
            bool isGood = 1;
            eveHit -> SetSortValue(1);

                 if (filterHitParentIDs ==  1) { isGood = 0; for (auto id : choosenHitParentIDs)  { if (eveHit->GetTrackID()==id) { isGood = 1; break; } } }
            else if (filterHitParentIDs == -1) { isGood = 1; for (auto id : choosenHitParentIDs)  { if (eveHit->GetTrackID()==id) { isGood = 0; break; } } }
            if (!isGood) {
              eveHit -> SetSortValue(-1);
              continue;
            }

                 if (filterMCIDs ==  1) { isGood = 0; for (auto id : choosenMCIDs)  { if (eveHit->GetMCID()==id) { isGood = 1; break; } } }
            else if (filterMCIDs == -1) { isGood = 1; for (auto id : choosenMCIDs)  { if (eveHit->GetMCID()==id) { isGood = 0; break; } } }
            if (!isGood) {
              eveHit -> SetSortValue(-1);
              continue;
            }
          }

          numSelected++;

          eveObj -> AddToEveSet(eveSet, fEveScale);
        }
        eveEvent -> AddElement(eveSet);
        //fEveElementList.push_back(eveSet);

        TString namePar = Form("eveColor__%s",branchName.Data());
        if (fPar->CheckPar(namePar)) {
          auto evePointSet = (TEvePointSet *) eveSet;
          auto color = fPar -> GetParColor(namePar);
          evePointSet -> SetMarkerColor(color);
        }

        namePar = Form("eveSize__%s",branchName.Data());
        if (fPar->CheckPar(namePar)) {
          auto evePointSet = (TEvePointSet *) eveSet;
          auto size = fPar -> GetParSize(namePar);
          evePointSet -> SetMarkerSize(size);
        }
      }
      else {
        for (Int_t iObject = 0; iObject < nObjects; ++iObject) {
          eveObj = (KBContainer *) branch -> At(iObject);
          TEveElement *eveElement = eveObj -> CreateEveElement();
          eveObj -> SetEveElement(eveElement, fEveScale);
          TString name = Form("%s_%d",eveElement -> GetElementName(),iObject);
          eveElement -> SetElementName(name);
          eveEvent -> AddElement(eveElement);
          //fEveElementList.push_back(eveElement);
          numSelected++;
        }
      }

      kb_info << "Drawing " << branchName << " [" << branch -> At(0) -> ClassName() << "] " << numSelected << "(" << branch -> GetEntries() << ")" << endl;
    }

    if (fPar->CheckPar("eveAxisX")) {
      Double_t length = 100.;
      if (fPar->CheckPar("eveAxisL"))
        length = fPar->GetParDouble("eveAxisL");
      TVector3 origin(fPar->GetParDouble("eveAxisX"), fPar->GetParDouble("eveAxisY"), fPar->GetParDouble("eveAxisZ"));
      for (auto kaxis : {KBVector3::kX,KBVector3::kY,KBVector3::kZ}) {
        KBVector3 direction(0,0,0);
        direction.AddAt(length,kaxis);
        auto axis = new TEveArrow(direction.X(),direction.Y(),direction.Z(),origin.X(),origin.Y(),origin.Z()); // TODO
        if (kaxis==KBVector3::kX) axis -> SetMainColor(kRed);
        if (kaxis==KBVector3::kY) axis -> SetMainColor(kBlue);
        if (kaxis==KBVector3::kZ) axis -> SetMainColor(kBlack);
        ((TEveEventManager *) fEveEventManagerArray -> At(0)) -> AddElement(axis);
        //fEveElementList.push_back(axis);
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
        else {
          for (Int_t iBranch = 0; iBranch < numEveBranches; ++iBranch)
          {
            TString branchName = ((TObjString *) eveBranchNames -> At(iBranch)) -> GetString();
            if (branchName.Index("Hit")==0) {
              cout << branchName << " is to be filled to pad plane" << endl;
              hitArray = (TClonesArray *) fBranchPtrMap[branchName];
              hitArray -> Print();
              exist_hit = true;
              break;
            }
          }
        }
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
          padplane -> FillDataToHist("raw");
          padplane -> FillDataToHist("out");
        }
      }

      auto cvs = (TCanvas *) fCvsDetectorPlaneArray -> At(iPlane);
      cvs -> Clear();
      cvs -> cd();
      //histPlane -> SetMaximum(5000);
      histPlane -> SetMinimum(0.1);
      histPlane -> DrawClone("colz");
      histPlane -> Reset();
      histPlane -> Draw("same");
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
            {
              bool isGood = 1;
              if (filterTrackIDs ==  1) { isGood = 0; for (auto id : choosenTrackIDs)  { if (tracklet->GetTrackID() ==id) { isGood = 1; break; } } }
              else if (filterTrackIDs == -1) { isGood = 1; for (auto id : choosenTrackIDs)  { if (tracklet->GetTrackID() ==id) { isGood = 0; break; } } }
              if (!isGood)
                continue;

              if (filterTrackParentIDs==  1) { isGood = 0; for (auto id : choosenTrackParentIDs) { if (tracklet->GetParentID()==id) { isGood = 1; break; } } }
              else if (filterTrackParentIDs== -1) { isGood = 1; for (auto id : choosenTrackParentIDs) { if (tracklet->GetParentID()==id) { isGood = 0; break; } } }
              if (!isGood)
                continue;

              if (filterTrackPDGs   ==  1) { isGood = 0; for (auto id : choosenTrackPDGs)    { if (tracklet->GetPDG()     ==id) { isGood = 1; break; } } }
              else if (filterTrackPDGs   == -1) { isGood = 1; for (auto id : choosenTrackPDGs)    { if (tracklet->GetPDG()     ==id) { isGood = 0; break; } } }
              if (!isGood)
                continue;
            }
            tracklet -> TrajectoryOnPlane(axis1, axis2) -> Draw("samel");
          }
        }
      }
    }

    // @todo palette is changed when drawing top node because of TGeoMan(?)
    gStyle -> SetPalette(kBird);
  }
}

void KBRun::WriteCvsDetectorPlanes(TString format)
{
  TIter next(fCvsDetectorPlaneArray);
  TCanvas *cvs;
  while ((cvs = (TCanvas *) next()))
    cvs -> SaveAs(TString(cvs->GetName())+".event"+TString::Itoa(fCurrentEventID,10)+"."+format);
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

  fHistChannelBuffer -> Draw("hist");

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
  fSignalEndOfRun = false;
  fCheckIn = true;

  fInterruptHandler -> Add();

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

  fInterruptHandler -> Remove();

  fCheckIn = false;
}

void KBRun::PrintMCTrack(KBMCTrack *track, Option_t *)
{
  Int_t trackID = track -> GetTrackID();
  Int_t parentID = track -> GetParentID();

  TString particleName = Form("%d",track -> GetPDG());
  auto particle = GetParticle(track -> GetPDG());
  if (particle != nullptr)
    particleName = particle -> GetName();

  Int_t detectorID = track -> GetDetectorID(0);
  TString detectorName = Form("%d",detectorID);
  if (fSDTable != nullptr) {
    Int_t numDetectors = fSDTable -> GetEntries();
    for (auto iDetector=0; iDetector<numDetectors; ++iDetector) {
      auto detector = (TParameter<Int_t> *) fSDTable -> At(iDetector);
      if (detector -> GetVal() == detectorID) {
        detectorName = TString("[SD]") + detector -> GetName();
        break;
      }
    }
  }
  if (fVolumeTable != nullptr) {
    Int_t numVolumes = fVolumeTable -> GetEntries();
    for (auto iDetector=0; iDetector<numVolumes; ++iDetector) {
      auto detector = (TParameter<Int_t> *) fVolumeTable -> At(iDetector);
      if (detector -> GetVal() == detectorID) {
        detectorName = detector -> GetName();
        break;
      }
    }
  }


  Int_t processID = track -> GetCreatorProcessID();
  TString processName = Form("%d",processID);
  if (fProcessTable != nullptr) {
    Int_t numProcesses = fProcessTable -> GetEntries();
    for (auto iProcess=0; iProcess<numProcesses; ++iProcess) {
      auto process = (TParameter<Int_t> *) fProcessTable -> At(iProcess);
      if (process -> GetVal() == processID) {
        processName = process -> GetName();
        break;
      }
    }
  }

  /*
  Double_t px = track -> GetPX(0);
  Double_t py = track -> GetPY(0);
  Double_t pz = track -> GetPZ(0);
  Double_t vx = track -> GetVX(0);
  Double_t vy = track -> GetVY(0);
  Double_t vz = track -> GetVZ(0);
  */

  kr_info(0) << "MC-" << setw(4) << trackID << "(" << setw(4) << parentID << ") "
                << setw(13) << particleName
    <<     "["  << setw(20) << processName << "]"
    << " det="  << setw(20) << detectorName << ","
    << " mom="  << setw(12) << track -> Momentum().Mag() << ","
    << " len="  << setw(12) << track -> TrackLength() << endl;
    //<< " mom=(" << px << "," << py << "," << pz << "),"
    //<< " pos=(" << vx << "," << vy << "," << vz << ")" << endl;
}
