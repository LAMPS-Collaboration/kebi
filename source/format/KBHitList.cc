#include "KBHitList.hh"
#include "KBHit.hh"

ClassImp(KBHitList)

KBHitList::KBHitList()
{
  Clear();
}

void KBHitList::Clear(Option_t *option)
{
  TObject::Clear(option);

  fHitArray.clear();
  fHitIDArray.clear();

  fN = 0;
  fW = 0;

  fEX = 0;
  fEY = 0;
  fEZ = 0;
  fEXX = 0;
  fEYY = 0;
  fEZZ = 0;
  fEXY = 0;
  fEYZ = 0;
  fEZX = 0;

  if (fODRFitter != nullptr)
    fODRFitter -> Reset();
}

void KBHitList::Print(Option_t *option) const
{
  TString opts = TString(option);
  if (fHitArray.size()!=0) {
    for (auto hit : fHitArray)
      hit -> Print(option);
  }
  else {
    TString title;
    if (opts.Index(">")>=0) title += "> ";
    if (opts.Index("t")>=0) title += "Hit-IDs: ";

    kc_info << title;
    if (opts.Index("s")>=0)
      for (auto hitID : fHitIDArray)
        kc_raw << hitID << " ";
    else
      for (auto hitID : fHitIDArray)
        kc_raw << setw(4) << hitID;
    kc_raw << endl;
  }
}

KBGeoLine KBHitList::FitLine()
{
  if (fODRFitter == nullptr)
    fODRFitter = KBODRFitter::GetFitter();

  KBGeoLine line;
  line.SetRMS(-1);

  if (fN < 4)
    return line;

  fODRFitter -> Reset();
  fODRFitter -> SetCentroid(fEX,fEY,fEZ);
  fODRFitter -> SetMatrixA(GetCovWXX(),GetCovWYY(),GetCovWZZ(),GetCovWXY(),GetCovWYZ(),GetCovWZX());
  fODRFitter -> SetWeightSum(fW);
  fODRFitter -> SetNumPoints(fN);
  if (fODRFitter -> Solve() == false)
    return line;

  fODRFitter -> ChooseEigenValue(0);
  line.SetLine(TVector3(fEX,fEY,fEZ),fODRFitter -> GetDirection());
  line.SetRMS(fODRFitter -> GetRMSLine());

  return line;
}

KBGeoPlane KBHitList::FitPlane()
{
  if (fODRFitter == nullptr)
    fODRFitter = KBODRFitter::GetFitter();

  KBGeoPlane plane;
  plane.SetRMS(-1);

  if (fN < 4)
    return plane;

  fODRFitter -> Reset();
  fODRFitter -> SetCentroid(fEX,fEY,fEZ);
  fODRFitter -> SetMatrixA(GetCovWXX(),GetCovWYY(),GetCovWZZ(),GetCovWXY(),GetCovWYZ(),GetCovWZX());
  fODRFitter -> SetWeightSum(fW);
  fODRFitter -> SetNumPoints(fN);
  if (fODRFitter -> Solve() == false)
    return plane;

  fODRFitter -> ChooseEigenValue(2);
  plane.SetPlane(TVector3(fEX,fEY,fEZ),fODRFitter -> GetNormal());
  plane.SetRMS(fODRFitter -> GetRMSPlane());

  return plane;
}

