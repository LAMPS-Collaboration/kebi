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
  auto hist_padplane = padplane -> GetHist();

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

    if (section == 0 && row == 0 && layer) {
      pad -> Print();
      pad -> GetHist("raw") -> Draw();
      return;
    }
  }

  padplane -> GetCanvas();
  gStyle -> SetPalette(kBird);
  hist_padplane -> Draw("colz");
}
