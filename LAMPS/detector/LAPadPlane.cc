#include "LAPadPlane.hh"

#include "TGraph.h"
#include "TMath.h"
#include "TVector2.h"
#include "TH2Poly.h"
#include "TCollection.h"
#include "TEllipse.h"
#include "TLine.h"
#include "TCanvas.h"

#include <iostream>
using namespace std;

ClassImp(LAPadPlane)

LAPadPlane::LAPadPlane()
:KBPadPlane("LAMPS-TPC Pad Plane", "")
{
}

bool LAPadPlane::Init()
{
  fFuncXRightBound = new TF1("RightBound","1/(TMath::Tan(TMath::Pi()*3/8))*x",10,55);

  fPar -> GetParDouble("rMinTPC", fRMin);
  fPar -> GetParDouble("rMaxTPC", fRMax);

  fPar -> GetParInt("ppNLayerDivision", fNLayerDivision);

  fArcLength = new Double_t[fNLayerDivision]; fRadius = new Double_t[fNLayerDivision];
  fNLayers = new Int_t[fNLayerDivision];

  for (Int_t iDiv = 0; iDiv < fNLayerDivision; iDiv++) {
    fPar -> GetParDouble(Form("ppArcLength%d",iDiv), fArcLength[iDiv]);
    fPar -> GetParDouble(Form("ppRadius%d",iDiv), fRadius[iDiv]);
    fPar -> GetParInt(Form("ppNLayers%d",iDiv), fNLayers[iDiv]);
  }

  fTanPi1o8 = TMath::Tan(TMath::Pi()*1./8.);
  fTanPi3o8 = TMath::Tan(TMath::Pi()*3./8.);
  fTanPi5o8 = TMath::Tan(TMath::Pi()*5./8.);
  fTanPi7o8 = TMath::Tan(TMath::Pi()*7./8.);
  for (Int_t i = 0; i < 8; i++) {
    fCosPiNo4[i] = TMath::Cos(TMath::Pi()*i/4.);
    fSinPiNo4[i] = TMath::Sin(TMath::Pi()*i/4.);
  }

  Double_t rMinLayer = fRMin;
  Double_t rMaxLayer = fRMin + fRadius[0];

  for (Int_t iDiv = 0; iDiv < fNLayerDivision; iDiv++) {
    fLayerDivI.push_back(fNLayersTotal);
    fRDivI.push_back(rMinLayer);

    for (Int_t iLayer = 0; iLayer < fNLayers[iDiv]; iLayer++) {
      rMaxLayer = rMinLayer + fRadius[iDiv];

      Double_t phiPadEnd = fArcLength[iDiv]/rMinLayer;
      Int_t nHalfRows = (Int_t)(TMath::Pi()/8./phiPadEnd) -1;

      fNHalfRowsInLayer.push_back(nHalfRows);
      rMinLayer += fRadius[iDiv];
      fNLayersTotal++;
    }
  }

  for (Int_t iSection = 0; iSection < 8; iSection++) {
    for (Int_t iLayer = 0; iLayer < fNLayersTotal; iLayer++) {
      Int_t idx = FindDivisionIndex(iLayer);

      rMinLayer = fRDivI.at(idx) + (iLayer - fLayerDivI.at(idx)) * fRadius[idx];
      rMaxLayer = rMinLayer + fRadius[idx];
      Double_t rMid = (rMinLayer + rMaxLayer)/2.;

      Int_t nHalfRows = fNHalfRowsInLayer[iLayer];
      for (Int_t iRow = 1; iRow < nHalfRows+1; iRow++)
      {
        Double_t phiPadMid = (iRow - 0.5) * fArcLength[idx]/rMinLayer;
        Double_t phiSection = iSection * TMath::Pi()/4. + TMath::Pi()/2.;

        TVector2 pos;
        pos.SetMagPhi(rMid, phiSection);

        TVector2 posL = pos.Rotate(phiPadMid);
        AddPad(iSection, -iRow, iLayer, posL.X(), posL.Y());

        TVector2 posR = pos.Rotate(-phiPadMid);
        AddPad(iSection,  iRow, iLayer, posR.X(), posR.Y());
      }
    }
  }

  for (Int_t iSection = 0; iSection < 8; iSection++) {
    for (Int_t iLayer = 0; iLayer < fNLayersTotal; iLayer++) {
      Int_t nHalfRows = fNHalfRowsInLayer[iLayer];
      for (Int_t iRow = -nHalfRows; iRow < nHalfRows; iRow++) {
        if (iRow == 0)
          continue;
        std::vector<Int_t> key0;
        key0.push_back(iSection);
        key0.push_back(iRow);
        key0.push_back(iLayer);
        auto pad0 = (KBPad *) fChannelArray -> At(fPadMap[key0]);

        Int_t row1 = iRow+1;
        if (iRow == -1)
          row1 = 1;

        std::vector<Int_t> key1;
        key1.push_back(iSection);
        key1.push_back(row1);
        key1.push_back(iLayer);
        auto pad1 = (KBPad *) fChannelArray -> At(fPadMap[key1]);

        pad0 -> AddNeighborPad(pad1);
        pad1 -> AddNeighborPad(pad0);
      }
    }
  }

  for (Int_t iLayer = 0; iLayer < fNLayersTotal; iLayer++) {
    Int_t nHalfRows = fNHalfRowsInLayer[iLayer];
    for (Int_t iSection = 0; iSection < 8; iSection++) {
      std::vector<Int_t> key0;
      key0.push_back(iSection);
      key0.push_back(nHalfRows);
      key0.push_back(iLayer);
      auto pad0 = (KBPad *) fChannelArray -> At(fPadMap[key0]);

      Int_t section1 = iSection+1;
      if (iSection == 7)
        section1 = 0;

      std::vector<Int_t> key1;
      key1.push_back(section1);
      key1.push_back(-nHalfRows);
      key1.push_back(iLayer);
      auto pad1 = (KBPad *) fChannelArray -> At(fPadMap[key1]);

      pad0 -> AddNeighborPad(pad1);
      pad1 -> AddNeighborPad(pad0);
    }
  }

  for (Int_t iSection = 0; iSection < 8; iSection++) {
    for (Int_t iLayer = 0; iLayer < fNLayersTotal-1; iLayer++) {

      Int_t layerCurrent = iLayer;
      Int_t layerNext = iLayer + 1;

      Int_t nHalfRowsCurrent = fNHalfRowsInLayer[layerCurrent];
      Int_t nHalfRowsNext = fNHalfRowsInLayer[layerNext];

      for (Int_t iRow = -nHalfRowsCurrent; iRow <= nHalfRowsCurrent; iRow++) 
      {
        if (iRow == 0)
          continue;

        Int_t rowCurrent = iRow;
        std::vector<Int_t> keyCurrent;
        keyCurrent.push_back(iSection);
        keyCurrent.push_back(rowCurrent);
        keyCurrent.push_back(layerCurrent);
        auto padCurrent = (KBPad *) fChannelArray -> At(fPadMap[keyCurrent]);

        Int_t row0 = rowCurrent;
        Int_t layer0 = layerCurrent;

        Int_t div = FindDivisionIndex(layer0);
        Double_t rMin0 = fRDivI.at(div)+(layer0-fLayerDivI.at(div))*fRadius[div];
        Double_t phiPadCurrent = (row0-0.5)*fArcLength[div]/rMin0;

        Double_t dPhi = fArcLength[div]/rMin0;

        for (Int_t iRowNext = -nHalfRowsNext; iRowNext <= nHalfRowsNext; iRowNext++) 
        {
          if (iRowNext == 0)
            continue;

          Int_t rowNext = iRowNext;
          std::vector<Int_t> keyNext;
          keyNext.push_back(iSection);
          keyNext.push_back(rowNext);
          keyNext.push_back(layerNext);
          auto padNext = (KBPad *) fChannelArray -> At(fPadMap[keyNext]);

          row0 = rowNext;
          layer0 = layerNext;

          div = FindDivisionIndex(layer0);
          rMin0 = fRDivI.at(div)+(layer0-fLayerDivI.at(div))*fRadius[div];
          Double_t phiPadNext = (row0-0.5)*fArcLength[div]/rMin0;

          if (TMath::Abs(phiPadCurrent - phiPadNext) < dPhi) {
            padCurrent -> AddNeighborPad(padNext);
            padNext -> AddNeighborPad(padCurrent);
          }
        }
      }
    }
  }

  return true;
}

