Int_t fEventID = 0;

void nx(Int_t eventID = -1) {
  if (eventID < 0) KBRun::GetRun() -> RunEve(++fEventID);
  else KBRun::GetRun() -> RunEve(fEventID = eventID);
  cout << "Event " << fEventID << endl;
}

void eve(TString name = "tpc.reco")
{
  auto run = new KBRun();
  run -> SetInputFile(name);
  run -> AddDetector(new LATpc());
  run -> AddParameterFile("lamps.par");
  run -> Init();
  run -> SetGeoTransparency(80);
  run -> RunEve(fEventID);
}
