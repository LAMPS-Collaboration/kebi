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
  auto tpc = (KBTpc *) run -> GetDetector();

  fNPlanes = tpc -> GetNPlanes();
  par -> GetParInt("tbStart", fTbStart);
  par -> GetParInt("nTbs", fNTbs);
  par -> GetParDouble("gasDriftVelocity", fDriftVelocity);
  par -> GetParDouble("tbTime", fTbTime);
  par -> GetParDouble("ADCThreshold", fADCThreshold);

  fPadArray = (TClonesArray *) run -> GetBranch("Pad");

  for (auto iPlane = 0; iPlane < fNPlanes; iPlane++)
    fPadPlane[iPlane] = tpc -> GetPadPlane(iPlane);

  fHitArray = new TClonesArray("KBHit", 5000);
  run -> RegisterBranch("Hit", fHitArray, true);

  fPSA = new KBPSA();
  fPSA -> SetTbRange(fTbStart, fTbStart + fNTbs - 1);
  fPSA -> SetThreshold(fADCThreshold);

  return true;
}

void KBPSATask::Exec(Option_t*)
{
  fHitArray -> Delete();

  Int_t nPads = fPadArray -> GetEntriesFast();

  Int_t idx = 0;

  for (auto iPad = 0; iPad < nPads; iPad++) {
    auto pad = (KBPad *) fPadArray -> At(iPad);
    Double_t zPlane = fPadPlane[pad->GetPlaneID()] -> GetPlaneK();

    auto bufferOut = pad -> GetBufferOut();

    vector<KBChannelHit> hitArray;
    fPSA -> AnalyzeChannel(bufferOut, &hitArray);

    for (auto channelHit : hitArray) {

      ///@todo build pad plane dependent code
      Double_t z;
      if (pad -> GetPlaneID() == 0)
        z = zPlane - (channelHit.GetTDC()+0.5)*fTbTime*fDriftVelocity;
      else
        z = zPlane + (channelHit.GetTDC()+0.5)*fTbTime*fDriftVelocity;

      auto hit = (KBHit *) fHitArray -> ConstructedAt(idx);
      hit -> SetHitID(idx);
      hit -> SetPadID(pad -> GetPadID());
      hit -> SetX(pad -> GetI());
      hit -> SetY(z);
      hit -> SetZ(pad -> GetJ());
      hit -> SetTb(channelHit.GetTDC());
      hit -> SetCharge(channelHit.GetADC());
      hit -> SetSection(pad -> GetSection());
      hit -> SetRow(pad -> GetRow());
      hit -> SetLayer(pad -> GetLayer());

      idx++;
    }
  }

  cout << "  [" << this -> GetName() << "] Number of found hits: " << idx << endl;
}
