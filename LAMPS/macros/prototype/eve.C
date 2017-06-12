void eve_prototype(TString name = "run0101.reco")
{
  auto run = new KBRun();
  run -> SetInputFile(name);
  run -> AddParameterFile("LAP.par");
  run -> AddDetector(new LAPTpc());
  run -> Init();
  run -> SetGeoTransparency(80);

  run -> RunEve(0);
}
