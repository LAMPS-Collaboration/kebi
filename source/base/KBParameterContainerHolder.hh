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
    KBParameterContainer *GetParameterContainer();

  protected:
    KBParameterContainer *fPar;

  ClassDef(KBParameterContainerHolder, 1)
};

#endif