KBGeoCircle KBHitList::FitCircle(kbaxis ref)
{
  if (fODRFitter == nullptr)
    fODRFitter = KBODRFitter::GetFitter();
  fODRFitter -> Reset();

  KBGeoCircle circle;
  circle.SetRMS(-1);

  if (fN < 4)
    return circle;

  auto mean = GetMean(ref);
  auto var = GetVar(ref);

  Double_t iMean = mean.I();
  Double_t jMean = mean.J();
  Double_t RSR = 2 * sqrt(var.I() + var.J()); // Riemann sphere radius

  // SGP for Stereo Graphic Projection

  Double_t iSGPMean = 0;
  Double_t jSGPMean = 0;
  Double_t kSGPMean = 0;
  Double_t iShifted = 0;
  Double_t jShifted = 0;

  for (auto hit : fHitArray)
  {
    KBVector3 pos(hit -> GetPosition(), ref);
    iShifted = pos.I() - iMean;
    jShifted = pos.J() - jMean;
    Double_t w = hit -> GetCharge();

    Double_t rEff = sqrt(iShifted*iShifted + jShifted*jShifted) / (2*RSR);
    Double_t denominator = 1 + rEff*rEff;
    Double_t iSGP = iShifted / denominator;
    Double_t jSGP = jShifted / denominator;
    Double_t kSGP = 2 * RSR * rEff * rEff / denominator;

    iSGPMean += w * iSGP;
    jSGPMean += w * jSGP;
    kSGPMean += w * kSGP;
  }

  iSGPMean = iSGPMean / fW;
  jSGPMean = jSGPMean / fW;
  kSGPMean = kSGPMean / fW;

  fODRFitter -> SetCentroid(iSGPMean, jSGPMean, kSGPMean);
  KBVector3 mapMean(ref, iSGPMean, jSGPMean, kSGPMean);

  for (auto hit : fHitArray)
  {
    KBVector3 pos(hit -> GetPosition(), ref);
    iShifted = pos.I() - iMean;
    jShifted = pos.J() - jMean;
    Double_t w = hit -> GetCharge();

    Double_t rEff = sqrt(iShifted*iShifted + jShifted*jShifted) / (2*RSR);
    Double_t denominator = 1 + rEff*rEff;
    Double_t iSGP = iShifted / denominator;
    Double_t jSGP = jShifted / denominator;
    Double_t kSGP = 2 * RSR * rEff * rEff / denominator;

    fODRFitter -> AddPoint(iSGP, jSGP, kSGP, w);
  }

  if (fODRFitter -> Solve() == false)
    return circle;

  fODRFitter -> ChooseEigenValue(0); KBVector3 lll(ref, fODRFitter -> GetDirection());
  fODRFitter -> ChooseEigenValue(1); KBVector3 mmm(ref, fODRFitter -> GetDirection());
  fODRFitter -> ChooseEigenValue(2); KBVector3 nnn(ref, fODRFitter -> GetDirection());

  nnn.SetK(abs(nnn.K()));
  KBVector3 RSC(ref,iMean,jMean,RSR); // Riemann Sphere Center
  auto kkk = nnn.I()*iSGPMean + nnn.J()*jSGPMean + nnn.K()*kSGPMean;
  auto ddd = abs(nnn.Dot(RSC) - kkk);
  if (nnn.Z()*RSR > kkk) //RSR is higher
    ddd = -ddd;

  KBVector3 uuu = 1./sqrt(lll.K()*lll.K() + mmm.K()*mmm.K()) * (lll.K()*lll + mmm.K()*mmm);
  KBVector3 vvv = 1./sqrt(uuu.I()*uuu.I() + uuu.J()*uuu.J()) * KBVector3(ref, uuu.I(), uuu.J(), 0);

  auto wp = ddd*nnn + sqrt(RSR*RSR - ddd*ddd)*uuu;
  auto wm = ddd*nnn - sqrt(RSR*RSR - ddd*ddd)*uuu;
  auto vp = wp.Dot(vvv);
  auto vm = wm.Dot(vvv);
  auto pp = TMath::ATan2(abs(vp),-wp.K());
  auto pm = TMath::ATan2(abs(vm),-wm.K());
  auto rp = RSR * (TMath::Tan(pp/2) + TMath::Tan(pm/2));
  auto rm = RSR * (TMath::Tan(pp/2) - TMath::Tan(pm/2));

  auto center = rm * vvv + RSC;
  circle.SetCircle(center.I(), center.J(), rp);

#ifdef DRAW_HELIX_FITTER
  auto FCC = RSC + ddd * nnn; // (riemann) Fit circle center
  auto dddk = RSR + ddd * nnn.K();
  auto vFCC = FCC.Dot(vvv);
  auto vuuu = uuu.Dot(vvv);
  auto vddd = ddd*(nnn.Dot(vvv));
  auto vcenter = iMean*vvv.I()+jMean*vvv.J();
  auto kcenter = RSR;
  auto FCR = sqrt(RSR*RSR - ddd*ddd);

  auto gc = KBGeoSphere(0,vcenter,kcenter,RSR);                             gc.GetCircleYZ()->Draw("al");
  auto ld = KBGeoLine(0,kcenter,0,vddd,dddk,0);                             ld.CreateArrowXY()->Draw("same>");
  auto m1 = new TMarker(vddd,dddk,20);                                      m1->Draw("samep");
  auto m2 = new TMarker(vFCC,FCC.Z(),25);                                   m2->Draw("samep");
  auto u1 = KBGeoLine(vddd, dddk, 0, vddd+FCR*vuuu, dddk+FCR*uuu.K(), 0);   u1.CreateArrowXY()->Draw("same>");
  auto u2 = KBGeoLine(vddd, dddk, 0, vddd-FCR*vuuu, dddk-FCR*uuu.K(), 0);   u2.CreateArrowXY()->Draw("same>");
  auto wp = KBGeoLine(vcenter, kcenter, 0, vcenter+ vp, kcenter+wp.K(), 0); wp.CreateArrowXY()->Draw("same|>");
  auto wm = KBGeoLine(vcenter, kcenter, 0, vcenter+ vm, kcenter+wm.K(), 0); wm.CreateArrowXY()->Draw("same|>");
#endif

  circle.SetRMS(1);
  return circle;
}

