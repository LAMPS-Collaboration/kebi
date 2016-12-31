#include "KBRun.hh"

#include "TSystem.h"
#include "TStyle.h"
#include "TApplication.h"

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

#include "KBHit.hh"
#include "KBHelixTrack.hh"
#include "KBMCStep.hh"
#include "TEvePointSet.h"
#include "TEveLine.h"

#include <iostream>
using namespace std;

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
  fPersistentBranchArray = new TObjArray();
  fCvsDetectorPlaneArray = new TObjArray();
  fTemporaryBranchArray = new TObjArray();
  fBranchPtr = new TObject*[1000];
  for (Int_t i = 0; i < 1000; i++) 
    fBranchPtr[i] = nullptr;
}

TString KBRun::GetKEBIVersion()     { return TString(KEBI_VERSION); }
TString KBRun::GetGETDecoderVersion() { return TString(GETDECODER_VERSION); }
TString KBRun::GetKEBIHostName()    { return TString(KEBI_HOSTNAME); }
TString KBRun::GetKEBIUserName()    { return TString(KEBI_USERNAME); }
TString KBRun::GetKEBIPath()        { return TString(KEBI_PATH); }

void KBRun::PrintKEBI()
{
  cout << "===========================================================================================" << endl;
  cout << "[KEBI] Compiled Information" << endl;
  cout << "-------------------------------------------------------------------------------------------" << endl;
  cout << "  KEBI Version     : " << KBRun::GetKEBIVersion() << endl;
  cout << "  GETDecoder Version : " << KBRun::GetGETDecoderVersion() << endl;
  cout << "  KEBI Host Name   : " << KBRun::GetKEBIHostName() << endl;
  cout << "  KEBI User Name   : " << KBRun::GetKEBIUserName() << endl;
  cout << "  KEBI Path        : " << KBRun::GetKEBIPath() << endl;
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

  cout << endl;
  cout << "===========================================================================================" << endl;
  cout << "[KBRun] Run Information" << endl;
  if (fPar != nullptr && option_string.Index("p") >= 0) {
    cout << "-------------------------------------------------------------------------------------------" << endl;
    fPar -> Print();
  }

  if (fDetector != nullptr && option_string.Index("d") >= 0) {
  cout << "-------------------------------------------------------------------------------------------" << endl;
    fDetector -> Print();
  }

  cout << "-------------------------------------------------------------------------------------------" << endl;
  cout << "[KBRun Input] " << fInputFileName << endl;
  if (fInputTree != nullptr && option_string.Index("i") >= 0)
    fInputTree -> Print("toponly");

  cout << "-------------------------------------------------------------------------------------------" << endl;
  cout << "[KBRun Output] " << fOutputFileName << endl;
  if (fOutputTree != nullptr && option_string.Index("o") >= 0)
    fOutputTree -> Print("toponly");

  cout << "===========================================================================================" << endl;
}

TString KBRun::ConfigureDataPath(TString name)
{
  TString newName = name;

  if (newName[0] != '/' && newName[0] != '$' && newName != '~') {
    if (fDataPath.IsNull())
      newName = TString(gSystem -> Getenv("KEBIPATH")) + "/data/" + newName;
    else
      newName = fDataPath + "/" + newName;
  }

  if (newName.Index(".root") != newName.Sizeof()-6)
    newName = newName + ".root";

  return newName;
}

void KBRun::SetDataPath(TString path) { fDataPath = path; }
TString KBRun::GetDataPath() { return fDataPath; }

void KBRun::SetInputFile(TString fileName, TString treeName) {
  fInputFileName = ConfigureDataPath(fileName);
  fInputTreeName = treeName;
}

void KBRun::AddFriend(TString fileName) { fFriendFileNameArray.push_back(ConfigureDataPath(fileName)); }
void KBRun::SetOutputFile(TString name) { fOutputFileName = ConfigureDataPath(name); }

