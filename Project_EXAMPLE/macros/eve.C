Int_t fEventID = 0;

void nx(Int_t eventID = -1) {
  if (eventID < 0) KBRun::GetRun() -> RunEve(++fEventID);
  else KBRun::GetRun() -> RunEve(fEventID = eventID);
  cout << "Event " << fEventID << endl;
}

void eve(TString name = "dummy.mc")
{
  auto run = new KBRun();
  run -> SetInputFile(name);
  run -> SetTag("eve");
  run -> AddParameterFile("dummy.par");
  run -> AddDetector(new EXDetector());
  run -> Init();
  run -> SetGeoTransparency(80);
  run -> RunEve(fEventID);
}
