#include "TSystem.h"
#include "TApplication.h"
#include "KBParameterContainer.hh"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>

using namespace std;

ClassImp(KBParameterContainer)

KBParameterContainer::KBParameterContainer()
:TObjArray()
{
  fName = "ParameterContainer";
}

KBParameterContainer::KBParameterContainer(TString parName)
:KBParameterContainer()
{
  AddFile(parName);
}

void KBParameterContainer::ReplaceEnvironmentVariable(TString &val)
{
  if (val[0] == '$') {
    TString env = val;
    Ssiz_t nenv = env.First("/");
    env.Resize(nenv);
    env.Remove(0, 1);
    TString path = getenv(env);
    val.Replace(0, nenv, path);
  }
}

Int_t KBParameterContainer::AddFile(TString fileName, TString parNameForFile)
{
  TString fileNameFull;

  if (fileName[0] != '/' && fileName[0] != '$' && fileName != '~')
    fileNameFull = TString(gSystem -> Getenv("KEBIPATH")) + "/input/" + fileName;
  else
    fileNameFull = fileName;

  ReplaceEnvironmentVariable(fileNameFull);

  if (TString(gSystem -> Which(".", fileNameFull.Data())).IsNull())
    fileNameFull = TString(gSystem -> Getenv("PWD")) + "/" + fileName;

  if (TString(gSystem -> Which(".", fileNameFull.Data())).IsNull()) {
    cout << "[KBParameterContainer] Parameter file " << fileNameFull << " does not exist!" << endl;
    return 0;
  }
  cout << "[KBParameterContainer] Adding parameter file " << fileNameFull << endl;

  if (parNameForFile.IsNull())
    parNameForFile = Form("INPUT_PARAMETER_FILE%d", fNumInputFiles);
  fNumInputFiles++;
  SetPar(parNameForFile, fileNameFull);

  Int_t countParameters = 0;

  ifstream file(fileNameFull);
  string line;

  while (getline(file, line)) {
    if (line.find("#") == 0)
      continue;

    countParameters++;

    TString parName;
    TString parType;

    istringstream ss(line);
    ss >> parName >> parType;

    if (parType == "f" || parType == "file" || parType == "FILE") {
      TString val;
      ss >> val;
      ReplaceEnvironmentVariable(val);
      AddFile(val, parName);
    }
    else if (parType == "b" || parType == "bool" || parType == "Bool_t") {
      Bool_t val;
      ss >> val;
      SetPar(parName, val);
    }
    else if (parType == "i" || parType == "int" || parType == "Int_t") {
      Int_t val;
      ss >> val;
      SetPar(parName, val);
    }
    else if (parType == "d" || parType == "double" || parType == "Double_t") {
      Double_t val;
      ss >> val;
      SetPar(parName, val);
    }
    else if (parType == "s" || parType == "TString") {
      TString val;
      ss >> val;
      ReplaceEnvironmentVariable(val);
      SetPar(parName, val);
    }
    else
      countParameters--;
  }

  if (countParameters == 0) {
    this -> Remove(FindObject(parNameForFile));
    fNumInputFiles--;
  }

  return countParameters;
}

Int_t KBParameterContainer::GetNumInputFiles() { return fNumInputFiles; }

void KBParameterContainer::Print(Option_t *) const
{
  TIter iterator(this);

  TObject *obj;

  cout << "[" << fName << "]" << " List of parameters :" << endl;
  while ((obj = dynamic_cast<TObject*>(iterator()))) 
  {
    TString className = obj -> ClassName();

    if (className == "TNamed") {
      auto par = (TNamed *) obj;
      auto key = par -> GetName();
      auto value = par -> GetTitle();
      cout << left << "  " << setw(25) << key << "  s  " << value << endl;
    }
    else if (className == "TParameter<int>") {
      auto par = (TParameter<Int_t> *) obj;
      auto key = par -> GetName();
      Int_t value = par -> GetVal();
      cout << left << "  " << setw(25) << key << "  i  " << value << endl;
    }
    else if (className == "TParameter<double>") {
      auto par = (TParameter<double> *) obj;
      auto key = par -> GetName();
      Double_t value = par -> GetVal();
      cout << left << "  " << setw(25) << key << "  d  " << value << endl;
    }
    else if (className == "TParameter<bool>") {
      auto par = (TParameter<bool> *) obj;
      auto key = par -> GetName();
      TString value = par -> GetVal() == true ? "true" : "false";
      cout << left << "  " << setw(25) << key << "  b  " << value << endl;
    }
  }
}


bool KBParameterContainer::SetPar(TString name, Bool_t val)
{
  if (FindObject(name) != nullptr) {
    cout << "[KBParameterContainer] Parameter with name " << name << " already exist!" << endl;
    return false;
  }

  Add(new TParameter<Bool_t>(name, val));
  return true;
}

bool KBParameterContainer::SetPar(TString name, Int_t val)
{
  if (FindObject(name) != nullptr) {
    cout << "[KBParameterContainer] Parameter with name " << name << " already exist!" << endl;
    return false;
  }

  Add(new TParameter<Int_t>(name, val));
  return true;
}

bool KBParameterContainer::SetPar(TString name, Double_t val)
{
  if (FindObject(name) != nullptr) {
    cout << "[KBParameterContainer] Parameter with name " << name << " already exist!" << endl;
    return false;
  }

  Add(new TParameter<Double_t>(name, val));
  return true;
}

bool KBParameterContainer::SetPar(TString name, TString val)
{
  if (FindObject(name) != nullptr) {
    cout << "[KBParameterContainer] Parameter with name " << name << " already exist!" << endl;
    return false;
  }

  Add(new TNamed(name, val));
  return true;
}

bool KBParameterContainer::GetParBool(TString name, Bool_t &val, bool exitElse) const
{
  TObject *obj = FindObject(name);
  if (obj == nullptr) {
    cout << "[KBParameterContainer] parameter with name " << name << " no not exist!" << endl;
    if (exitElse)
      gApplication -> Terminate();
    return false;
  }

  val = ((TParameter<Bool_t> *) obj) -> GetVal();
  return true;
}

bool KBParameterContainer::GetParInt(TString name, Int_t &val, bool exitElse) const
{
  TObject *obj = FindObject(name);
  if (obj == nullptr) {
    cout << "[KBParameterContainer] parameter with name " << name << " no not exist!" << endl;
    if (exitElse)
      gApplication -> Terminate();
    return false;
  }

  val = ((TParameter<Int_t> *) obj) -> GetVal();
  return true;
}

bool KBParameterContainer::GetParDouble(TString name, Double_t &val, bool exitElse) const
{
  TObject *obj = FindObject(name);
  if (obj == nullptr) {
    cout << "[KBParameterContainer] parameter with name " << name << " no not exist!" << endl;
    if (exitElse)
      gApplication -> Terminate();
    return false;
  }

  val = ((TParameter<Double_t> *) obj) -> GetVal();
  return true;
}

bool KBParameterContainer::GetParString(TString name, TString &val, bool exitElse) const
{
  TObject *obj = FindObject(name);
  if (obj == nullptr) {
    cout << "[KBParameterContainer] parameter with name " << name << " no not exist!" << endl;
    if (exitElse)
      gApplication -> Terminate();
    return false;
  }

  val = ((TNamed *) obj) -> GetTitle();
  return true;
}
