#ifndef KBHELIXTRACK_HH
#define KBHELIXTRACK_HH

#include "TString.h"

#include "KBHit.hh"
#include "KBTracklet.hh"
#include "KBVector3.hh"
#include "KBHelixTrackFitter.hh"
#include <vector>

/**
 * SpiRIT Helix Track Container.
 *
 * All units in [mm], [ADC], [radian], [MeV].
 */

class KBHelixTrack : public KBTracklet
{
  public:
    KBHelixTrack();
    KBHelixTrack(Int_t id);
    virtual ~KBHelixTrack() {};

    enum KBFitStatus { kBad, kLine, kPlane, kHelix, kGenfitTrack };

  private:
    KBFitStatus fFitStatus;  ///< One of kBad, kHelix and kLine.

    Double_t fIHelixCenter;  ///< i-component of the helix center
    Double_t fJHelixCenter;  ///< j-component of the helix center
    Double_t fHelixRadius;   ///< Radius of the helix
    Double_t fKInitial;      ///< k-position at angle alpha = 0
    Double_t fAlphaSlope;    ///< k = fAlphaSlope * alpha + fKInitial

    Double_t fChargeSum;     ///< Sum of charge

    Double_t fExpectationI;  //! < Expectation value of i
    Double_t fExpectationK;  //! < Expectation value of j
    Double_t fExpectationJ;  //! < Expectation value of k
    Double_t fExpectationII; //! < Expectation value of i^2
    Double_t fExpectationJJ; //! < Expectation value of j^2
    Double_t fExpectationKK; //! < Expectation value of k^2
    Double_t fExpectationIJ; //! < Expectation value of ij
    Double_t fExpectationJK; //! < Expectation value of jk
    Double_t fExpectationKI; //! < Expectation value of ki

    Double_t fRMSW;          ///< width RMS of the fit
    Double_t fRMSH;          ///< height RMS of the fit

    Double_t fAlphaHead;     ///< Head position alpha
    Double_t fAlphaTail;     ///< Last position alpha

    Bool_t fIsPositiveChargeParticle;

    std::vector<KBHit *> fMainHits; //!
    std::vector<KBHit *> fCandHits; //!

    std::vector<Int_t> fMainHitIDs;    ///<
    std::vector<Double_t> fdEdxArray;  ///< dE/dx array;

    Int_t    fGenfitID;        ///< GENFIT Track ID
    Double_t fGenfitMomentum;  ///< Momentum reconstructed by GENFIT

