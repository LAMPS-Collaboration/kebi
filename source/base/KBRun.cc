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
  fPersistentBranchArray = new TObjArray();
  fCvsDetectorPlaneArray = new TObjArray();
  fTemporaryBranchArray = new TObjArray();
  fBranchPtr = new TObject*[1000];
  for (Int_t i = 0; i < 1000; ++i) 
    fBranchPtr[i] = nullptr;
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
  TString option_string = TString(option);
  option_string.ToLower();

  if (option_string.Index("all") >= 0)
    option_string = "cpdio";

  if (option_string.Index("c") >= 0)
    KBRun::PrintKEBI();

  kb_out << endl;
  kb_out << "===========================================================================================" << endl;
  kb_info << "Run" << endl;
  if (fPar != nullptr && option_string.Index("p") >= 0) {
    kb_out << "-------------------------------------------------------------------------------------------" << endl;
    fPar -> Print();
  }

  if (fDetector != nullptr && option_string.Index("d") >= 0) {
  kb_out << "-------------------------------------------------------------------------------------------" << endl;
    fDetector -> Print();
  }

  kb_out << "-------------------------------------------------------------------------------------------" << endl;
  kb_info << "Input: " << fInputFileName << endl;
  if (fInputTree != nullptr && option_string.Index("i") >= 0)
    fInputTree -> Print("toponly");

  kb_out << "-------------------------------------------------------------------------------------------" << endl;
  kb_info << "Output: " << fOutputFileName << endl;
  if (fOutputTree != nullptr && option_string.Index("o") >= 0)
    fOutputTree -> Print("toponly");

  if (!fInitialized) {
    kb_out << "-------------------------------------------------------------------------------------------" << endl;
    kb_warning << "Print() is recommanded to be called after the initialization." << endl;
    kb_warning << "This run is not initialized. Please call Init() before Print()." << endl;

  }

  kb_out << "===========================================================================================" << endl;
}

TString KBRun::ConfigureDataPath(TString name)
{
  TString newName = name;

  if (name == "last")
    newName = TString(KEBI_PATH) + "/data/LAST_OUTPUT";
  else {
    if (newName[0] != '.' && newName[0] != '/' && newName[0] != '$' && newName != '~') {
      if (fDataPath.IsNull())
        newName = TString(KEBI_PATH) + "/data/" + newName;
      else
        newName = fDataPath + "/" + newName;
    }

    if (newName.Index(".root") != newName.Sizeof()-6)
      newName = newName + ".root";
  }

  return newName;
}

void KBRun::SetDataPath(TString path) { fDataPath = path; }
TString KBRun::GetDataPath() { return fDataPath; }

void KBRun::SetInputFile(TString fileName, TString treeName) {
  fInputFileName = ConfigureDataPath(fileName);
  fInputTreeName = treeName;
}

void KBRun::AddInput(TString fileName) { fInputFileNameArray.push_back(ConfigureDataPath(fileName)); }
void KBRun::SetInputTreeName(TString treeName) { fInputTreeName = treeName; }
TChain *KBRun::GetInputChain() { return fInputTree; }
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

  Int_t idxInput = 0;
  if (fInputFileName.IsNull() && fInputFileNameArray.size() != 0) {
    fInputFileName = fInputFileNameArray[0];
    idxInput = 1;
  }

  if (fInputFileName.IsNull() == false) {
    kb_out << endl;
    kb_print << "Input file : " << fInputFileName << endl;
    if (!CheckFileExistence(fInputFileName)) {
      kb_print << "given input file deos not exist!" << endl;
      return false;
    }
    fInputFile = new TFile(fInputFileName, "read");

    if (fInputTreeName.IsNull())
      fInputTreeName = "data";

    fInputTree = new TChain(fInputTreeName);
    fInputTree -> AddFile(fInputFileName);

    Int_t nInputs = fInputFileNameArray.size();
    for (Int_t i = idxInput; i < nInputs; i++)
      fInputTree -> AddFile(fInputFileNameArray[i]);

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
  for (auto i=0; i<7; ++i) fHash = fHash + clist[((Int_t) gRandom -> Uniform(62))];

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

  if (fDetector != nullptr) {
    fDetector -> SetParameterContainer(fPar);
    fDetector -> Init();
    kb_out << endl;
    kb_print << fDetector -> GetName() << " initialized" << endl;
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
      kb_error << "Output file is not set!" << endl;
      kb_error << "Please set output-file-name(SetOutputFile) or runID(SetRunID)." << endl;
      return false;
    }
  }
  else
    fOutputFileName = ConfigureDataPath(fOutputFileName);

  if (CheckFileExistence(fOutputFileName)) {
    //kb_warning << "  Output file " << fOutputFileName << " already exist!" << endl;
    //return false;
  }

  fLogFileName = TString(KEBI_PATH) + "/data/kbrun.log";

  kb_out << endl;
  kb_info << "Output file : " << fOutputFileName << endl;
  fOutputFile = new TFile(fOutputFileName, "recreate");
  fOutputTree = new TTree("data", "");

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

  Terminate(this);
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

