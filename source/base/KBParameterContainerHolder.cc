#include "KBParameterContainerHolder.hh"

void KBParameterContainerHolder::CreateParameterContainer() { fPar = new KBParameterContainer(); }
void KBParameterContainerHolder::SetParameterContainer(KBParameterContainer *par) { fPar = par; }
void KBParameterContainerHolder::SetParameterContainer(TString file) { fPar = new KBParameterContainer(file); }
void KBParameterContainerHolder::AddParameterFile(TString file) { if (fPar == nullptr) fPar = new KBParameterContainer(); fPar -> AddFile(file); }
KBParameterContainer *KBParameterContainerHolder::GetParameterContainer() { return fPar; }