  public:
    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option="") const;
    virtual void Copy (TObject &object) const;

    virtual bool Fit();

    KBTrackFitter *CreateTrackFitter() const;

    void AddHit(KBHit *hit);
    void RemoveHit(KBHit *hit);
    void DeleteHits();
    void SortHits(bool increasing = true);
    void SortHitsByTimeOrder();

    void FinalizeHits();

    void SetGenfitID(Int_t idx);

    void SetFitStatus(KBFitStatus value);
    void SetIsBad();
    void SetIsLine();
    void SetIsPlane();
    void SetIsHelix();
    void SetIsGenfitTrack();

    void SetLineDirection(TVector3 dir);  ///< ONLY USED IN TRACK FINDING
    void SetPlaneNormal(TVector3 norm);   ///< ONLY USED IN TRACK FINDING

    void SetHelixCenter(Double_t i, Double_t j);
    void SetHelixRadius(Double_t r);
    void SetKInitial(Double_t k);
    void SetAlphaSlope(Double_t s);

    void SetRMSW(Double_t rms);
    void SetRMSH(Double_t rms);
    void SetAlphaHead(Double_t alpha);
    void SetAlphaTail(Double_t alpha);

    void DetermineParticleCharge(TVector3 vertex);
    void SetIsPositiveChargeParticle(Bool_t val);

    void SetGenfitMomentum(Double_t p);

    Int_t GetGenfitID() const;

    KBFitStatus GetFitStatus() const;
    TString GetFitStatusString() const;
    bool IsBad() const;
    bool IsLine() const;
    bool IsPlane() const;
    bool IsHelix() const;
    bool IsGenfitTrack() const;

    Double_t GetHelixCenterJ() const;
    Double_t GetHelixCenterI() const;
    Double_t GetHelixRadius() const;
    Double_t GetKInitial() const;
    Double_t GetAlphaSlope() const;

    void GetHelixParameters(Double_t &iCenter, 
                            Double_t &jCenter, 
                            Double_t &radius, 
                            Double_t &dipAngle,
                            Double_t &kInitial,
                            Double_t &alphaSlope) const;

    Double_t GetChargeSum() const;

    KBVector3 GetMean() const;
    Double_t GetIMean() const;
    Double_t GetJMean() const;
    Double_t GetKMean() const;
    Double_t GetICov() const;
    Double_t GetJCov() const;

    Double_t CovWII() const; ///< SUM_i {(z_centroid-z_i)*(z_centroid-z_i) }
    Double_t CovWJJ() const; ///< SUM_i {(x_centroid-x_i)*(x_centroid-x_i) }
    Double_t CovWKK() const; ///< SUM_i {(y_centroid-y_i)*(y_centroid-y_i) }

    Double_t CovWIJ() const; ///< SUM_i {(z_centroid-z_i)*(x_centroid-x_i) }
    Double_t CovWJK() const; ///< SUM_i {(x_centroid-x_i)*(y_centroid-y_i) }
    Double_t CovWKI() const; ///< SUM_i {(y_centroid-y_i)*(z_centroid-z_i) }

    Double_t GetExpectationI()  const;
    Double_t GetExpectationJ()  const;
    Double_t GetExpectationK()  const;

    Double_t GetExpectationII() const;
    Double_t GetExpectationJJ() const;
    Double_t GetExpectationKK() const;

    Double_t GetExpectationIJ() const;
    Double_t GetExpectationJK() const;
    Double_t GetExpectationKI() const;

    Double_t GetRMSW() const;
    Double_t GetRMSH() const;
    Double_t GetAlphaHead() const;
    Double_t GetAlphaTail() const;

    Bool_t IsPositiveChargeParticle() const;



    Int_t GetNumHits() const;
    KBHit *GetHit(Int_t idx) const;
    std::vector<KBHit *> *GetHitArray();

    Int_t GetNumCandHits() const;
    std::vector<KBHit *> *GetCandHitArray();



    Int_t GetNumHitIDs() const;
    Int_t GetHitID(Int_t idx) const;
    std::vector<Int_t> *GetHitIDArray();

    std::vector<Double_t> *GetdEdxArray();
    Double_t GetdEdxWithCut(Double_t lowR, Double_t highR) const;



    KBVector3 GetLineDirection() const;     ///< ONLY USED IN TRACK FINDING
    KBVector3 GetPlaneNormal() const;       ///< ONLY USED IN TRACK FINDING

    KBVector3 PerpLine(TVector3 p) const;   ///< ONLY USED IN TRACK FINDING
    KBVector3 PerpPlane(TVector3 p) const;  ///< ONLY USED IN TRACK FINDING

    Double_t GetGenfitMomentum() const;    /// Momentum reconstructed by genfit (if is set)
    /**
     * Angle between p and pt. Value becomes 0 on xz plane.
     * = tan(-fAlphaSlope/fHelixRadius) + pi/2
    */
    Double_t DipAngle() const;

    /** [Distance from point to center of helix in xz-plane] - [Helix radius] */
    Double_t DistCircle(TVector3 pointGiven) const;

       Int_t Charge() const;                        ///< Assumed charge of the track
       Int_t Helicity() const;                      ///< Helicity of track +/-
    Double_t LengthInPeriod() const;                ///< Length of track in one period
    Double_t KLengthInPeriod() const;               ///< y-length of track in one period
    Double_t LengthByAlpha(Double_t alpha) const;   ///< Length of track in change of alpha
    Double_t AlphaByLength(Double_t length) const;  ///< Angle alpha in change of length
    TVector3 PositionByAlpha(Double_t alpha) const; ///< Position at angle alpha [mm]
    TVector3 Direction(Double_t alpha) const;       ///< Direction at angle alpha

    /**
     * Extrapolate track due to alpha angle of the given point.
     * Returns alpha;
     */
    Double_t AlphaAtPosition(TVector3 p) const;

    /**
     * Extrapolate track due to given alpha.
     * Returns extrapolation length from the initial track reference position.
    */
    Double_t ExtrapolateToAlpha(Double_t alpha) const;

    /**
     * Extrapolate track due to given alpha.
     * Returns extrapolation length from the initial track reference position.
     *
     * @param alpha         given alpha.
     * @param pointOnHelix  extrapolated position on the helix.
    */
    Double_t ExtrapolateToAlpha(Double_t alpha, TVector3 &pointOnHelix) const;

    /**
     * Extrapolate track due to alpha angle of the given point.
     * Returns extrapolation length from the initial track reference position.
     *
     * @param pointGiven    given point.
     * @param pointOnHelix  extrapolated position on the helix.
     * @param alpha         extrapolated alpha angle.
    */
    Double_t ExtrapolateToPointAlpha(TVector3 pointGiven, TVector3 &pointOnHelix, Double_t &alpha) const;

    /**
     * Extrapolate track due to y-position of the given point.
     * Returns extrapolation length from the initial track reference position.
     *
     * @param pointGiven    given point.
     * @param pointOnHelix  extrapolated position on the helix.
     * @param alpha         extrapolated alpha angle.
    */
    Double_t ExtrapolateToPointK(TVector3 pointGiven, TVector3 &pointOnHelix, Double_t &alpha) const;

    /** Check feasibility of Extrapolation of track to zy-plane at x-position. */
    bool CheckExtrapolateToI(Double_t x) const;

    /** Check feasibility of Extrapolation of track to xy-plane at z-position. */
    bool CheckExtrapolateToJ(Double_t z) const;

    /**
     * Extrapolate track to zy-plane at x-position.
     * Returns true if extrapolation is possible, false if not.
     *
     * @param x              x-position of zy-plane.
     * @param pointOnHelix1  extrapolated position on the helix close to fAlphaHead
     * @param alpha1         extrapolated alpha angle close to fAlphaHead
     * @param pointOnHelix2  extrapolated position on the helix close to fAlphaTail
     * @param alpha2         extrapolated alpha angle close to fAlphaTail
    */
    bool ExtrapolateToI(Double_t x, 
        TVector3 &pointOnHelix1, Double_t &alpha1,
        TVector3 &pointOnHelix2, Double_t &alpha2) const;

    /**
     * Extrapolate track to xy-plane at z-position.
     * Returns true if extrapolation is possible, false if not.
     *
     * @param z              z-position of xy-plane.
     * @param pointOnHelix1  extrapolated position on the helix close to fAlphaHead
     * @param alpha1         extrapolated alpha angle close to fAlphaHead
     * @param pointOnHelix2  extrapolated position on the helix close to fAlphaTail
     * @param alpha2         extrapolated alpha angle close to fAlphaTail
    */
    bool ExtrapolateToJ(Double_t z,
        TVector3 &pointOnHelix1, Double_t &alpha1,
        TVector3 &pointOnHelix2, Double_t &alpha2) const;

    /**
     * Extrapolate track to zy-plane at x-position.
     * Returns true if extrapolation is possible, false if not.
     *
     * @param x              x-position of zy-plane.
     * @param alphaRef       reference position for extapolation
     * @param pointOnHelix   extrapolated position on the helix close to alphaRef
    */
    bool ExtrapolateToI(Double_t x, Double_t alphaRef, TVector3 &pointOnHelix) const;

    /**
     * Extrapolate track to xy-plane at z-position.
     * Returns true if extrapolation is possible, false if not.
     *
     * @param z              z-position of xy-plane
     * @param alphaRef       reference position for extapolation
     * @param pointOnHelix   extrapolated position on the helix close to alphaRef
    */
    bool ExtrapolateToJ(Double_t z, Double_t alphaRef, TVector3 &pointOnHelix) const;

    /**
     * Extrapolate track to xy-plane at z-position.
     * Returns true if extrapolation is possible, false if not.
     *
     * @param z             z-position of xy-plane.
     * @param pointOnHelix  extrapolated position close to tracjectory.
    */
    bool ExtrapolateToJ(Double_t z, TVector3 &pointOnHelix) const;

    /**
     * Interpolate between head and tail of helix.
     * Return interpolated position using ratio
     * Interpolate when 0 < r < 1.
     * Extrapolate when r < 0 or r > 1.
    */
    TVector3 InterpolateByRatio(Double_t r) const;

    /**
     * Interpolate between head and tail of helix.
     * Return interpolated position using length
     * Interpolate when 0 < length < TrackLength().
     * Extrapolate when length < 0 or length > TrackLength().
    */
    TVector3 InterpolateByLength(Double_t r) const;

    /**
     * Map and return position.
     * Concept of this mapping is to strecth helix to straight line.
     * - 1st axis : radial axis
     * - 2nd axis : normal to 1st and 3rd axis
     * - 3nd axis : length along helix line
    */
    TVector3 Map(TVector3 p) const;

    /**
     * Extrapolate to cloesest position from p to helix by Mapping.
     * Return length
     * @param p  input position
     * @param q  position on helix.
     * @param m  mapped position
    */
    Double_t ExtrapolateByMap(TVector3 p, TVector3 &q, TVector3 &m) const;

    /**
     * Check continuity of the track. Hit array must be filled.
     * Returns ratio of the continuous region. (-1 if less than 2 hits)
     *
     * If difference between "length from hits" is smaller than 20,
     * (> pad-diagonal, < 2*pad-row) the region is considered to be connected.
     *
     * The total length calculation is different from TrackLength(),
     * because TrackLength() use only the head/tail alpha angle informatiotn
     * but this method calculate length directly from the hit using
     * Map(...) method. 
     *
     * @param continuousLength  calculated length of continuos region.
     * @param totalLength       calculated total length of the track.
    */
    Double_t Continuity(Double_t &totalLength, Double_t &continuousLength);

    /** Same as Continuity(Double_t&, Double_t&) but no length outputs. */
    Double_t Continuity();

    virtual TVector3 Momentum(Double_t B = 0.5) const;       ///< Momentum of track (B = 0.5 by default)
    virtual TVector3 PositionAtHead() const;                 ///< Position at head of helix
    virtual TVector3 PositionAtTail() const;                 ///< Position at tail of helix
    virtual Double_t TrackLength() const;                    ///< Length of track calculated from head to tail.
    virtual TVector3 ExtrapolateTo(TVector3 point) const;    ///< Extrapolate to POCA from point, returns extrapolated position
    virtual TVector3 ExtrapolateHead(Double_t length) const; ///< Extrapolate head of track about length, returns extrapolated position
    virtual TVector3 ExtrapolateTail(Double_t length) const; ///< Extrapolate tail of track about length, returns extrapolated position
    virtual TVector3 ExtrapolateByRatio(Double_t r) const;   ///< Extrapolate by ratio (tail:0, head:1), returns extrapolated position
    virtual TVector3 ExtrapolateByLength(Double_t l) const;  ///< Extrapolate by length (tail:0), returns extrapolated position
    virtual Double_t LengthAt(TVector3 point) const;         ///< Length at POCA from point, where tail=0, head=TrackLength

  ClassDef(KBHelixTrack, 1)
};

class KBHitSortByIncreasingLength {
  public:
    KBHitSortByIncreasingLength(KBHelixTrack *track):fTrk(track) {}
    bool operator() (KBHit* a, KBHit* b) { return fTrk->Map(a->GetPosition()).Z() < fTrk->Map(b->GetPosition()).Z(); }
  private:
    KBHelixTrack *fTrk;
};

class KBHitSortByDecreasingLength {
  public:
    KBHitSortByDecreasingLength(KBHelixTrack *track):fTrk(track) {}
    bool operator() (KBHit* a, KBHit* b) { return fTrk->Map(a->GetPosition()).Z() > fTrk->Map(b->GetPosition()).Z(); }
  private:
    KBHelixTrack *fTrk;
};

#endif
