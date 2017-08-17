void decoding(Int_t runID = 101, TString pathToData = "/home/lamps/ELPH_data2/")
{
  auto run = new KBRun();
  run -> SetRunID(runID);
  run -> SetTag("decoder_test");
  run -> AddParameterFile("prototype.par");
  run -> AddDetector(new LAPTpc());

  auto decoder = new LAPDecoderTask();
  decoder -> SetPadPersistency(true);
  decoder -> ReadDirectory(pathToData);
  run -> Add(decoder);

  run -> Init();
  run -> RunInRange(0,9);
  //run -> RunSingle(0);
}
