KBRun *run;

void next(int num=1)
{
  for (auto i=0; i<num; ++i)
    run -> ExecStep();
  run -> RunEve(-999,"e");
}

void run_step()
{
  run = new KBRun();
  run -> SetIOFile("komac0036.reco","komac0036.tf");
  run -> AddDetector(new LAPTpc());
  run -> SetPar("runCopyBranchToOuput s Hit");
  run -> AddPar("prototype_step.par");
  run -> Add(new LAPTrackFindingTask());

  run -> Init();
  //run -> GetEvent(82); run -> SetEntries(83);
  //run -> GetEvent(0); run -> SetEntries(1);
  run -> GetEvent(1); run -> SetEntries(2);
  //while(run -> ExecStep()) {}
  for (auto i=0; i<120; ++i) run -> ExecStep();
  run -> RunEve(-999,"e");
}
