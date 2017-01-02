void reco(Int_t runID = 101)
{
  auto run = new KBRun();
  run -> SetRunID(runID);
  run -> SetOutputFile(Form("run%04d.reco",runID));
  run -> AddParameterFile("LAP.par");
  run -> AddDetector(new LAPTpc());

  auto decoder = new LAPDecoderTask();
  decoder -> ReadDirectory("/home/storage/LAMPS_TPC_Prototype_ELPH_data/corrected");

  auto psa = new KBPSATask();

  auto noiseSubtraction = new LAPNoiseSubtractionTask();

  run -> Add(decoder);
  run -> Add(noiseSubtraction);
  run -> Add(psa);

  run -> Init();
  run -> RunSingle(0);
}
