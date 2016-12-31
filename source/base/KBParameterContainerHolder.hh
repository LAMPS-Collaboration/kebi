#ifndef KBPARAMETERCONTAINERHOLDER_HH
#define KBPARAMETERCONTAINERHOLDER_HH

#include "KBParameterContainer.hh"

class KBParameterContainerHolder
{
  public:
    KBParameterContainerHolder() {};
    virtual ~KBParameterContainerHolder() {};

    KBParameterContainer *CreateParameterContainer();
    void SetParameterContainer(KBParameterContainer *par);
    KBParameterContainer *GetParameterConainer();

  protected:
    KBParameterContainer *fPar;

  ClassDef(KBParameterContainerHolder, 1)
};

#endif
