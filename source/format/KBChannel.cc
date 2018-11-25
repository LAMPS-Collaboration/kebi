#include "KBChannel.hh"

ClassImp(KBChannel)

void KBChannel::Clear(Option_t *option)
{
  TObject::Clear(option);

  fID = -1;
  fPos1 = TVector3(-999,-999,-999);
  fPos2 = TVector3(-999,-999,-999);
}

void KBChannel::SetID(Int_t id) { fID = id; }
Int_t KBChannel::GetID() const { return fID; }

void KBChannel::AddChannelHit(KBChannelHit *channelHit) { fChannelHitArray.Add(channelHit); }
TObjArray *KBChannel::GetChannelHitArray() { return &fChannelHitArray; }

void KBChannel::SetPos1(TVector3 pos) { fPos1 = pos; }
TVector3 KBChannel::GetPos1() const { return fPos1; }

void KBChannel::SetPos2(TVector3 geo) { fPos2 = geo; }
TVector3 KBChannel::GetPos2() const { return fPos2; }
