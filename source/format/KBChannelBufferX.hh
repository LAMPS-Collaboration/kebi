#ifndef KBCHANNELBUFFERX_HH
#define KBCHANNELBUFFERX_HH

#include "KBChannel.hh"
#include "TH1.h"

class KBChannelBufferX : public KBChannel
{
  public:
    KBChannelBufferX() {};
    virtual ~KBChannelBufferX() {}

    virtual void Draw(Option_t *option = "");

    virtual TH1 *GetHist(bool update = true) = 0;

  protected:
    TH1 *fHistogram = nullptr; //!

  ClassDef(KBChannelBufferX, 1)
};

#endif
