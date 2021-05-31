void run_track_finding()
{
  auto run = new KBRun();
  run -> SetIOFile("komac0036.reco","komac0036.tf");
  run -> AddDetector(new LAPTpc());
  run -> AddPar("prototype_komac.par");

  run -> Add(new LAPTrackFindingTask());
  run -> GetPar() -> SetPar("runCopyBranchToOuput s[] Hit");

  run -> Init();
  run -> Print();
  run -> Run();
}
