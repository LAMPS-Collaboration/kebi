#include "KBParameterContainerHolder.hh"

KBParameterContainer *KBParameterContainerHolder::CreateParameterContainer() { fPar = new KBParameterContainer(); }
void KBParameterContainerHolder::SetParameterContainer(KBParameterContainer *par) { fPar = par; }
KBParameterContainer *KBParameterContainerHolder::GetParameterConainer() { return fPar; }
