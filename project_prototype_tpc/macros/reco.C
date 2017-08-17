void reco(Int_t runID = 101, Int_t split = 0)
{
  auto run = new KBRun();
  run -> SetRunID(runID); run -> SetTag("reco");
  run -> SetSplit(split, 5);
  run -> AddParameterFile("prototype.par");
  run -> AddDetector(new LAPTpc());

  auto decoder = new LAPDecoderTask();
  decoder -> SetPadPersistency(true);
  decoder -> ReadDirectory("/home/lamps/ELPH_data/");
  run -> Add(decoder);

  auto noiseSubtraction = new LAPNoiseSubtractionTask();
  run -> Add(noiseSubtraction);

  auto psa = new KBPSATask();
  psa -> SetHitPersistency(true);
  psa -> SetPSA(new KBPSA);
  run -> Add(psa);

  run -> Add(new LAPBeamTrackingTask);

  run -> Init();
  run -> Run();
}
