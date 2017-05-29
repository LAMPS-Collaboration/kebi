void digi()
{
  auto run = new KBRun();
  run -> SetInputFile("/Users/ejungwoo/KEBI/data/tpc.mc.root");
  run -> SetOutputFile("/Users/ejungwoo/KEBI/data/tpc.digi.root");
  run -> AddDetector(new LATpc());
  run -> AddParameterFile("lamps.par");

  auto drift = new LADriftElectronTask();
  drift -> SetPadPersistency(true);

  auto electronics = new LAElectronicsTask();

  auto psa = new KBPSATask();
  psa -> SetPSA(new KBPSAFastFit());

  run -> Add(drift);
  run -> Add(electronics);
  run -> Add(psa);

  run -> Init();
  run -> RunSingle(0);
}
