#ifndef KBCHANNELBAR_HH
#define KBCHANNELBAR_HH

#include "KBChannelBufferD.hh"
#include "TVector3.h"
#include "KBVector3.hh"

class KBChannelBar : public KBChannelBufferD
{
  public:
    KBChannelBar() { Clear(); }
    virtual ~KBChannelBar() {}

    virtual void Clear(Option_t *option = "");
    virtual void Copy (TObject &object) const;

    void SetPosFace(TVector3 pos) { SetPos1(pos); } // Set position at the face of signal (t = 0)
    void SetPosTail(TVector3 pos) { SetPos2(pos); } // Set position at the tail of signal (t = 0)

    TVector3 GetPosFace() const { return GetPos1(); } // Get position at the face of signal (t = 0)
    TVector3 GetPosTail() const { return GetPos2(); } // Get position at the tail of signal (t = 0)

    void SetLayer(Int_t layer) { fLayer = layer; }
    void SetPairID(Int_t id) { fPairID = id; }

    Int_t GetLayer() const { return fLayer; }
    Int_t GetPairID() const { return fPairID; }

    void SetSignalAxis(KBVector3::Axis axis) { fSignalAxis = axis; }

    bool IsVertical() const;
    bool IsHorizontal() const;

    Double_t GetTb(TVector3 pos) const;
    Double_t GetTb(Double_t x) const;

  private:
    Int_t fLayer = -1;
    Int_t fPairID = -1;

    KBVector3::Axis fSignalAxis = KBVector3::kNon;

  ClassDef(KBChannelBar, 1)
};

#endif
