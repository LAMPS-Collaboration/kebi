void example_pad()
{
  auto run = new KBRun();
  auto tpc = new LAPTpc();
  //run -> AddInput("/home/lamps/KEBI/data/run0101.decoder_test.root");
  run -> AddInput("last");
  run -> AddParameterFile("prototype.par");
  run -> AddDetector(tpc);
  run -> Init();

  auto padArray = (TClonesArray *) run -> GetBranch("Pad");

  Int_t numEvents = run -> GetNumEvents();
  run -> GetEvent(0);

  auto padplane = tpc -> GetPadPlane();
  padplane -> SetPadArray(padArray);

  /////////////////////////////////////////////////////////////////
  //1st example of getting pad
  Int_t numPads = padArray -> GetEntries();
  for (auto iPad = 0; iPad < numPads; ++iPad) 
  {
    auto pad = (KBPad *) padArray -> At(iPad);
    Int_t padID = pad -> GetPadID();
    Int_t asadID = pad -> GetAsAdID();
    Int_t agetID = pad -> GetAGETID();
    Int_t channelID = pad -> GetChannelID();

    Double_t x = pad -> GetI();
    Double_t y = pad -> GetJ();

    Int_t section = pad -> GetSection();
    Int_t row = pad -> GetRow();
    Int_t layer = pad -> GetLayer();

    Short_t *adc = pad -> GetBufferRaw();

    if (section == 0 && row == 0 && layer == 2) {
      pad -> Print();
      new TCanvas();
      pad -> GetHist("raw") -> Draw();
      break;
    }
  }
  /////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////
  //2nd example of getting pad
  Int_t section, row, layer;
  auto padID = padplane -> FindPadID(section=0, row=0, layer=3);
  auto pad = padplane -> GetPad(padID);
  pad -> Print();
  new TCanvas();
  pad -> GetHist("raw") -> Draw();
  /////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////
  //pad plane
  auto hist_padplane = padplane -> GetHist();
  for (auto iPad = 0; iPad < padplane -> GetNPads(); ++iPad) {
    auto pad2 = padplane -> GetPad(iPad);
    auto bin = hist_padplane -> FindBin(pad2 -> GetI(), pad2 -> GetJ());
    hist_padplane -> SetBinContent(bin, pad2 -> GetPadID());
    //hist_padplane -> SetBinContent(bin, pad2 -> GetSection());
    //hist_padplane -> SetBinContent(bin, pad2 -> GetLayer());
    //hist_padplane -> SetBinContent(bin, pad2 -> GetRow());
  }
  auto cvs = padplane -> GetCanvas();
  gStyle -> SetPalette(kBird);
  hist_padplane -> SetTitle("PadPlane_PadID");
  //hist_padplane -> SetTitle("PadPlane_Section");
  //hist_padplane -> SetTitle("PadPlane_Layer");
  //hist_padplane -> SetTitle("PadPlane_Row");
  hist_padplane -> Draw("text");
  padplane -> DrawFrame();
  //cvs -> SaveAs(TString(hist_padplane->GetTitle())+".png");
  /////////////////////////////////////////////////////////////////
}
