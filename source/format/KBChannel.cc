#include "KBChannel.hh"

ClassImp(KBChannel)

void KBChannel::SetID(Int_t id) { fID = id; }
Int_t KBChannel::GetID() const { return fID; }

void KBChannel::AddChannelHit(KBChannelHit *channelHit) { fChannelHitArray.Add(channelHit); }
TObjArray *KBChannel::GetChannelHitArray() { return &fChannelHitArray; }