Int_t LAPadPlane::FindPadID(Int_t section, Int_t row, Int_t layer)
{
  if (layer < 0 || layer >= fNLayersTotal)
    return -1;

  if (row == 0 || std::abs(row) > fNHalfRowsInLayer[layer])
    return -1;

  std::vector<Int_t> key;
  key.push_back(section);
  key.push_back(row);
  key.push_back(layer);

  Int_t id = fPadMap[key];

  return id;
}

Int_t LAPadPlane::FindPadID(Double_t i, Double_t j)
{
  Int_t section = FindSection(i,j);

  Double_t xRot =  i*fCosPiNo4[section] + j*fSinPiNo4[section];
  Double_t yRot = -i*fSinPiNo4[section] + j*fCosPiNo4[section];

  TVector2 pos(xRot, yRot);
  Double_t r = pos.Mod();
  Double_t phi = pos.Phi();
  phi -= TMath::Pi()/2.;

  Int_t idx = FindDivisionIndex(r);
  if (idx == -1)
    return -1;

  Int_t layer = ((Int_t) ((r - fRDivI.at(idx)) / fRadius[idx])) + fLayerDivI.at(idx);
  if (layer >= fNLayersTotal)
    return -1;

  Double_t rMin = fRDivI.at(idx) + (layer - fLayerDivI.at(idx)) * fRadius[idx];
  Double_t phiOnePad = fArcLength[idx] / rMin;

  Int_t row;
  if (phi > 0) row = ((Int_t) (phi / phiOnePad)) + 1;
  else         row = ((Int_t) (phi / phiOnePad)) - 1;
  row *= -1;
  if (std::abs(row) > fNHalfRowsInLayer[layer])
    return -1;

  std::vector<Int_t> key;
  key.push_back(section);
  key.push_back(row);
  key.push_back(layer);

  Int_t id = fPadMap[key];

  return id;
}