bool KBRun::Init()
{
  Int_t idxFriend = 0;
  if (fInputFileName.IsNull() && fFriendFileNameArray.size() != 0) {
    fInputFileName = fFriendFileNameArray[0];
    idxFriend = 1;
  }

  if (fInputFileName.IsNull() == false) {
    cout << endl;
    cout << "[KBRun] Input file : " << fInputFileName << endl;
    fInputFile = new TFile(fInputFileName, "read");

    fInputTree = new TChain(fInputTreeName);
    fInputTree -> AddFile(fInputFileName);
    fNumEntries = fInputTree -> GetEntries();
    cout << "  " << fInputTree -> GetName() << " tree containing " << fInputTree -> GetEntries() << " entries." << endl;

    TObjArray *branchArray = fInputTree -> GetListOfBranches();
    Int_t nBranches = branchArray -> GetEntries();
    for (Int_t iBranch = 0; iBranch < nBranches; iBranch++) {
      TBranch *branch = (TBranch *) branchArray -> At(iBranch);
      fInputTree -> SetBranchStatus(branch -> GetName(), 1);
      fInputTree -> SetBranchAddress(branch -> GetName(), &fBranchPtr[fNBranches]);
      fBranchPtrMap[branch -> GetName()] = fBranchPtr[fNBranches];
      fNBranches++;
      cout << "  Input branch " << branch -> GetName() << " found" << endl;
    }

    Int_t nFriends = fFriendFileNameArray.size();
    for (auto i = idxFriend; i < nFriends; i++) {
      auto chain = new TChain("data");
      chain -> AddFile(fFriendFileNameArray[i]);
      fInputTree -> AddFriend(chain);

      branchArray = chain -> GetListOfBranches();
      nBranches = branchArray -> GetEntries();
      for (Int_t iBranch = 0; iBranch < nBranches; iBranch++) {
        TBranch *branch = (TBranch *) branchArray -> At(iBranch);
        chain -> SetBranchStatus(branch -> GetName(), 1);
        chain -> SetBranchAddress(branch -> GetName(), &fBranchPtr[fNBranches]);
        fBranchPtrMap[branch -> GetName()] = fBranchPtr[fNBranches];
        fNBranches++;
        cout << "  Input branch " << branch -> GetName() << " found" << endl;
      }
    }

  }

  if (fPar == nullptr) {
    if (fInputFile != nullptr && fInputFile -> Get("ParameterContainer") != nullptr)
      fPar = (KBParameterContainer *) fInputFile -> Get("ParameterContainer");
    else {
      cout << "[KBRun] FAILED to load parameter container." << endl;
      return false;
    }
  }

  if (fDetector != nullptr) {
    fDetector -> SetParameterContainer(fPar);
    fDetector -> Init();
    cout << endl;
    cout << fDetector -> GetName() << " initialized" << endl;
  }

  if (fOutputFileName.IsNull() == false) {
    cout << endl;
    cout << "[KBRun] Output file : " << fOutputFileName << endl;
    fOutputFile = new TFile(fOutputFileName, "recreate");
    fOutputTree = new TTree("data", "");
  }

  fInitialized = InitTasks();

  cout << endl;
  if (fInitialized)
    cout << "[KBRun] KBRun initialized" << endl;
  else {
    cout << "[KBRun] FAILED initializing tasks." << endl;
    cout << "        Exit run." << endl;
    Terminate(this);
  }

  fRunHeader = new KBParameterContainer();
  fRunHeader -> SetPar("KEBIVersion",KBRun::GetKEBIVersion());
  fRunHeader -> SetPar("GETDecoderVersion",KBRun::GetGETDecoderVersion());
  fRunHeader -> SetPar("KEBIHostName",KBRun::GetKEBIHostName());
  fRunHeader -> SetPar("KEBIUserName",KBRun::GetKEBIUserName());
  fRunHeader -> SetPar("KEBIPath",KBRun::GetKEBIPath());

  return fInitialized;
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
    cout << "    -Output branch " << name << " (persistent)" << endl;
  } else {
    fTemporaryBranchArray -> Add(obj);
    cout << "    -Output branch " << name << " (temporary)" << endl;
  }

  return true;
}

TObject *KBRun::GetBranch(TString name)
{
  TObject *dataContainer = fBranchPtrMap[name];
  return dataContainer;
}

void KBRun::AddParameterFile(TString name)
{ 
  if (fPar == nullptr)
    fPar = new KBParameterContainer();

  fPar -> AddFile(name); 
}

