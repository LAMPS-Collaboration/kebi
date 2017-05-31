#ifndef KBLINEARTRACK_HH
#define KBLINEARTRACK_HH

#include "KBTracklet.hh"
#include "KBGeoLine.hh"

class KBLinearTrack : public KBTracklet, public KBGeoLine
{
  public:
    KBLinearTrack();
    KBLinearTrack(Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2);
    KBLinearTrack(TVector3 pos1, TVector3 pos2);
    virtual ~KBLinearTrack() {};

    void SetTrack(TVector3 pos1, TVector3 pos2);

    virtual KBTrackFitter *CreateTrackFitter() const;

    virtual TVector3 Momentum(Double_t B = 0.5) const;
    virtual TVector3 PositionAtHead() const;
    virtual TVector3 PositionAtTail() const;
    virtual Double_t TrackLength() const;

    virtual TVector3 ExtrapolateTo(TVector3 point) const;
    virtual TVector3 ExtrapolateHead(Double_t l) const;
    virtual TVector3 ExtrapolateTail(Double_t l) const;
    virtual TVector3 ExtrapolateByRatio(Double_t r) const;
    virtual TVector3 ExtrapolateByLength(Double_t l) const;

    virtual Double_t LengthAt(TVector3 point) const;

  ClassDef(KBLinearTrack, 1)
};

#endif
