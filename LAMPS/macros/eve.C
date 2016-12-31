void eve()
{
  auto run = new KBRun();
  run -> SetInputFile("run0101.reco");
  run -> AddParameterFile("LAP.par");
  run -> AddDetector(new LAPTpc());
  run -> Init();
  run -> SetGeoTransparency(80);

  run -> RunEve(0);
}
