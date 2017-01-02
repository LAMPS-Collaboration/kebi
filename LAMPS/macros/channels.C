void SetHist(TH1D* hist);

void channels(Int_t eventID = 0)
{
  gStyle -> SetOptStat(0);
  gStyle -> SetPadTopMargin(0.10);
  gStyle -> SetPadBottomMargin(0.10);
  gStyle -> SetPadLeftMargin(0.11);
  gStyle -> SetPadRightMargin(0.03);
  gStyle -> SetTitleFontSize(0.06);

  auto run = new KBRun();
  run -> SetInputFile("run0101.reco");
  run -> AddDetector(new LAPTpc());
  run -> Init();

  TClonesArray *padArray = (TClonesArray *) run -> GetBranch("Pad");
  run -> GetEntry(eventID);

  Int_t idxRef;
  KBPad *padRef;
  for (auto iPad = 0; iPad < 306; iPad++) {
    padRef = (KBPad *) padArray -> At(iPad);
    idxRef = iPad;
    auto noiseAmpRef = padRef -> GetNoiseAmplitude();
    if (noiseAmpRef == 1)
      break;
  }

  {
    auto histRef = padRef -> GetHist("r");
    SetHist(histRef);
    histRef -> SetTitle(Form("Event %d (Raw)",eventID));

    auto cvs = new TCanvas(Form("channels_event_%04d_raw",eventID),"cvs",20,20,800,680);
    histRef -> Draw("l");

    for (auto iPad = 0; iPad < 306; iPad++) {
      if (idxRef == iPad)
        continue;
      auto pad = (KBPad *) padArray -> At(iPad);
      auto hist = pad -> GetHist("r");
      SetHist(hist);
      hist -> Draw("samel");
    }
    //cvs -> SaveAs(TString("figures/")+cvs->GetName()+".pdf");
  }

  {
    auto histRef = padRef -> GetHist("o");
    SetHist(histRef);
    histRef -> SetTitle(Form("Event %d (Out)",eventID));

    auto cvs = new TCanvas(Form("channels_event_%04d_out",eventID),"cvs",40,40,800,680);
    histRef -> Draw("l");

    for (auto iPad = 0; iPad < 306; iPad++) {
      if (idxRef == iPad)
        continue;
      auto pad = (KBPad *) padArray -> At(iPad);
      auto hist = pad -> GetHist("o");
      SetHist(hist);
      hist -> Draw("samel");
    }
    //cvs -> SaveAs(TString("figures/")+cvs->GetName()+".pdf");
  }
}

void SetHist(TH1D* hist)
{
  hist -> SetTitleSize(0.04,"xy");
  hist -> SetTitleOffset(1.4,"y");
  hist -> SetTitleOffset(1.1,"x");
  hist -> GetXaxis() -> CenterTitle();
  hist -> GetYaxis() -> CenterTitle();
}
