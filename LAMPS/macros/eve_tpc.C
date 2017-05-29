void eve_tpc(TString name = "tpc.digi")
{
  auto run = new KBRun();
  run -> SetInputFile(name);
  run -> AddDetector(new LATpc());
  run -> AddParameterFile("lamps.par");
  run -> Init();
  run -> SetGeoTransparency(80);

  run -> RunEve(0);
}
