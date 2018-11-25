#include "KBChannelBufferS.hh"

ClassImp(KBChannelBufferS)

void KBChannelBufferS::Clear(Option_t *option)
{
  KBChannelBufferX::Clear(option);

  for (auto iTb = 0; iTb < fN; ++iTb)
    fArray[iTb] = 0;
}

void KBChannelBufferS::Copy(TObject &obj) const
{
  TObject::Copy(obj);
  auto channel = (KBChannelBufferS &) obj;

  channel.SetID(fID);
  channel.SetPos1(fPos1);
  channel.SetPos2(fPos2);

  channel.Set(fN);
  for (auto iTb = 0; iTb < fN; ++iTb)
    channel.SetAt(iTb, fArray[iTb]);
}

TH1 *KBChannelBufferS::GetHist(TString name)
{
  if (name.IsNull())
    name = Form("Channel%d",fID);

  auto hist = new TH1S(name,Form("Channel%d;Time-bucket;ADC",fID),fN,0,fN);
  for (auto iTb = 0; iTb < fN; ++iTb)
    hist -> SetBinContent(iTb+1,fArray[iTb]);

  return hist;
}
