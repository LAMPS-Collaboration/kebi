#ifndef KBPARAMETERCONTAINERHOLDER_HH
#define KBPARAMETERCONTAINERHOLDER_HH

#include "KBParameterContainer.hh"

class KBParameterContainerHolder
{
  public:
    KBParameterContainerHolder() {};
    virtual ~KBParameterContainerHolder() {};

    void CreateParameterContainer();
    void SetParameterContainer(KBParameterContainer *par);
    void SetParameterContainer(TString file);
    void AddParameterFile(TString file);
    KBParameterContainer *GetParameterContainer();

  protected:
    KBParameterContainer *fPar = nullptr;

  ClassDef(KBParameterContainerHolder, 1)
};

#endif
