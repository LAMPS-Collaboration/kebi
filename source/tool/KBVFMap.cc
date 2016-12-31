/**
 * Vector Field Map
*/

#include <iostream>
using namespace std;

#include "TLine.h"
#include "TArrow.h"

#include "KBVFMap.hh"

ClassImp(KBVFMap)

KBVFMap::KBVFMap(TH2* sample)
{
  InitializeField(sample);
}

void KBVFMap::InitializeField(TH2* sample)
{
  fNBinsX = sample -> GetXaxis() -> GetNbins();
  fNBinsY = sample -> GetYaxis() -> GetNbins();

  fLowLimitX  = sample -> GetXaxis() -> GetBinLowEdge(1);
  fHighLimitX = sample -> GetXaxis() -> GetBinLowEdge(fNBinsX-1);
  fBinWidthX  = sample -> GetXaxis() -> GetBinWidth(1);

  fLowLimitY  = sample -> GetYaxis() -> GetBinLowEdge(1);
  fHighLimitY = sample -> GetYaxis() -> GetBinLowEdge(fNBinsY-1);
  fBinWidthY  = sample -> GetYaxis() -> GetBinWidth(1);

  fVFPoints = new KBVFPoint**[fNBinsX];
  for(auto xbin = 0; xbin < fNBinsX; xbin++) {
    fVFPoints[xbin] = new KBVFPoint*[fNBinsY];
    for(auto ybin = 0; ybin < fNBinsY; ybin++) {
      fVFPoints[xbin][ybin] = new KBVFPoint();
      auto point = fVFPoints[xbin][ybin];
      auto bin = sample -> GetBin(xbin+1, ybin+1);
      point -> SetValue(sample -> GetBinContent(bin));
    }
  }

  for(auto xbin = 1; xbin < fNBinsX-1; xbin++) {
    for(auto ybin = 1; ybin < fNBinsY-1; ybin++) {
      auto point = fVFPoints[xbin][ybin];

      point -> SetNeighborPoint(0, fVFPoints[xbin+1][ybin]);
      point -> SetNeighborPoint(1, fVFPoints[xbin][ybin+1]);
      point -> SetNeighborPoint(2, fVFPoints[xbin-1][ybin]);
      point -> SetNeighborPoint(3, fVFPoints[xbin][ybin-1]);
      point -> SetNeighborPoint(4, fVFPoints[xbin+1][ybin+1]);
      point -> SetNeighborPoint(5, fVFPoints[xbin-1][ybin+1]);
      point -> SetNeighborPoint(6, fVFPoints[xbin-1][ybin-1]);
      point -> SetNeighborPoint(7, fVFPoints[xbin+1][ybin-1]);

      point -> SetIsActive(true);
    }
  }

  for(auto xbin = 1; xbin < fNBinsX-1; xbin++) {
    for(auto ybin = 1; ybin < fNBinsY-1; ybin++) {
      auto point = fVFPoints[xbin][ybin];
      point -> Initialize();

      auto vx = point -> GetVX();
      auto vy = point -> GetVY();
      auto mag = std::sqrt(vx*vx + vy*vy);

      if (fMaxV < mag)
        fMaxV = mag;
    }
  }

  fBinWidthXY = std::sqrt(fBinWidthX*fBinWidthX + fBinWidthY*fBinWidthY);
}

void KBVFMap::Fit(TF1* function, Int_t itMax, Option_t *opt)
{
  TString option = opt;
  option.ToLower();

  bool drawBox = false;
  if (option.Contains("b"))
    drawBox = true;

  bool drawFunction = false;
  if (option.Contains("f"))
    drawFunction = true;

  for (auto iter = 0; iter < itMax; iter++)
  {
    Double_t vxSum = 0;
    Double_t vySum = 0;
    Int_t    count = 0;

    for(auto xbin = 1; xbin < fNBinsX-1; xbin++) 
    {
      auto xLE = fLowLimitX + fBinWidthX * (xbin);
      auto xHE = fLowLimitX + fBinWidthX * (xbin+1);
      auto yAtXLE = function -> Eval(xLE);
      auto yAtXHE = function -> Eval(xHE);
      auto yBinAtXLE = int((yAtXLE - fLowLimitY)/fBinWidthY);
      auto yBinAtXHE = int((yAtXHE - fLowLimitY)/fBinWidthY);

      auto yAtCenterOfXBin = function -> Eval((xLE+xHE)/2.);

      if (yBinAtXLE > yBinAtXHE) {
        auto temp = yBinAtXLE;
        yBinAtXLE = yBinAtXHE;
        yBinAtXHE = temp;
      }
      
      auto numYBins = yBinAtXHE - yBinAtXLE + 1;
      for (auto iy = 0; iy < numYBins; iy++) {
        auto ybin = yBinAtXLE + iy;
        if (ybin < 1 || ybin > fNBinsY-2)
          continue;

        auto yAtYBin = fLowLimitY + fBinWidthY * (ybin+.5);
        auto dY = yAtYBin - yAtCenterOfXBin;

        auto point = fVFPoints[xbin][ybin];
        vxSum += point -> GetVX() / dY*dY;
        vySum += point -> GetVY() / dY*dY;
        count++;
      }
    }

    vxSum = vxSum / count;
    vySum = vySum / count;

    auto dx = vxSum * fBinWidthX / fMaxV;
    auto dy = vySum * fBinWidthY / fMaxV;

    function -> SetParameter(0, function -> GetParameter(0) + dx);
    function -> SetParameter(1, function -> GetParameter(1) + dy);
  }

  if (drawBox) 
  {
    for(auto xbin = 1; xbin < fNBinsX-1; xbin++) 
    {
      auto xLE = fLowLimitX + fBinWidthX * (xbin);
      auto xHE = fLowLimitX + fBinWidthX * (xbin+1);
      auto yAtXLE = function -> Eval(xLE);
      auto yAtXHE = function -> Eval(xHE);
      auto yBinAtXLE = Int_t((yAtXLE - fLowLimitY)/fBinWidthY);
      auto yBinAtXHE = Int_t((yAtXHE - fLowLimitY)/fBinWidthY);

      if (yBinAtXLE > yBinAtXHE) {
        auto temp = yBinAtXLE;
        yBinAtXLE = yBinAtXHE;
        yBinAtXHE = temp;
      }
      
      auto numYBins = yBinAtXHE - yBinAtXLE + 1;
      for (auto iy = 0; iy < numYBins; iy++) {
        auto ybin = yBinAtXLE + iy;
        if (ybin < 1 || ybin > fNBinsY-2)
          continue;
        DrawBox(xbin, ybin);
      }
    }
  }

  if (drawFunction)
    function -> DrawClone("same");
}

