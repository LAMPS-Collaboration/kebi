#ifndef KBCHANNELBUFFERD_HH
#define KBCHANNELBUFFERD_HH

#include "KBChannelBufferX.hh"
#include "TArrayD.h"
#include "TH1D.h"

class KBChannelBufferD : public KBChannelBufferX, public TArrayD
{
  public:
    KBChannelBufferD() {};
    virtual ~KBChannelBufferD() {}

    TH1 *GetHist(bool update = true);

  ClassDef(KBChannelBufferD, 1)
};

#endif
