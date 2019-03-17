#ifndef KBGEAR_HH
#define KBGEAR_HH

#include <iostream>
#include <fstream>
#include "KBGlobal.hh"
#include "KBParameterContainer.hh"

/**
 * Virtual class for stear classes of KEBI
*/

class KBGear
{
  public:
    KBGear() {};
    virtual ~KBGear() {};

    void CreateParameterContainer();
    void SetParameterContainer(KBParameterContainer *par);
    void SetParameterContainer(TString file);
    void AddParameterFile(TString file);
    KBParameterContainer *GetParameterContainer();

    virtual void SetRank(Int_t rank);
    Int_t GetRank();

  protected:
    KBParameterContainer *fPar = nullptr;

    Int_t fRank = 0;

  ClassDef(KBGear, 1)
};

#endif
