#ifndef KBRUN_HH
#define KBRUN_HH

#include "KBCompiled.h"

#include "KBTask.hh"
#include "KBParameterContainer.hh"
#include "KBParameterContainerHolder.hh"
#include "KBDetector.hh"

#include "KBTpc.hh"
#include "KBPadPlane.hh"

#include "TError.h"

#include "TH1D.h"
#include "TGraph.h"

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TObject.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#ifdef ACTIVATE_EVE
#include "TEveEventManager.h"
#include "TEveEventManager.h"
#endif

#include <map>
#include <vector>
#include <fstream>
using namespace std;

class KBRun : public KBTask, public KBParameterContainerHolder
{
  public:
    static KBRun* GetRun(); ///< Get KBRun static pointer.

    KBRun(); ///< Do not use this constructor. Use GetRun() only.
    virtual ~KBRun() {};

    static TString GetKEBIVersion();      ///< Compiled KEBI version
    static TString GetGETDecoderVersion();  ///< Compiled GETDecoder version
    static TString GetKEBIHostName();     ///< KEBI Host name
    static TString GetKEBIUserName();     ///< KEBI User name
    static TString GetKEBIPath();         ///< Compiled KEBI path
    static void PrintKEBI();              ///< Print compiled KEBI information

    /**
     * @param option
     *   - "all" : Print all. Same as "cpdio". (Default)
     *   - "c" : Print compiled info.
     *   - "p" : Print parameter container
     *   - "d" : Print detector
     *   - "i" : Print input
     *   - "o" : Print output
     */
    virtual void Print(Option_t *option="all") const;

    void SetRunName(TString name);
    TString GetRunName() const;

    void SetRunID(Int_t id);
    Int_t GetRunID() const;

    void SetDataPath(TString path); ///< Set data directory path. Default directory : path/to/KEBI/data
    TString GetDataPath();

    void SetInputFile(TString fileName, TString treeName = "data"); ///< Set input file and tree name
    void AddInput(TString fileName); ///< Add file to input file
    void SetInputTreeName(TString treeName); ///< Set input tree name
    TChain *GetInputChain();

    void SetOutputFile(TString name); ///< Set output file name
    TTree *GetOutputTree();
    void SetTag(TString tag);
    void SetSplit(Int_t split, Long64_t numSplitEntries);

    void SetIOFile(TString inputName, TString outputName, TString treeName = "data");

    bool Init(); ///< Initailize KBRun. Init() must be done before Run().

    void CreateParameterFile(TString name);

    /**
     * Register obj as a output branch with given name.
     * obj will not be registered if same name already exist in the branch list and return fail.
     * If persistent is true, branch will write it's data to output tree.
     */
    bool RegisterBranch(TString name, TObject *obj, bool persistent);
    TObject *GetBranch(TString name); ///< Get branch by name.
    TClonesArray *GetBranchA(TString name); ///< Get TClonesArray branch by name.

    void AddDetector(KBDetector *detector); ///< Set detector
    KBDetector *GetDetector();

    TGeoManager *GetGeoManager();
    void SetGeoTransparency(Int_t transparency); ///< Set transparency of geometry. Will show in eve.
    void SetEntries(Long64_t num); ///< Set total number of entries. Use only input do not exist.
    Long64_t GetEntries(); ///< Get total number of entries
    Long64_t GetNumEvents();
    Int_t GetEntry(Long64_t entry = 0, Int_t getall = 0); ///< GetEntry from input tree
    Int_t GetEvent(Long64_t entry);
    bool GetNextEvent();

    Long64_t GetStartEventID() const;   ///< Get starting eventID
    Long64_t GetEndEventID() const;     ///< Get ending eventID
    Long64_t GetCurrentEventID() const; ///< Get current eventID

    /// Get eventID count; event_count = current_eventID - start_eventID
    Long64_t GetEventCount() const;

    bool Event(Long64_t eventID);
    bool NextEvent();

    void Run(); ///< Run all events
    void EndOfEvent();

    void RunSingle(Long64_t eventID); ///< Run single event given eventID
    void RunInRange(Long64_t startID, Long64_t endID); ///< Run in range from startID to endID
    void RunInEventRange(Long64_t startID, Long64_t endID); ///< @todo Write this method

#ifdef ACTIVATE_EVE
    void RunEve(Long64_t eventID); ///< Run eventdisplay of given eventID
    void SetEve(TString option);
#endif

    static void ClickSelectedPadPlane();
    void DrawPadByPosition(Double_t x, Double_t y);

    void SetAutoTermination(Bool_t val);
    void Terminate(TObject *obj, TString message = "");

  private:
    TString ConfigureDataPath(TString namname);
    bool CheckFileExistence(TString fileName);
#ifdef ACTIVATE_EVE
    void OpenEventDisplay();
#endif
    void CheckIn();
    void CheckOut();

  private:
    TString fRunName = "";
    Int_t fRunID = -1;

    bool fInitialized = false;

    TString fDataPath = "";

    TString fInputFileName = "";
    TString fInputTreeName = "";
    TFile *fInputFile = nullptr;
    TChain *fInputTree = nullptr;

    vector<TString> fInputFileNameArray;

    TString fOutputFileName = "";
    TString fTag = "";
    Int_t fSplit = -1;
    Long64_t fNumSplitEntries = -1;
    TFile *fOutputFile = nullptr;
    TTree *fOutputTree = nullptr;

    TObjArray *fPersistentBranchArray = nullptr;
    TObjArray *fTemporaryBranchArray = nullptr;

    Int_t fNBranches = 0;
    TObject **fBranchPtr;
    std::map<TString, TObject*> fBranchPtrMap;

    Long64_t fNumEntries = 0;

    Long64_t fStartEventID = -1;
    Long64_t fEndEventID = -1;
    Long64_t fCurrentEventID = 0;
    Long64_t fEventCount = 0;
    bool fSignalEndOfEvent = false;

    KBParameterContainer *fRunHeader = nullptr;

    KBDetector *fDetector = nullptr;

#ifdef ACTIVATE_EVE
    TEveEventManager *fEveEventManager = nullptr;
    std::vector<TEveElement *> fEveElementList;
    TString fEveOption;
#endif

    TObjArray *fCvsDetectorPlaneArray = nullptr;
    TCanvas *fCvsChannelBuffer = nullptr;
    TH1D *fHistChannelBuffer = nullptr;
    TGraph *fGraphChannelBoundary = nullptr;
    TGraph *fGraphChannelBoundaryNb[20];

    TString fLogFileName;
    TString fHash;

    Bool_t fAutoTerminate = true;

  private:
    static KBRun *fInstance;


    ClassDef(KBRun, 1)
};

#endif