void KBRun::AddDetector(KBDetector *detector) { fDetector = detector; }
KBDetector *KBRun::GetDetector() { return fDetector; }

TGeoManager *KBRun::GetGeoManager() { return fDetector -> GetGeoManager(); }

void KBRun::SetGeoTransparency(Int_t transparency)
{
  fDetector -> SetTransparency(transparency); 
}

void KBRun::SetEntries(Long64_t num) { fNumEntries = num; }
Long64_t KBRun::GetEntries() { return fNumEntries; }
Long64_t KBRun::GetNumEvents() { return fNumEntries; }

Int_t KBRun::GetEntry(Long64_t entry, Int_t getall)
{
  if (fInputTree == nullptr)
    return -1;

  fCurrentEventID = entry;
  fEventCount = entry;
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
  if (fInputTree != nullptr)
    fInputTree -> GetEntry(eventID);

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

  Terminate(this);
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
  if (fDetector == nullptr) {
    kb_warning << "Cannot open event display: detector is not set." << endl;
  }

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
  for (auto iPlane = 0; iPlane < fDetector -> GetNPlanes(); iPlane++) {
    auto slotPlane = packPA -> NewSlot(); slotPlane -> SetElementName(Form("Plane%d Slot", iPlane));
    auto ecvsPlane = new TRootEmbeddedCanvas();
    auto framPlane = slotPlane -> MakeFrame(ecvsPlane); framPlane -> SetElementName(Form("Detector Plane%d Frame", iPlane));

    TCanvas *cvs = ecvsPlane -> GetCanvas();
    fCvsDetectorPlaneArray -> Add(cvs);
    cvs -> cd();
    fDetector -> GetDetectorPlane(iPlane) -> GetHist(1) -> Draw("col");
  }

  // 2nd Row
  packOv -> SetVertical();
  auto slotCh = packOv -> NewSlotWithWeight(.35); slotCh -> SetElementName("Channel Buffer Slot");
  auto ecvsCh = new TRootEmbeddedCanvas();
  auto frameCh = slotCh -> MakeFrame(ecvsCh); frameCh -> SetElementName("Channel Buffer Frame");
  fCvsChannelBuffer = ecvsCh -> GetCanvas();

  gEve -> GetBrowser() -> GetTabRight() -> SetTab(1);
  */

  for (Int_t iPlane = 0; iPlane < fDetector -> GetNPlanes(); iPlane++) {
    KBDetectorPlane *plane = fDetector -> GetDetectorPlane(iPlane);
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

void KBRun::RunEve(Long64_t eventID)
{
  if (gEve == nullptr) {
    OpenEventDisplay();

    for (Int_t i = 0; i < 20; ++i) {
      fGraphChannelBoundaryNb[i] = new TGraph();
      fGraphChannelBoundaryNb[i] -> SetLineColor(kGreen);
      fGraphChannelBoundaryNb[i] -> SetLineWidth(2);
    }
  }

  this -> GetEntry(eventID);

  Int_t nEveElements = fEveElementList.size();
  for (Int_t iEl = 0; iEl < nEveElements; ++iEl) {
    TEveElement *el = fEveElementList.back();
    gEve -> RemoveElement(el,gEve->GetCurrentEvent());
    fEveElementList.pop_back();
  }

  for (Int_t iBranch = 0; iBranch < fNBranches; ++iBranch)
  {
    TClonesArray *branch = (TClonesArray *) fBranchPtr[iBranch];
    TObject *objSample = nullptr;

    if (branch -> GetEntries() != 0) {
      objSample = branch -> At(0);
      if (objSample -> InheritsFrom("KBContainer") == false)
        continue;
    }
    else
      continue;

    kb_info << "Drawing " << objSample -> ClassName() << endl;
    KBContainer *eveObj = (KBContainer *) objSample;
    if (eveObj -> IsEveSet()) {
      TEveElement *eveSet = eveObj -> CreateEveElement();
      Int_t nObjects = branch -> GetEntries();
      for (Int_t iObject = 0; iObject < nObjects; ++iObject) {
        eveObj = (KBContainer *) branch -> At(iObject);
        eveObj -> AddToEveSet(eveSet);
      }
      gEve -> AddElement(eveSet);
      fEveElementList.push_back(eveSet);
    }
    else {
      Int_t nObjects = branch -> GetEntries();
      for (Int_t iObject = 0; iObject < nObjects; ++iObject) {
        eveObj = (KBContainer *) branch -> At(iObject);
        TEveElement *eveElement = eveObj -> CreateEveElement();
        eveObj -> SetEveElement(eveElement);
        TString name = eveElement -> GetElementName();
        eveElement -> SetElementName(name+Form("_%d",iObject));
        gEve -> AddElement(eveElement);
        fEveElementList.push_back(eveElement);
      }
    }
  }

  gEve -> Redraw3D();

  auto hitArray = (TClonesArray *) fBranchPtrMap[TString("Hit")];
  auto padArray = (TClonesArray *) fBranchPtrMap[TString("Pad")];

  auto numPlanes = fDetector -> GetNPlanes();
  for (auto iPlane = 0; iPlane < numPlanes; ++iPlane)
  {
    auto plane = fDetector -> GetDetectorPlane(iPlane);
    auto histPlane = plane -> GetHist();
    histPlane -> Reset();

    if (fDetector -> InheritsFrom("KBTpc"))
    {
      auto padplane = (KBPadPlane *) plane;

      if (hitArray != nullptr)
      {
        padplane -> Clear();
        padplane -> SetHitArray(hitArray);
        padplane -> FillDataToHist("hit");
      }
      else if (padArray != nullptr)
      {
        padplane -> Clear();
        padplane -> SetPadArray(padArray);
        //padplane -> FillDataToHist("raw");
        padplane -> FillDataToHist("out");
      }
      else
        return;
    }

    auto cvs = (TCanvas *) fCvsDetectorPlaneArray -> At(iPlane);
    cvs -> cd();
    histPlane -> SetMaximum(5000);
    histPlane -> SetMinimum(1);
    histPlane -> Draw("colz");
    plane -> DrawFrame();

    KBVector3::Axis axis1 = plane -> GetAxis1();
    KBVector3::Axis axis2 = plane -> GetAxis2();

    for (Int_t iBranch = 0; iBranch < fNBranches; ++iBranch)
    {
      TClonesArray *branch = (TClonesArray *) fBranchPtr[iBranch];
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

  gStyle -> SetPalette(kBird); // @todo palette is changed when drawing top node because of TGeoMan(?)
}
#endif

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

  KBTpc *tpc = (KBTpc *) fDetector;
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

  for (Int_t i = 0; i < 20; ++i)
    fGraphChannelBoundaryNb[i] -> Set(0);

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
  fCvsChannelBuffer -> Modified();
  fCvsChannelBuffer -> Update();

  ((TCanvas *) fCvsDetectorPlaneArray -> At(0)) -> cd();
  fGraphChannelBoundary -> Draw("samel");
  for (auto iNb = 0; iNb < numNbs; ++iNb)
    fGraphChannelBoundaryNb[iNb] -> Draw("samel");
}

bool KBRun::CheckFileExistence(TString fileName)
{
  TString name = gSystem -> Which(".", fileName.Data());
  if (name.IsNull())
    kb_info << fileName << " IS NEW." << endl;
  else
    kb_info << fileName << " EXIST!" << endl;

  if (name.IsNull())
    return false;
  return true;
}

void KBRun::CheckIn()
{
  fstream logFile(fLogFileName.Data(), ios::out | ios::app);

  time_t t = time(0);
  struct tm * now = localtime(&t);
  TString ldate = Form("%04d.%02d.%02d",now->tm_year+1900,now->tm_mon+1,now->tm_mday);
  TString ltime = Form("%02d:%02d",now->tm_hour,now->tm_min);
  TString lname = KBRun::GetKEBIUserName();
  TString lversion = KBRun::GetKEBIVersion();
  TString linput = fInputFileName.IsNull() ? "-" : fInputFileName;
  TString loutput = fOutputFileName.IsNull() ? "-" : fOutputFileName;

  logFile << fHash << "  " << ldate << "  " << ltime << "  START  " << lname << "  " << lversion << "  "
          << "in:" << linput << "  out:" << loutput << "  " << endl;

  logFile.close();
}

void KBRun::CheckOut()
{
  fstream logFile(fLogFileName.Data(), ios::out | ios::app);

  time_t t = time(0);
  struct tm * now = localtime(&t);
  TString ldate = Form("%04d.%02d.%02d",now->tm_year+1900,now->tm_mon+1,now->tm_mday);
  TString ltime = Form("%02d:%02d",now->tm_hour,now->tm_min);

  logFile << fHash << "  " << ldate << "  " << ltime << "  END" << endl;
  logFile.close();
}
