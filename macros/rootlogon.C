{
  SysInfo_t info;
  gSystem -> GetSysInfo(&info);
  TString osString = info.fOS;

  TString libString;
  if (osString.Index("Darwin") >= 0)
    libString = TString(gSystem -> Getenv("KEBIPATH")) + "/build/libKEBI.dylib";
  else if (osString.Index("Linux") >= 0)
    libString = TString(gSystem -> Getenv("KEBIPATH")) + "/build/libKEBI.so";

  if (gSystem -> Load(libString) != -1)
    cout << "Hello KEBI!" << endl;
  else
    cout << "Cannot load KEBI" << endl;
}
