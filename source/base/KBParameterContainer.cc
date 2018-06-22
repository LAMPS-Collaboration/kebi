#include "TROOT.h"
#include "TSystem.h"
#include "TDirectory.h"
#include "TApplication.h"
#include "KBParameterContainer.hh"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>

using namespace std;

ClassImp(KBParameterContainer)

KBParameterContainer::KBParameterContainer(bool debug)
:TObjArray(), fDebugMode(debug)
{
  fName = "ParameterContainer";
}

KBParameterContainer::KBParameterContainer(const char *parName, bool debug)
:KBParameterContainer(debug)
{
  AddFile(TString(parName));
}

void KBParameterContainer::SetDebugMode(bool val) { fDebugMode = val; }

void KBParameterContainer::SaveAs(const char *filename, Option_t *option) const
{
  if (filename && strstr(filename,".par"))
  {
    cout << "Writting " << filename << " as parameter file." << endl;
    ofstream out(filename);
    out << "# " << filename << endl;
    out << "# created from method KBParameterContainer::SaveAs" << endl;
    out << endl;

    TIter iterator(this);
    TObject *obj;
    while ((obj = dynamic_cast<TObject*>(iterator())))
    {
      TString className = obj -> ClassName();

      if (className == "TNamed") {
        TNamed *par = (TNamed *) obj;
        TString key = par -> GetName();
        bool newpar = false;
        if (key.Index("NEWPAR")==0) {
          key.Remove(0,6);
          newpar = true;
        }
        TString value = par -> GetTitle();
        out << left << setw(25) << key << "  s  " << value;
        if (newpar)
          out << " # YOU MUST MODIFY THIS PARAMETER VALUE";
        out << endl;
      }
      else if (className == "TParameter<int>") {
        TParameter<Int_t> *par = (TParameter<Int_t> *) obj;
        TString key = par -> GetName();
        bool newpar = false;
        if (key.Index("NEWPAR")==0) {
          key.Remove(0,6);
          newpar = true;
        }
        Int_t value = par -> GetVal();
        out << left << setw(25) << key << "  i  " << value;
        if (newpar)
          out << " # YOU MUST MODIFY THIS PARAMETER VALUE";
        out << endl;
      }
      else if (className == "TParameter<double>") {
        TParameter<Double_t> *par = (TParameter<Double_t> *) obj;
        TString key = par -> GetName();
        bool newpar = false;
        if (key.Index("NEWPAR")==0) {
          key.Remove(0,6);
          newpar = true;
        }
        Double_t value = par -> GetVal();
        out << left << setw(25) << key << "  d  " << value;
        if (newpar)
          out << " # YOU MUST MODIFY THIS PARAMETER VALUE";
        out << endl;
      }
      else if (className == "TParameter<bool>") {
        TParameter<bool> *par = (TParameter<bool> *) obj;
        TString key = par -> GetName();
        bool newpar = false;
        if (key.Index("NEWPAR")==0) {
          key.Remove(0,6);
          newpar = true;
        }
        TString value = par -> GetVal() == true ? "true" : "false";
        out << left << setw(25) << key << "  b  " << value;
        if (newpar)
          out << " # YOU MUST MODIFY THIS PARAMETER VALUE";
        out << endl;
      }
    }
    return;
  }
  else
    TObject::SaveAs(filename, option);
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
      TString sval;
      ss >> sval;
      sval.ToLower();
      Bool_t val = false;
      if (sval == "true" || sval == "1" || sval == "ktrue")
        val = true;
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

void KBParameterContainer::Print(Option_t *option) const
{
  TIter iterator(this);

  TObject *obj;

  cout << "[" << fName << "]" << " List of parameters :" << endl;
  while ((obj = dynamic_cast<TObject*>(iterator()))) 
  {
    TString className = obj -> ClassName();

    if (className == "TNamed") {
      TNamed *par = (TNamed *) obj;
      TString key = par -> GetName();
      TString value = par -> GetTitle();
      cout << left << "  " << setw(25) << key << "  s  " << value << endl;
    }
    else if (className == "TParameter<int>") {
      TParameter<Int_t> *par = (TParameter<Int_t> *) obj;
      TString key = par -> GetName();
      Int_t value = par -> GetVal();
      cout << left << "  " << setw(25) << key << "  i  " << value << endl;
    }
    else if (className == "TParameter<double>") {
      TParameter<Double_t> *par = (TParameter<Double_t> *) obj;
      TString key = par -> GetName();
      Double_t value = par -> GetVal();
      cout << left << "  " << setw(25) << key << "  d  " << value << endl;
    }
    else if (className == "TParameter<bool>") {
      TParameter<bool> *par = (TParameter<bool> *) obj;
      TString key = par -> GetName();
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

bool KBParameterContainer::SetPar(TString name, const char* val)
{
  return SetPar(name, TString(val));
}

bool KBParameterContainer::GetParBool(TString name)
{
  TObject *obj = FindObject(name);
  if (obj == nullptr) {
    cout << "[KBParameterContainer] parameter with name " << name << " does not exist!" << endl;
    if (fDebugMode)
      SetPar(TString("NEWPAR")+name, false);
    else
      gApplication -> Terminate();
    return false;
  }

  return ((TParameter<Bool_t> *) obj) -> GetVal();
}

Int_t KBParameterContainer::GetParInt(TString name)
{
  TObject *obj = FindObject(name);
  if (obj == nullptr) {
    cout << "[KBParameterContainer] parameter with name " << name << " does not exist!" << endl;
    if (fDebugMode)
      SetPar(TString("NEWPAR")+name, -999);
    else
      gApplication -> Terminate();
    return -999;
  }

  return ((TParameter<Int_t> *) obj) -> GetVal();
}

Double_t KBParameterContainer::GetParDouble(TString name)
{
  TObject *obj = FindObject(name);
  if (obj == nullptr) {
    cout << "[KBParameterContainer] parameter with name " << name << " does not exist!" << endl;
    if (fDebugMode)
      SetPar(TString("NEWPAR")+name, -999.999);
    else
      gApplication -> Terminate();
    return -999.999;
  }

  return ((TParameter<Double_t> *) obj) -> GetVal();
}

TString KBParameterContainer::GetParString(TString name)
{
  TObject *obj = FindObject(name);
  if (obj == nullptr) {
    cout << "[KBParameterContainer] parameter with name " << name << " does not exist!" << endl;
    if (fDebugMode)
      SetPar(TString("NEWPAR")+name, TString("DOES_NOT_EXIST"));
    else
      gApplication -> Terminate();
    return "DOES_NOT_EXIST";
  }

  return ((TNamed *) obj) -> GetTitle();
}
