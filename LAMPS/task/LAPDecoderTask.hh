#ifndef LAMPSPROTOTYPEDECODERTASK_HH
#define LAMPSPROTOTYPEDECODERTASK_HH

#include "KBTask.hh"
#include "KBTpc.hh"
#include "TTree.h"

#include "TClonesArray.h"
#include "GETDecoder.hh"
#include "GETCoboFrame.hh"
#include "GETBasicFrame.hh"

class LAPDecoderTask : public KBTask
{ 
  public:
    LAPDecoderTask();
    virtual ~LAPDecoderTask() {}

    bool Init();
    void Exec(Option_t*);

    void SetPadPersistency(bool persistence);

    void ReadDirectory(TString directoryName);

    void AddData(TString name);
    void LoadMetaData(TString name);
    void SetNumEvents(Long64_t nEvents);

  private:
    TClonesArray* fPadArray;
    bool fPersistency;

    KBPadPlane *fPadPlane;

    Long64_t fNEvents = -1;

    GETDecoder *fDecoder;
    ifstream fPadMap;

  ClassDef(LAPDecoderTask, 1)
};

#endif
