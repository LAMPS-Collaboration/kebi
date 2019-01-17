#include "KBGlobal.hh"

void KBGlobal::CreateParameterContainer() { fPar = new KBParameterContainer(); }
void KBGlobal::SetParameterContainer(KBParameterContainer *par) { fPar = par; }
void KBGlobal::SetParameterContainer(TString file) { fPar = new KBParameterContainer(file); }
void KBGlobal::AddParameterFile(TString file) {
  if (fPar == nullptr)
    fPar = new KBParameterContainer();
  fPar -> AddFile(file);
}
KBParameterContainer *KBGlobal::GetParameterContainer() { return fPar; }

void KBGlobal::SetRank(Int_t rank) { fRank = rank; }
Int_t KBGlobal::GetRank() { return fRank; }