void KBVFMap::DrawBox(Int_t x, Int_t y)
{
  auto xw = fBinWidthX+fLowLimitX;
  auto x1 = x*fBinWidthX+fLowLimitX;
  auto x2 = x1 + xw;

  auto yw = fBinWidthY+fLowLimitY;
  auto y1 = y*fBinWidthY+fLowLimitY;
  auto y2 = y1 + yw;

  TLine *line[4];
  line[0] = new TLine(x1, y1, x2, y1);
  line[1] = new TLine(x1, y2, x2, y2);
  line[2] = new TLine(x1, y1, x1, y2);
  line[3] = new TLine(x2, y1, x2, y2);

  for (auto i = 0; i < 4; i++) {
    line[i] -> SetLineColor(kGray+1);
    line[i] -> Draw();
  }
}

void KBVFMap::DrawField()
{
  for(auto xbin = 1; xbin < fNBinsX-1; xbin++) {
    for(auto ybin = 1; ybin < fNBinsY-1; ybin++) {
      auto point = fVFPoints[xbin][ybin];
      auto vx = point -> GetVX();
      auto vy = point -> GetVY();

      auto binWidth = std::sqrt(fBinWidthX*fBinWidthX + fBinWidthX*fBinWidthX);

      auto mag = std::sqrt(vx*vx + vy*vy);
      auto scale = .2 * binWidth * std::log(mag) / std::log(fMaxV);

      vx *= scale / mag;
      vy *= scale / mag;

      auto arrowSize = .008 * scale;

      auto binx = fLowLimitX + fBinWidthX * (0.5 + xbin);
      auto biny = fLowLimitY + fBinWidthY * (0.5 + ybin);

      auto arrow = new TArrow(binx - vx, biny - vy, binx + vx, biny + vy, arrowSize, "|>");
      arrow -> Draw();
    }
  }
}

void KBVFMap::SetNBinsX(Int_t n)        { fNBinsX = n; }
void KBVFMap::SetLowLimitX(Double_t v)  { fLowLimitX = v; }
void KBVFMap::SetHighLimitX(Double_t v) { fHighLimitX = v; }
void KBVFMap::SetBinWidthX(Double_t v)  { fBinWidthX = v; }

Int_t    KBVFMap::GetNBinsX()      const { return fNBinsX; }
Double_t KBVFMap::GetLowLimitX()   const { return fLowLimitX; }
Double_t KBVFMap::GetHighLimitX()  const { return fHighLimitX; }
Double_t KBVFMap::GetBinWidthX()   const { return fBinWidthX; }

void KBVFMap::SetNBinsY(Int_t n)        { fNBinsY = n; }
void KBVFMap::SetLowLimitY(Double_t v)  { fLowLimitY = v; }
void KBVFMap::SetHighLimitY(Double_t v) { fHighLimitY = v; }
void KBVFMap::SetBinWidthY(Double_t v)  { fBinWidthY = v; }

Int_t    KBVFMap::GetNBinsY()      const { return fNBinsY; }
Double_t KBVFMap::GetLowLimitY()   const { return fLowLimitY; }
Double_t KBVFMap::GetHighLimitY()  const { return fHighLimitY; }
Double_t KBVFMap::GetBinWidthY()   const { return fBinWidthY; }

void KBVFMap::SetMaxV(Double_t v) { fMaxV = v; }
Double_t KBVFMap::GetMaxV() const { return fMaxV; }
