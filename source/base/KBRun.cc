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
#include "KBContainer.hh"
#include "KBMCStep.hh"
#include "TEvePointSet.h"
#include "TEveLine.h"

#include <unistd.h>
#include <iostream>
#include <fstream>
using namespace std;

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
  for (Int_t i = 0; i < 1000; i++) 
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

  if (name == "last")
    newName = TString(KEBI_PATH) + "/data/LAST_OUTPUT";
  else {
    if (newName[0] != '/' && newName[0] != '$' && newName != '~') {
      if (fDataPath.IsNull())
        newName = TString(gSystem -> Getenv("KEBIPATH")) + "/data/" + newName;
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
void KBRun::SetOutputFile(TString name) { fOutputFileName = name; }
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

  cout << "[KBRun] Initializing" << endl;

  Int_t idxInput = 0;
  if (fInputFileName.IsNull() && fInputFileNameArray.size() != 0) {
    fInputFileName = fInputFileNameArray[0];
    idxInput = 1;
  }

  if (fInputFileName.IsNull() == false) {
    cout << endl;
    cout << "[KBRun] Input file : " << fInputFileName << endl;
    if (!CheckFileExistence(fInputFileName)) {
      cout << "  Input file " << fInputFileName << " does not exist!" << endl;
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
  }

  if (fPar == nullptr) {
    if (fInputFile != nullptr && fInputFile -> Get("ParameterContainer") != nullptr)
      fPar = (KBParameterContainer *) fInputFile -> Get("ParameterContainer");
    else {
      cout << "[KBRun] FAILED to load parameter container." << endl;
      return false;
    }
  }

  if (fInputFile != nullptr && fInputFile -> Get("RunHeader") != nullptr) {
    KBParameterContainer *runHeaderIn = (KBParameterContainer *) fInputFile -> Get("RunHeader");
    Int_t runID;
    runHeaderIn -> GetParInt("RunID",runID);

    if (fRunID == -1)
      fRunID = runID;
    else if (runID != -1 && fRunID != runID) {
      cout << "[KBRun] Run-ID for input and output file do not match!" << endl;
      return false;
    }
  }

  fRunHeader = new KBParameterContainer();
  fRunHeader -> SetName("RunHeader");
  fRunHeader -> SetPar("KEBIVersion",KBRun::GetKEBIVersion());
  fRunHeader -> SetPar("GETDecoderVersion",KBRun::GetGETDecoderVersion());
  fRunHeader -> SetPar("KEBIHostName",KBRun::GetKEBIHostName());
  fRunHeader -> SetPar("KEBIUserName",KBRun::GetKEBIUserName());
  fRunHeader -> SetPar("KEBIPath",KBRun::GetKEBIPath());
  fRunHeader -> SetPar("RunID",fRunID);
  if (fInputFileName.IsNull() == false)
    fRunHeader -> SetPar("InputFile",fInputFileName);

  if (fDetector != nullptr) {
    fDetector -> SetParameterContainer(fPar);
    fDetector -> Init();
    cout << endl;
    cout << "[KBRun] " << fDetector -> GetName() << " initialized" << endl;
  }

  if (fOutputFileName.IsNull()) {
    if (fRunID != -1) {
      fOutputFileName = Form("run%04d", fRunID);

      if (!fTag.IsNull())
        fOutputFileName = fOutputFileName + "." + fTag;
      else
        fOutputFileName = fOutputFileName + ".out";

      if (!fTag.IsNull())
        fOutputFileName = fOutputFileName + Form(".%d",fSplit);

      fOutputFileName = ConfigureDataPath(fOutputFileName);
    }
    else {
      cout << "[KBRun] Output file is not set!" << endl;
      cout << "        Please set output-file-name(SetOutputFile) or runID(SetRunID)." << endl;
      return false;
    }
  }

  if (CheckFileExistence(fOutputFileName)) {
    cout << "  Output file " << fOutputFileName << " already exist!" << endl;
    //return false;
  }

  cout << endl;
  cout << "[KBRun] Output file : " << fOutputFileName << endl;
  fOutputFile = new TFile(fOutputFileName, "recreate");
  fOutputTree = new TTree("data", "");

  fInitialized = InitTasks();

  cout << endl;
  if (fInitialized) {
    cout << "[KBRun] " << fNumEntries << " input entries" << endl;
    cout << "[KBRun] KBRun initialized" << endl;
  }
  else
    cout << "[KBRun] FAILED initializing tasks." << endl;

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
    fEndEventID = fNumEntries-1;
  }

  if (fSplit != -1) {
    fStartEventID = fSplit * fNumSplitEntries;
    fEndEventID = ((fSplit+1) * fNumSplitEntries) - 1 ;
    if (fEndEventID > fNumEntries - 1)
      fEndEventID = fNumEntries - 1;
  }

  Int_t numRunEntries = fEndEventID - fStartEventID + 1;

  fEventCount = 0;
  for (Long64_t iEntry = fStartEventID; iEntry <= fEndEventID; iEntry++) {
    fCurrentEventID = iEntry;
    if (fInputTree != nullptr) {
      fInputTree -> GetEntry(iEntry);
      ///@todo fCurrentEventID = EventHeader -> GetEventID();
    }

    cout << "[KBRun] Execute Run " << iEntry << " (" << fEventCount << "/" << numRunEntries << ")" << endl;
    ExecuteTask("");
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

  cout << endl;
  cout << "[KBRun] End of Run " << fStartEventID << " -> " << fEndEventID << " (" << fEndEventID - fStartEventID + 1 << ")" << endl;

  cout << endl;
  Print("cio");

  if (fOutputTree != nullptr) {
    fOutputFile -> Close();
    TString linkName = TString(KEBI_PATH) + "/data/LAST_OUTPUT";
    unlink(linkName.Data());
    symlink(fOutputFileName.Data(), linkName.Data());
  }

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
  if (gEve == nullptr)
    OpenEventDisplay();

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

    if (branch -> GetEntriesFast() != 0) {
      objSample = branch -> At(0);
      if (objSample -> InheritsFrom("KBContainer") == false)
        continue;
    }
    else
      continue;

    KBContainer *eveObj = (KBContainer *) objSample;
    if (eveObj -> IsEveSet()) {
      TEveElement *eveSet = eveObj -> CreateEveElement();
      Int_t nObjects = branch -> GetEntriesFast();
      for (Int_t iObject = 0; iObject < nObjects; ++iObject) {
        eveObj = (KBContainer *) branch -> At(iObject);
        eveObj -> AddToEveSet(eveSet);
      }
      gEve -> AddElement(eveSet);
      fEveElementList.push_back(eveSet);
    }
    else {
      Int_t nObjects = branch -> GetEntriesFast();
      for (Int_t iObject = 0; iObject < nObjects; ++iObject) {
        eveObj = (KBContainer *) branch -> At(iObject);
        TEveElement *eveElement = eveObj -> CreateEveElement();
        eveObj -> SetEveElement(eveElement);
        gEve -> AddElement(eveElement);
        fEveElementList.push_back(eveElement);
      }
    }
  }

  gEve -> Redraw3D();

  KBTpc *tpc = (KBTpc *) fDetector;
  KBPadPlane *padplane = tpc -> GetPadPlane(0);
  TH2 *hist_padplane = padplane -> GetHist();
  hist_padplane -> Reset();

  TClonesArray *hitArray = (TClonesArray *) fBranchPtrMap[TString("Hit")];
  TClonesArray *padArray = (TClonesArray *) fBranchPtrMap[TString("Pad")];

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
    padplane -> FillDataToHist("raw");
    //padplane -> FillDataToHist("out");
  }
  else
    return;

  gStyle -> SetPalette(kBird); // @todo palette is changed when Drawing top node
  TCanvas *cvs = (TCanvas *) fCvsDetectorPlaneArray -> At(0);
  cvs -> cd();
  hist_padplane -> SetMaximum(5000);
  //hist_padplane -> SetMinimum(1);
  hist_padplane -> Draw("colz");
  padplane -> DrawFrame();
}

