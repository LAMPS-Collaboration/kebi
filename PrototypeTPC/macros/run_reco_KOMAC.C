void run_reco_KOMAC(Int_t runID = 36, Int_t numberOfEvents = 100)
{
  auto run = new KBRun();
  run -> SetRunName("komac",runID);
  run -> SetTag("reco");
  run -> AddPar("prototype_KOMAC.par");
  run -> AddDetector(new LAPTpc());
  run -> SetNumEvents(numberOfEvents);

  auto decoder = new LAPDecoderFromRootTask();
  decoder -> SetPadPersistency(true);

  auto noiseSubtraction = new LAPNoiseSubtractionTask();
  noiseSubtraction -> SkipNoiseSubtraction(true);

  auto psa = new KBPSATask();
  psa -> SetHitPersistency(true);

  //psa -> SetPSA(new KBPSAFastFit); // pulse fit
  psa -> SetPSA(new KBPSA); // find largest peak

  run -> Add(decoder);
  run -> Add(noiseSubtraction);
  run -> Add(psa);

  run -> Init();
  run -> Run();
}