KBParameterContainer *KBRun::GetParameterContainer()
{
  return fPar;
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

Int_t KBRun::GetEntry(Long64_t entry, Int_t getall)
{
  if (fInputTree == nullptr)
    return -1;

  fCurrentEventID = entry;
  fEventCount = entry;
  return fInputTree -> GetEntry(entry, getall);
}

Long64_t KBRun::GetStartEventID() const { return fStartEventID; }
Long64_t KBRun::GetEndEventID() const { return fEndEventID; }
Long64_t KBRun::GetCurrentEventID() const { return fCurrentEventID; }
Long64_t KBRun::GetEventCount() const { return fEventCount; }

void KBRun::Run()
{
  cout << endl;
  if (fInitialized == false) {
    cout << "[KBRun] KBRun is not Initialized!" << endl;
    cout << "        Exit run" << endl;
    return;
  }

  if (fStartEventID == -1 && fEndEventID == -1) {
    fStartEventID = 0;
    fEndEventID = fNumEntries;
  }

  fEventCount = 0;
  for (Long64_t iEntry = fStartEventID; iEntry <= fEndEventID; iEntry++) {
    fCurrentEventID = iEntry;
    if (fInputTree != nullptr) {
      fInputTree -> GetEntry(iEntry);
      ///@todo fCurrentEventID = EventHeader -> GetEventID();
    }

    cout << "[KBRun] Execute Run " << iEntry << endl;
    ExecuteTask("");
    if (fOutputTree != nullptr)
      fOutputTree -> Fill();

    fEventCount++;
  }

  if (fOutputTree != nullptr) {
    fOutputFile -> cd();
    fOutputTree -> Write();
    fPar -> Write(fPar->GetName(),TObject::kSingleKey);
  }

  cout << endl;
  cout << "[KBRun] End of Run " << fStartEventID << " -> " << fEndEventID << " (" << fEndEventID - fStartEventID + 1 << ")" << endl;

  cout << endl;
  Print("cio");

  if (fOutputTree != nullptr)
    fOutputFile -> Close();

  Terminate(this);
}

void KBRun::RunSingle(Long64_t eventID)
{
  if (eventID < 0 || eventID > fNumEntries - 1) {
    cout << "[KBRun] eventID not in proper range." << endl;
    cout << "        eventID : " << eventID << endl;
    cout << "        entry range : " << 0 << " -> " << fNumEntries - 1 << endl;
    cout << "        Exit run" << endl;
    return;
  }

  fStartEventID = eventID;
  fEndEventID = eventID;
  Run();
}

void KBRun::RunInRange(Long64_t startID, Long64_t endID)
{
  if (startID > endID || startID < 0 || endID > fNumEntries - 1) {
    cout << "[KBRun] startID and endID not in proper range." << endl;
    cout << "        startID / endID : " << startID << " / " << endID << endl;
    cout << "        entry range : " << 0 << " -> " << fNumEntries - 1 << endl;
    cout << "        Exit run" << endl;
    return;
  }

  fStartEventID = startID;
  fEndEventID = endID;
  Run();
}

void KBRun::RunInEventRange(Long64_t, Long64_t)
{
}

void KBRun::OpenEventDisplay()
{
  if (fDetector == nullptr) {
    cout << "Cannot open event display: detector is not set." << endl;
  }

  if (gEve != nullptr)
    return;

  TEveManager::Create(true, "V");
  fEveEventManager = new TEveEventManager();
  gEve -> AddEvent(fEveEventManager);

  {
    Int_t dummy;
    UInt_t w, h; UInt_t wMax = 1400;
    UInt_t hMax = 850;
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

  gEve -> GetDefaultViewer() -> GetGLViewer() -> SetClearColor(kWhite);

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

  for (auto iPlane = 0; iPlane < fDetector -> GetNPlanes(); iPlane++) {
    auto plane = fDetector -> GetDetectorPlane(iPlane);
    auto cvs = plane -> GetCanvas();
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
  if (gEve == nullptr)
    OpenEventDisplay();

  this -> GetEntry(eventID);

  auto fTpc = (KBTpc *) fDetector;

  auto padplane = fTpc -> GetPadPlane(0);
  auto hist_padplane = padplane -> GetHist();

  auto hitArray = (TClonesArray *) fBranchPtrMap[TString("Hit")];
  auto hitEveSet = new TEvePointSet("hit");
  hitEveSet -> SetMarkerColor(kBlack);
  hitEveSet -> SetMarkerSize(0.8);
  hitEveSet -> SetMarkerStyle(20);
  gEve -> AddElement(hitEveSet);
  
  Double_t threshold;
  fPar -> GetParDouble("ADCThreshold", threshold);

  auto nHits = hitArray -> GetEntriesFast();
  for (auto iHit = 0; iHit < nHits; iHit++) {
    auto hit = (KBHit *) hitArray -> At(iHit);
    if (hit -> GetCharge() < threshold) 
      continue;

    hitEveSet -> SetNextPoint(hit -> GetX(), hit -> GetY(), hit -> GetZ());
  }

  gEve -> Redraw3D();


  auto padArray = (TClonesArray *) fBranchPtrMap[TString("Pad")];
  padplane -> SetPadArray(padArray);
  padplane -> FillBufferToHist("out");

  gStyle -> SetPalette(kBird); // @todo palette is changed when Drawing top node

  auto cvs = (TCanvas *) fCvsDetectorPlaneArray -> At(0);
  cvs -> cd();
  hist_padplane -> Draw("colz");
  padplane -> DrawFrame();
}

void Terminate(KBTask *obj, TString message)
{
  cout << "Terminated from [" << obj -> GetName() << "] " << message << endl;
  gApplication -> Terminate();
}