void KBHitList::AddHit(KBHit* hit)
{
  fHitArray.push_back(hit);
  fHitIDArray.push_back(hit->GetHitID());

  auto pos = hit->GetPosition();
  Double_t x = pos.X();
  Double_t y = pos.Y();
  Double_t z = pos.Z();
  Double_t w = hit -> GetCharge();

  Double_t wsum = fW + w;

  fEX  = (fW * fEX + w*x) / wsum;
  fEY  = (fW * fEY + w*y) / wsum;
  fEZ  = (fW * fEZ + w*z) / wsum;
  fEXX = (fW * fEXX + w*x*x) / wsum;
  fEYY = (fW * fEYY + w*y*y) / wsum;
  fEZZ = (fW * fEZZ + w*z*z) / wsum;
  fEXY = (fW * fEXY + w*x*y) / wsum;
  fEYZ = (fW * fEYZ + w*y*z) / wsum;
  fEZX = (fW * fEZX + w*z*x) / wsum;

  fW = wsum;
  ++fN;
}

void KBHitList::AddHit(Double_t x, Double_t y, Double_t z, Double_t q)
{
  AddHit(new KBHit(x,y,z,q));
}

bool KBHitList::RemoveHit(KBHit* hit)
{
  bool found = false;
  Int_t numHits = fHitArray.size();
  for (auto iHit = 0; iHit < numHits; iHit++) {
    if (fHitArray[iHit] == hit) {
      fHitArray.erase(fHitArray.begin()+iHit);
      fHitIDArray.erase(fHitIDArray.begin()+iHit);
      found = true;
      --fN;
      break;
    }
  }

  auto pos = hit->GetPosition();
  Double_t x = pos.X();
  Double_t y = pos.Y();
  Double_t z = pos.Z();
  Double_t w = hit -> GetCharge();

  Double_t wsum = fW - w;

  fEX  = (fW * fEX - w * x) / wsum;
  fEY  = (fW * fEY - w * y) / wsum;
  fEZ  = (fW * fEZ - w * z) / wsum;
  fEXX = (fW * fEXX - w * x * x) / wsum;
  fEYY = (fW * fEYY - w * y * y) / wsum;
  fEZZ = (fW * fEZZ - w * z * z) / wsum;
  fEXY = (fW * fEXY - w * x * y) / wsum;
  fEYZ = (fW * fEYZ - w * y * z) / wsum;
  fEZX = (fW * fEZX - w * z * x) / wsum;

  fW = wsum;

  return found;
}