bool LAPadPlane::IsInBoundary(Double_t i, Double_t j)
{
  Double_t r = TMath::Sqrt(i*i+j*j);
  if (r < fRMin || r > fRMax)
    return false;

  return true;
}

TH2* LAPadPlane::GetHist(Option_t *option)
{
  if (fH2Plane != nullptr)
    return fH2Plane; 

  TH2Poly *h2 = new TH2Poly();

  Double_t xPoints[5] = {0};
  Double_t yPoints[5] = {0};

  Double_t scalePhi = 0.8;
  Double_t scaleR = 0.9;

  KBPad *pad;
  TIter iterPads(fChannelArray);
  //Int_t countBig = 0;
  //Int_t countSmall = 0;
  Int_t selectSection = TString(option).Atoi();
  while ((pad = (KBPad *) iterPads.Next())) 
  {
    if(selectSection != -1 && selectSection != pad -> GetSection())
      continue;

    Int_t layer = pad -> GetLayer();
    Double_t x = pad -> GetI();
    Double_t y = pad -> GetJ();

    Int_t idx = FindDivisionIndex(layer);

    Double_t rMin = fRDivI.at(idx) + (layer - fLayerDivI.at(idx)) * fRadius[idx];
    Double_t rMax = rMin + scaleR * fRadius[idx];
    Double_t phiOnePad = fArcLength[idx] / rMin;

    phiOnePad = scalePhi * phiOnePad;

    TVector2 pos(x,y);
    TVector2 posPoint;

    posPoint = pos.Rotate(phiOnePad/2.);
    posPoint.SetMagPhi(rMin,posPoint.Phi());
    xPoints[0] = posPoint.X();
    yPoints[0] = posPoint.Y();
    xPoints[4] = posPoint.X();
    yPoints[4] = posPoint.Y();

    posPoint = pos.Rotate(-phiOnePad/2.);
    posPoint.SetMagPhi(rMin,posPoint.Phi());
    xPoints[1] = posPoint.X();
    yPoints[1] = posPoint.Y();

    posPoint = pos.Rotate(-phiOnePad/2.);
    posPoint.SetMagPhi(rMax,posPoint.Phi());
    xPoints[2] = posPoint.X();
    yPoints[2] = posPoint.Y();

    posPoint = pos.Rotate(phiOnePad/2.);
    posPoint.SetMagPhi(rMax,posPoint.Phi());
    xPoints[3] = posPoint.X();
    yPoints[3] = posPoint.Y();

    h2 -> AddBin(5, xPoints, yPoints);
    /*
    Double_t xMean = 0, yMean = 0;
    for (auto i = 0; i < 4; i++) {
      xMean += xPoints[i];
      yMean += yPoints[i];
    }
    xMean = xMean/4;
    yMean = yMean/4;
    if (sqrt(xMean*xMean + yMean*yMean) > 289) {
      h2 -> Fill(xMean, yMean, 10);
      countBig++;
    }
    else {
      h2 -> Fill(xMean, yMean, 1);
      countSmall++;
    }
    */
  }
  //cout << countBig << " " << countSmall << endl;

  if (true) {
    TList *bins = h2->GetBins(); TIter Next(bins);
    TObject *obj;
    TH2PolyBin *b;
    TGraph *g;
    while ((obj = Next())) {
      b = (TH2PolyBin*)obj;
      g = (TGraph*)b->GetPolygon();
      if (g) g->SetLineWidth(1);
      if (g) g->SetLineColor(kGray+2);
    }
  }

  fH2Plane = (TH2 *) h2;
  fH2Plane -> SetStats(0);
  fH2Plane -> SetTitle(";x (mm); y (mm)");
  fH2Plane -> GetXaxis() -> CenterTitle();
  fH2Plane -> GetYaxis() -> CenterTitle();

  return fH2Plane;
}

