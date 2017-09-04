#include "KBChannelBufferD.hh"

ClassImp(KBChannelBufferD)

TH1 *KBChannelBufferD::GetHist(bool update)
{
  if (fHistogram == nullptr) {
    fHistogram = new TH1D(Form("Channel%d",fID),Form("Channel%d;Time-bucket;ADC",fID),fN,0,fN);
    update = false;
  }

  if (update) {
    for (auto iTb = 0; iTb < fN; ++iTb)
      fHistogram -> SetBinContent(iTb+1,fArray[iTb]);
  }

  return fHistogram;
}
