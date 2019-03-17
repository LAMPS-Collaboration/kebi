#include "KBGear.hh"

void KBGear::CreateParameterContainer() { fPar = new KBParameterContainer(); }
void KBGear::SetParameterContainer(KBParameterContainer *par) { fPar = par; }
void KBGear::SetParameterContainer(TString file) { fPar = new KBParameterContainer(file); }
void KBGear::AddParameterFile(TString file)
{
  if (fPar == nullptr)
    fPar = new KBParameterContainer();
  fPar -> AddFile(file);
}

KBParameterContainer *KBGear::GetParameterContainer() { return fPar; }

void KBGear::SetRank(Int_t rank) { fRank = rank; }
Int_t KBGear::GetRank() { return fRank; }