void LAPadPlane::DrawFrame(Option_t *option)
{
  Color_t lineColor = kBlack;

  TEllipse *outerCircle = new TEllipse(0, 0, fRMax, fRMax);
            outerCircle -> SetFillStyle(0);
            outerCircle -> SetLineColor(lineColor);
            outerCircle -> Draw("samel");

  TEllipse *innerCircle = new TEllipse(0, 0, fRMin, fRMin);
            innerCircle -> SetFillStyle(0);
            innerCircle -> SetLineColor(lineColor);
            innerCircle -> Draw("samel");

  TLine* line1 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*1/8),  fRMax*TMath::Sin(TMath::Pi()*1/8),
                fRMin*TMath::Cos(TMath::Pi()*1/8),  fRMin*TMath::Sin(TMath::Pi()*1/8));
  TLine* line2 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*3/8),  fRMax*TMath::Sin(TMath::Pi()*3/8),
                fRMin*TMath::Cos(TMath::Pi()*3/8),  fRMin*TMath::Sin(TMath::Pi()*3/8));
  TLine* line3 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*5/8),  fRMax*TMath::Sin(TMath::Pi()*5/8),
                fRMin*TMath::Cos(TMath::Pi()*5/8),  fRMin*TMath::Sin(TMath::Pi()*5/8));
  TLine* line4 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*7/8),  fRMax*TMath::Sin(TMath::Pi()*7/8),
                fRMin*TMath::Cos(TMath::Pi()*7/8),  fRMin*TMath::Sin(TMath::Pi()*7/8));
  TLine* line5 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*9/8),  fRMax*TMath::Sin(TMath::Pi()*9/8),
                fRMin*TMath::Cos(TMath::Pi()*9/8),  fRMin*TMath::Sin(TMath::Pi()*9/8));
  TLine* line6 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*11/8), fRMax*TMath::Sin(TMath::Pi()*11/8),
                fRMin*TMath::Cos(TMath::Pi()*11/8), fRMin*TMath::Sin(TMath::Pi()*11/8));
  TLine* line7 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*13/8), fRMax*TMath::Sin(TMath::Pi()*13/8),
                fRMin*TMath::Cos(TMath::Pi()*13/8), fRMin*TMath::Sin(TMath::Pi()*13/8));
  TLine* line8 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*15/8), fRMax*TMath::Sin(TMath::Pi()*15/8),
                fRMin*TMath::Cos(TMath::Pi()*15/8), fRMin*TMath::Sin(TMath::Pi()*15/8));

  line1 -> SetLineColor(lineColor);
  line2 -> SetLineColor(lineColor);
  line3 -> SetLineColor(lineColor);
  line4 -> SetLineColor(lineColor);
  line5 -> SetLineColor(lineColor);
  line6 -> SetLineColor(lineColor);
  line7 -> SetLineColor(lineColor);
  line8 -> SetLineColor(lineColor);

  line1 -> Draw("samel");
  line2 -> Draw("samel");
  line3 -> Draw("samel");
  line4 -> Draw("samel");
  line5 -> Draw("samel");
  line6 -> Draw("samel");
  line7 -> Draw("samel");
  line8 -> Draw("samel");
}