vector<KBHit*> *KBHitList::GetHitArray() { return &fHitArray; }
vector<Int_t> *KBHitList::GetHitIDArray() { return &fHitIDArray; }

Int_t KBHitList::GetNumHits() const { return fN; };
KBHit *KBHitList::GetHit(Int_t idx) const { return fHitArray.at(idx); }
Int_t KBHitList::GetHitID(Int_t idx) const { return fHitIDArray.at(idx); }


Double_t KBHitList::GetW()  const { return fW; }
Double_t KBHitList::GetChargeSum()  const { return fW; }

Double_t KBHitList::GetXMean() const { return fEX; }
Double_t KBHitList::GetYMean() const { return fEY; }
Double_t KBHitList::GetZMean() const { return fEZ; }
Double_t KBHitList::GetExpectationX()  const { return fEX; }
Double_t KBHitList::GetExpectationY()  const { return fEY; }
Double_t KBHitList::GetExpectationZ()  const { return fEZ; }

Double_t KBHitList::GetCovWXX() const { return fW * (fEXX - fEX * fEX); }
Double_t KBHitList::GetCovWYY() const { return fW * (fEYY - fEY * fEY); }
Double_t KBHitList::GetCovWZZ() const { return fW * (fEZZ - fEZ * fEZ); }
Double_t KBHitList::GetCovWXY() const { return fW * (fEXY - fEX * fEY); }
Double_t KBHitList::GetCovWYZ() const { return fW * (fEYZ - fEY * fEZ); }
Double_t KBHitList::GetCovWZX() const { return fW * (fEZX - fEZ * fEX); }

Double_t KBHitList::GetVarX()  const { return GetCovWXX()/fW; }
Double_t KBHitList::GetVarY()  const { return GetCovWYY()/fW; }
Double_t KBHitList::GetVarZ()  const { return GetCovWZZ()/fW; }

Double_t KBHitList::GetExpectationXX() const { return fEXX; }
Double_t KBHitList::GetExpectationYY() const { return fEYY; }
Double_t KBHitList::GetExpectationZZ() const { return fEZZ; }
Double_t KBHitList::GetExpectationXY() const { return fEXY; }
Double_t KBHitList::GetExpectationYZ() const { return fEYZ; }
Double_t KBHitList::GetExpectationZX() const { return fEZX; }


TVector3 KBHitList::GetMean()          const { return TVector3(fEX, fEY, fEZ); }
TVector3 KBHitList::GetExpectation()   const { return TVector3(fEX, fEY, fEZ); }
TVector3 KBHitList::GetCovWD()         const { return TVector3(GetCovWXX(),GetCovWYY(),GetCovWZZ()); }
TVector3 KBHitList::GetCovWO()         const { return TVector3(GetCovWXY(),GetCovWYZ(),GetCovWZX()); }
TVector3 KBHitList::GetVar()           const { return 1/fW * GetCovWD(); }
TVector3 KBHitList::GetExpectation2D() const { return TVector3(fEXX,fEYY,fEZZ); }
TVector3 KBHitList::GetExpectation2O() const { return TVector3(fEXY,fEYZ,fEZX); }


KBVector3 KBHitList::GetMean(kbaxis ref)          const { return KBVector3(fEX,fEY,fEZ,ref); }
KBVector3 KBHitList::GetExpectation(kbaxis ref)   const { return KBVector3(fEX,fEY,fEZ,ref); }
KBVector3 KBHitList::GetCovWD(kbaxis ref)         const { return KBVector3(GetCovWXX(),GetCovWYY(),GetCovWZZ(),ref); }
KBVector3 KBHitList::GetCovWO(kbaxis ref)         const { return KBVector3(GetCovWXY(),GetCovWYZ(),GetCovWZX(),ref); }
KBVector3 KBHitList::GetVar(kbaxis ref)           const { return 1/fW * GetCovWD(ref); }
