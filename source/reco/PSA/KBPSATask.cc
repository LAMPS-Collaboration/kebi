#include "KBRun.hh"
#include "KBPSATask.hh"
#include "KBChannelHit.hh"
#include "KBHit.hh"

#include <vector>
#include <iostream>
using namespace std;

ClassImp(KBPSATask)

KBPSATask::KBPSATask()
:KBTask("KBPSATask","")
{
}

bool KBPSATask::Init()
{
  KBRun *run = KBRun::GetRun();

  auto par = run -> GetParameterContainer();
  fTpc = (KBTpc *) run -> GetDetector();

  fNPlanes = fTpc -> GetNPlanes();
  par -> GetParDouble("gasDriftVelocity", fDriftVelocity);
  par -> GetParDouble("tbTime", fTbTime);

  fPadArray = (TClonesArray *) run -> GetBranch("Pad");

  for (auto iPlane = 0; iPlane < fNPlanes; iPlane++)
    fPadPlane[iPlane] = fTpc -> GetPadPlane(iPlane);

  fHitArray = new TClonesArray("KBHit", 5000);
  run -> RegisterBranch("Hit", fHitArray, true);

  if (fPSA == nullptr)
    fPSA = new KBPSA();
  fPSA -> SetParameters(par);

  return true;
}

void KBPSATask::Exec(Option_t*)
{
  fHitArray -> Delete();

  Int_t nPads = fPadArray -> GetEntriesFast();

  Int_t idx = 0;

  for (auto iPad = 0; iPad < nPads; iPad++) {
    auto pad = (KBPad *) fPadArray -> At(iPad);
    Double_t kPlane = fPadPlane[pad->GetPlaneID()] -> GetPlaneK();

    auto bufferOut = pad -> GetBufferOut();

    vector<KBChannelHit> hitArray;
    fPSA -> AnalyzeChannel(bufferOut, &hitArray);

    for (auto channelHit : hitArray) {

      ///@todo build pad plane dependent code
      Double_t k;
      if (pad -> GetPlaneID() == 0)
        k = kPlane - (channelHit.GetTDC()+0.5)*fTbTime*fDriftVelocity;
      else
        k = kPlane + (channelHit.GetTDC()+0.5)*fTbTime*fDriftVelocity;

      Double_t x,y,z;
      fTpc -> IJKToXYZ(pad->GetI(),pad->GetJ(),k,x,y,z);

      auto hit = (KBHit *) fHitArray -> ConstructedAt(idx);
      hit -> SetHitID(idx);
      hit -> SetPadID(pad -> GetPadID());
      hit -> SetX(x);
      hit -> SetY(y);
      hit -> SetZ(z);
      hit -> SetTb(channelHit.GetTDC());
      hit -> SetCharge(channelHit.GetADC());
      hit -> SetSection(pad -> GetSection());
      hit -> SetRow(pad -> GetRow());
      hit -> SetLayer(pad -> GetLayer());
      hit -> Change();

      idx++;
    }
  }

  cout << "  [" << this -> GetName() << "] Number of found hits: " << idx << endl;
}

void KBPSATask::SetPSA(KBPSA *psa)
{
  fPSA = psa;
}
