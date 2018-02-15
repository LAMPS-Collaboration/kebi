#ifndef KBWPOINTCLUSTER_HH
#define KBWPOINTCLUSTER_HH

#include "KBWPoint.hh"
#include <vector>
#include <cmath>

/* Cluster of TWPoint.
 * Adding TWPoint to KBWPointCluster will calculate mean, covariance.
 * MeanError(i,j) returns covariance/weight_sum.
 */

class KBWPointCluster : public KBWPoint
{
  public:
    KBWPointCluster();
    virtual ~KBWPointCluster();

    virtual void Print(Option_t *option = "") const;
    virtual void Clear(Option_t *option = "");

    void Add(KBWPoint wp);

    Double_t StandardDeviation(Int_t i)   { return sqrt(fCov[i][i]); }
    Double_t Covariance(Int_t i, Int_t j) { return fCov[i][j]; }
    Double_t  MeanError(Int_t i, Int_t j) { return fCov[i][j]/fW; }

  private:
    Double_t fCov[3][3];

  ClassDef(KBWPointCluster, 1)
};

#endif
