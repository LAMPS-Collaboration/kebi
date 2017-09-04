#include "KBChannelBufferX.hh"

ClassImp(KBChannelBufferX)

void KBChannelBufferX::Draw(Option_t *option)
{
  auto hist = GetHist();
  hist -> Draw(option);

  //TODO : draw hits
}
