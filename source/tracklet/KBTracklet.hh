#ifndef KBTRACKLET_HH
#define KBTRACKLET_HH

#include "TObject.h" 

#include "KBHitList.hh"
#include "KBHit.hh"
#include "KBTrackFitter.hh"

class KBTrackFitter;

class KBTracklet : public TObject
{
  protected:
    Int_t fTrackID;
    Int_t fParentID;  ///< Vertex ID

    KBHitList *fHitList = nullptr;  //!

  public:
    KBTracklet() {}
    virtual ~KBTracklet() {}

    void SetTrackID(Int_t val) { fTrackID = val; }
    Int_t GetTrackID() { return fTrackID; }

    void SetParentID(Int_t val) { fParentID = val; }
    Int_t GetParentID() { return fParentID; }

    void SetHitList(KBHitList *list) { fHitList = list; }
    KBHitList *GetHitList() { return fHitList; }

    virtual void AddHit(KBHit *hit);
    virtual void RemoveHit(KBHit *hit);

    virtual KBTrackFitter *CreateTrackFitter() const = 0;

    virtual TVector3 Momentum(Double_t B = 0.5) const = 0; ///< Momentum of track at head.
    virtual TVector3 PositionAtHead() const = 0; ///< Position at head of helix
    virtual TVector3 PositionAtTail() const = 0; ///< Position at tail of helix
    virtual Double_t TrackLength() const = 0; ///< Length of track calculated from head to tail.

    virtual TVector3 ExtrapolateTo(TVector3 point) const = 0; ///< Extrapolate to POCA from point, returns extrapolated position
    virtual TVector3 ExtrapolateHead(Double_t l) const = 0; ///< Extrapolate head of track about length, returns extrapolated position
    virtual TVector3 ExtrapolateTail(Double_t l) const = 0; ///< Extrapolate tail of track about length, returns extrapolated position
    virtual TVector3 ExtrapolateByRatio(Double_t r) const = 0; ///< Extrapolate by ratio (tail:0, head:1), returns extrapolated position
    virtual TVector3 ExtrapolateByLength(Double_t l) const = 0; ///< Extrapolate by length (tail:0), returns extrapolated position

    virtual Double_t LengthAt(TVector3 point) const = 0; ///< Length at POCA from point, where tail=0, head=TrackLength

  ClassDef(KBTracklet, 1)
};

#endif
