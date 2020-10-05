//void run_reco(Int_t runID = 98, Int_t split = 0, Int_t numEventsPerSplit=2000)
void run_reco(Int_t runID = 98, Int_t split = 0, Int_t numEventsPerSplit=2000)
{
  auto run = new KBRun();
  run -> SetRunID(runID);
  run -> SetTag("reco");
  run -> SetSplit(split, numEventsPerSplit);
  run -> AddPar("prototype.par");
  run -> AddDetector(new LAPTpc());

  auto pathToRawData  = run -> GetPar() -> GetParString("pathToRawData");
  auto pathToMetaData = run -> GetPar() -> GetParString("pathToMetaData");

  string line;
  Int_t runID0, numEventsNalval=0, numEventsVME=0;
  ifstream summary(run -> GetPar() -> GetParString("expRunSummary"));
  getline(summary,line);
  getline(summary,line);
  getline(summary,line);
  while (getline(summary,line)) {
    istringstream ss(line);
    ss >> runID0;
    if (runID0==runID) {
      ss >> numEventsNalval >> numEventsVME;
      break;
    }
  }
  Int_t numEventsInRun = (numEventsNalval<numEventsVME) ? numEventsNalval : numEventsVME;

  auto decoder = new LAPDecoderTask();
  decoder -> SetPadPersistency(true);
  decoder -> LoadData(pathToRawData, pathToMetaData);
  decoder -> SetNumEvents(numEventsInRun);
  run -> Add(decoder);

  auto noiseSubtraction = new LAPNoiseSubtractionTask();
  run -> Add(noiseSubtraction);

  auto psa = new KBPSATask();
  psa -> SetHitPersistency(true);
  psa -> SetPSA(new KBPSAFastFit);
  run -> Add(psa);

  run -> Add(new LAPBeamTrackingTask);

  run -> Init();
  run -> Run();
}
