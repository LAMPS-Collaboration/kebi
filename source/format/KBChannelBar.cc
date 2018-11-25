#include "KBChannelBar.hh"

ClassImp(KBChannelBar)

void KBChannelBar::Clear(Option_t *option)
{
  KBChannelBufferD::Clear(option);

  fLayer = -1;
  fPairID = -1;

  fSignalAxis = KBVector3::kNon;
}

void KBChannelBar::Copy(TObject &obj) const
{
  TObject::Copy(obj);
  auto channel = (KBChannelBar &) obj;

  channel.SetID(fID);
  channel.SetPos1(fPos1);
  channel.SetPos2(fPos2);

  channel.Set(fN);
  for (auto iTb = 0; iTb < fN; ++iTb)
    channel.SetAt(iTb, fArray[iTb]);
}

bool KBChannelBar::IsVertical()   const { return (fSignalAxis==KBVector3::kY||fSignalAxis==KBVector3::kMY)?true:false; }
bool KBChannelBar::IsHorizontal() const { return (fSignalAxis==KBVector3::kX||fSignalAxis==KBVector3::kMX)?true:false; }

Double_t KBChannelBar::GetTb(TVector3 posIn) const
{
  auto tIn = KBVector3(posIn).At(fSignalAxis);

  auto tFace = KBVector3(fPos1).At(fSignalAxis);
  if (tIn > tFace)
    return -1;

  auto tTail = KBVector3(fPos2).At(fSignalAxis);
  if (tIn < tTail)
    return -1;

  return fN*(tFace-tIn)/(tFace-tTail);
}

Double_t KBChannelBar::GetTb(Double_t x) const
{
  KBVector3 posIn;
  posIn.AddAt(x,fSignalAxis,true);
  return GetTb(posIn);
}
