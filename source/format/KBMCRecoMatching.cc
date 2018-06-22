#include "KBMCRecoMatching.hh"
#include <iostream>
using namespace std;

ClassImp(KBMCRecoMatching)

KBMCRecoMatching::KBMCRecoMatching()
: fStatus(KBMCRecoMatching::kNotFound), fMCID(-1), fMCMomentum(TVector3(0,0,0)), fRecoID(-1), fRecoMomentum(TVector3(0,0,0))
{
  Clear();
}

void KBMCRecoMatching::Clear(Option_t *)
{
  TObject::Clear();

  fStatus = KBMCRecoMatching::kNotFound;

  fMCID = -1;
  fMCMomentum = TVector3();

  fRecoID = -1;
  fRecoMomentum = TVector3();
}

void KBMCRecoMatching::Print(Option_t *) const
{
  if (fStatus == KBMCRecoMatching::kNotFound)
    cout << "[Not-Found] MC_ID:" << fMCID << " MC_p(" << fMCMomentum.X() << " " << fMCMomentum.Y() << " " << fMCMomentum.Z() << ")" << endl;
  else if (fStatus == KBMCRecoMatching::kFake)
    cout << "[Fake] Reco_ID:" << fRecoID << " Reco_p(" << fRecoMomentum.X() << " " << fRecoMomentum.Y() << " " << fRecoMomentum.Z() << ")" << endl;
  else if (fStatus == KBMCRecoMatching::kMatched) {
    cout << "[MATCH] MC_ID:" << fMCID     << " MC_p(" << fMCMomentum.X()   << " " << fMCMomentum.Y()   << " " << fMCMomentum.Z()   << ")" << endl;
    cout << "      Reco_ID:" << fRecoID << " Reco_p(" << fRecoMomentum.X() << " " << fRecoMomentum.Y() << " " << fRecoMomentum.Z() << ")" << endl;
  }
}

void KBMCRecoMatching::Copy(TObject &obj) const
{
  TObject::Copy(obj);
  auto m = (KBMCRecoMatching &) obj;

  m.Set(fMCID,fMCMomentum,fRecoID,fRecoMomentum);

       if (fStatus == KBMCRecoMatching::kNotFound) m.IsNotFound();
  else if (fStatus == KBMCRecoMatching::kFake)     m.IsFake();
  else if (fStatus == KBMCRecoMatching::kMatched)  m.IsMatched();
}

void KBMCRecoMatching::Set(Int_t mcid, TVector3 mcp, Int_t recoid, TVector3 recop)
{
  fMCID = mcid;
  fRecoID = recoid;
  fMCMomentum = mcp;
  fRecoMomentum = recop;
}

void KBMCRecoMatching::SetMCID(Int_t val)          { fMCID = val; }
void KBMCRecoMatching::SetRecoID(Int_t val)        { fRecoID = val; }
void KBMCRecoMatching::SetMCMomentum(TVector3 p)   { fMCMomentum = p; }
void KBMCRecoMatching::SetRecoMomentum(TVector3 p) { fRecoMomentum = p; }

void KBMCRecoMatching::SetIsMatched()  { fStatus = kMatched; }
void KBMCRecoMatching::SetIsNotFound() { fStatus = kNotFound; }
void KBMCRecoMatching::SetIsFake()     { fStatus = kFake; }
void KBMCRecoMatching::SetStatus(Status val) { fStatus = val; }

Int_t KBMCRecoMatching::GetMCID()            { return fMCID; }
Int_t KBMCRecoMatching::GetRecoID()          { return fRecoID; }
TVector3 KBMCRecoMatching::GetMCMomentum()   { return fMCMomentum; }
TVector3 KBMCRecoMatching::GetRecoMomentum() { return fRecoMomentum; }

bool KBMCRecoMatching::IsMatched()  { return fStatus == kMatched    ? true : false; }
bool KBMCRecoMatching::IsNotFound() { return fStatus == kNotFound ? true : false; }
bool KBMCRecoMatching::IsFake()     { return fStatus == kFake     ? true : false; }
KBMCRecoMatching::Status KBMCRecoMatching::GetStatus() { return fStatus; }