TCanvas *LAPadPlane::GetCanvas(Option_t *option)
{
  if (fCanvas == nullptr)
    fCanvas = new TCanvas(fName+Form("%d",fPlaneID),fName,800,800);

  return fCanvas;
}

void LAPadPlane::AddPad(Int_t section, Int_t row, Int_t layer, Double_t i, Double_t j)
{
  std::vector<Int_t> key;
  key.push_back(section);
  key.push_back(row);
  key.push_back(layer);

  Int_t id = fChannelArray -> GetEntriesFast();

  KBPad *pad = new KBPad();
  pad -> SetID(id);
  pad -> SetPosition(i, j);
  pad -> SetSectionRowLayer(section, row, layer);

  fPadMap.insert(std::pair<std::vector<Int_t>, Int_t>(key,id));
  fChannelArray -> Add(pad);
  fNPadsTotal++;
}

Int_t LAPadPlane::FindSection(Double_t i, Double_t j)
{
  if (j > fTanPi3o8*i) {
    if (j > fTanPi1o8*i) {
      if (j > fTanPi7o8*i) {
        if (j > fTanPi5o8*i) {
          return 0;
        } else return 1;
      } else return 2;
    } else return 3;
  }
  else
  {
    if (j < fTanPi1o8*i) {
      if (j < fTanPi7o8*i) {
        if (j < fTanPi5o8*i) {
          return 4;
        } else return 5;
      } else return 6;
    } else return 7;
  }
}

Int_t LAPadPlane::FindDivisionIndex(Int_t layer)
{
  Int_t idx = -1;
  for (Int_t iDiv = 0; iDiv < fNLayerDivision; iDiv++) {
    if (layer >= fLayerDivI.at(iDiv)) idx = iDiv;
    else break;
  }
  return idx;
}

Int_t LAPadPlane::FindDivisionIndex(Double_t r)
{
  Int_t idx = -1;
  for(Int_t iDiv=0; iDiv<fNLayerDivision; iDiv++) {
    if(r >= fRDivI.at(iDiv)) idx = iDiv;
    else break;
  }
  return idx;
}