void KBRun::Terminate(TObject *obj, TString message)
{
  cout << "Terminated from [" << obj -> GetName() << "] " << message << endl;
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

  if (fGraphChannelBoundary == nullptr) {
    fGraphChannelBoundary = new TGraph();
    fGraphChannelBoundary -> SetLineColor(kRed);
    fGraphChannelBoundary -> SetLineWidth(2);
  }
  fGraphChannelBoundary -> Set(0);

  KBTpc *tpc = (KBTpc *) fDetector;
  KBPadPlane *padplane = tpc -> GetPadPlane();
  Int_t id = padplane -> FindPadID(x, y);
  if (id < 0) {
    cout << "Could not find pad at position: " << x << ", " << y << endl;
    return;
  }

  KBPad *pad = padplane -> GetPad(id);
  pad -> SetHist(fHistChannelBuffer,"pao");
  pad -> Print();

  vector<TVector2> *corners = pad -> GetPadCorners();
  for (UInt_t iCorner = 0; iCorner < corners -> size(); ++iCorner) {
    TVector2 corner = corners -> at(iCorner);
    cout << "corner: " << corner.X() << ", " <<  corner.Y() << endl;
    fGraphChannelBoundary -> SetPoint(fGraphChannelBoundary -> GetN(), corner.X(), corner.Y());
  }

  fHistChannelBuffer -> Draw("l");
  fCvsChannelBuffer -> Modified();
  fCvsChannelBuffer -> Update();

  ((TCanvas *) fCvsDetectorPlaneArray -> At(0)) -> cd();
  fGraphChannelBoundary -> Draw("samel");
}

bool KBRun::CheckFileExistence(TString fileName)
{
  TString name = gSystem -> Which(".", fileName.Data());
  if (name.IsNull())
    cout << "[KBRun::CheckFileExistence] " << fileName << " IS NEW." << endl;
  else
    cout << "[KBRun::CheckFileExistence] " << fileName << " ALREADY EXIST!" << endl;

  if (name.IsNull())
    return false;
  return true;
}
