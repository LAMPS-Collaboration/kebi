void reco()
{
  auto run = new KBRun();
  run -> SetInputFile("/Users/ejungwoo/KEBI/data/tpc.digi.root");
  run -> SetOutputFile("/Users/ejungwoo/KEBI/data/tpc.reco.root");
  run -> AddDetector(new LATpc());

  auto psa = new KBPSATask();
  psa -> SetPSA(new KBPSAFastFit());

  run -> Add(psa);
  run -> Add(new LATrackFindingTask());
  run -> Add(new LAVertexFindingTask());

  run -> Init();
  run -> Run();
}
