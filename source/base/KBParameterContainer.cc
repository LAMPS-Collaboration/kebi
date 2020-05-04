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

KBParameterContainer::KBParameterContainer(Bool_t debug)
:TObjArray(), fDebugMode(debug)
{
  fName = "ParameterContainer";
}

KBParameterContainer::KBParameterContainer(const char *parName, Bool_t debug)
:KBParameterContainer(debug)
{
  AddFile(TString(parName));
}

void KBParameterContainer::SetDebugMode(Bool_t val) { fDebugMode = val; }

void KBParameterContainer::SaveAs(const char *filename, Option_t *option) const
{
  //if (filename && strstr(filename,".par"))
  if (filename && strstr(filename,".conf"))
  {
    kr_info(0) << "Writting " << filename << " as parameter file." << endl;
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
        Bool_t newpar = false;
        if (key.Index("NEWPAR")==0) {
          key.Remove(0,6);
          newpar = true;
        }
        TString value = par -> GetTitle();
        if (value.Index("AXIS_PARAMETER_")==0) {
          value.ReplaceAll("AXIS_PARAMETER_","");
          out << left << setw(25) << key << "  a  " << value;
        }
        else
          out << left << setw(25) << key << "  s  " << value;
        if (newpar)
          out << " # YOU MUST MODIFY THIS PARAMETER VALUE";
        out << endl;
      }
      else if (className == "TParameter<int>") {
        TParameter<Int_t> *par = (TParameter<Int_t> *) obj;
        TString key = par -> GetName();
        Bool_t newpar = false;
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
        Bool_t newpar = false;
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
        TParameter<Bool_t> *par = (TParameter<Bool_t> *) obj;
        TString key = par -> GetName();
        Bool_t newpar = false;
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
    kr_info(0) << "Parameter file " << fileNameFull << " does not exist!" << endl;
    return 0;
  }
  kr_info(0) << "Adding parameter file " << fileNameFull << endl;

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
    parType.ToLower();

    Bool_t overwrite = false;
    if (parType == "o" || parType == "overwrite") {
      overwrite = true;
      ss >> parType;
      parType.ToLower();
    }

    if (parType == "f" || parType == "file") {
      TString val;
      ss >> val;
      ReplaceEnvironmentVariable(val);
      AddFile(val, parName);
    }
    else if (parType == "b" || parType == "bool" || parType == "bool_t") {
      TString sval;
      ss >> sval;
      sval.ToLower();
      Bool_t val = false;
      if (sval == "true" || sval == "1" || sval == "ktrue")
        val = true;
      SetPar(parName, val, overwrite);
    }
    else if (parType == "i" || parType == "int" || parType == "int_t" || parType == "w" || parType == "width" || parType == "width_t")
    {
      Int_t val;
      ss >> val;
      SetPar(parName, val, overwrite);
    }
    else if (parType == "c" || parType == "color" || parType == "color_t") {
      TString val;
      ss >> val;
      SetParColor(parName, val, overwrite);
    }
    else if (parType == "d" || parType == "double" || parType == "double_t" || parType == "size" || parType == "size_t" ) {
      Double_t val;
      ss >> val;
      SetPar(parName, val, overwrite);
    }
    else if (parType == "s" || parType == "tstring") {
      TString val;
      ss >> val;
      ReplaceEnvironmentVariable(val);
      SetPar(parName, val, overwrite);
    }
    else if (parType == "a" || parType == "axis" || parType == "kbvector3::axis") {
      TString val;
      ss >> val;
      if (val.Index("AXIS_PARAMETER_")<0)
        val = TString("AXIS_PARAMETER_") + val;
      SetPar(parName, val, overwrite);
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

Int_t KBParameterContainer::AddPar(KBParameterContainer *parc, TString parNameForFile)
{
  kr_info(0) << "Adding parameter container " << parc -> GetName() << endl;

  if (parNameForFile.IsNull())
    parNameForFile = Form("INPUT_PAR_CONTAINER%d", fNumInputFiles);
  fNumInputFiles++;
  SetPar(parNameForFile, ""); //@todo

  Int_t countParameters = 0;
  Int_t countSameParameters = 0;

  TIter iterator(parc);
  TObject *obj;
  while ((obj = dynamic_cast<TObject*>(iterator())))
  {
    TString name = obj -> GetName();

    TObject *found = FindObject(name);
    if (found != nullptr) {
      kr_error(0) << "Parameter with name " << name << " already exist!" << endl;
      ++countSameParameters ;
    }
    else {
      Add(obj);
      ++countParameters;
    }
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

  kr_info(0) << "[" << fName << "]" << " List of parameters :" << endl;
  while ((obj = dynamic_cast<TObject*>(iterator()))) 
  {
    TString className = obj -> ClassName();

    if (className == "TNamed") {
      TNamed *par = (TNamed *) obj;
      TString key = par -> GetName();
      TString value = par -> GetTitle();
      if (value.Index("AXIS_PARAMETER_")==0) {
        value.ReplaceAll("AXIS_PARAMETER_","");
        kr_info(0) << left << "  " << setw(25) << key << "  a  " << value << endl;
      }
      else
        kr_info(0) << left << "  " << setw(25) << key << "  s  " << value << endl;
    }
    else if (className == "TParameter<int>") {
      TParameter<Int_t> *par = (TParameter<Int_t> *) obj;
      TString key = par -> GetName();
      Int_t value = par -> GetVal();
      kr_info(0) << left << "  " << setw(25) << key << "  i  " << value << endl;
    }
    else if (className == "TParameter<double>") {
      TParameter<Double_t> *par = (TParameter<Double_t> *) obj;
      TString key = par -> GetName();
      Double_t value = par -> GetVal();
      kr_info(0) << left << "  " << setw(25) << key << "  d  " << value << endl;
    }
    else if (className == "TParameter<bool>") {
      TParameter<Bool_t> *par = (TParameter<Bool_t> *) obj;
      TString key = par -> GetName();
      TString value = par -> GetVal() == true ? "true" : "false";
      kr_info(0) << left << "  " << setw(25) << key << "  b  " << value << endl;
    }
  }
}


Bool_t KBParameterContainer::SetPar(TString name, Bool_t val, Bool_t overwrite)
{
  if (FindObject(name) != nullptr) {
    if (overwrite)
      this -> Remove(FindObject(name));
    else {
      kr_error(0) << "Parameter with name " << name << " already exist!" << endl;
      return false;
    }
  }

  Add(new TParameter<Bool_t>(name, val));
  return true;
}

Bool_t KBParameterContainer::SetPar(TString name, Int_t val, Bool_t overwrite)
{
  if (FindObject(name) != nullptr) {
    if (overwrite)
      this -> Remove(FindObject(name));
    else {
      kr_error(0) << "Parameter with name " << name << " already exist!" << endl;
      return false;
    }
  }

  Add(new TParameter<Int_t>(name, val));
  return true;
}

Bool_t KBParameterContainer::SetPar(TString name, Double_t val, Bool_t overwrite)
{
  if (FindObject(name) != nullptr) {
    if (overwrite)
      this -> Remove(FindObject(name));
    else {
      kr_error(0) << "Parameter with name " << name << " already exist!" << endl;
      return false;
    }
  }

  Add(new TParameter<Double_t>(name, val));
  return true;
}

Bool_t KBParameterContainer::SetPar(TString name, TString val, Bool_t overwrite)
{
  if (FindObject(name) != nullptr) {
    if (overwrite)
      this -> Remove(FindObject(name));
    else {
      kr_error(0) << "Parameter with name " << name << " already exist!" << endl;
      return false;
    }
  }

  Add(new TNamed(name, val));
  return true;
}

Bool_t KBParameterContainer::SetParColor(TString name, TString valColor, Bool_t overwrite)
{
  if (FindObject(name) != nullptr) {
    if (overwrite)
      this -> Remove(FindObject(name));
    else {
      kr_error(0) << "Parameter with name " << name << " already exist!" << endl;
      return false;
    }
  }

  Int_t val = 0;
  if (valColor.IsDec()) {
    val = valColor.Atoi();
  }
  else if (valColor.Index("k")==0) {
    valColor.ReplaceAll("kWhite"  ,"0");
    valColor.ReplaceAll("kBlack"  ,"1");
    valColor.ReplaceAll("kGray"   ,"920");
    valColor.ReplaceAll("kRed"    ,"632");
    valColor.ReplaceAll("kGreen"  ,"416");
    valColor.ReplaceAll("kBlue"   ,"600");
    valColor.ReplaceAll("kYellow" ,"400");
    valColor.ReplaceAll("kMagenta","616");
    valColor.ReplaceAll("kCyan"   ,"432");
    valColor.ReplaceAll("kOrange" ,"800");
    valColor.ReplaceAll("kSpring" ,"820");
    valColor.ReplaceAll("kTeal"   ,"840");
    valColor.ReplaceAll("kAzure"  ,"860");
    valColor.ReplaceAll("kViolet" ,"880");
    valColor.ReplaceAll("kPink"   ,"900");

    auto valArray = valColor.Tokenize("+");
    int val1 = (((TObjString *) valArray->At(0)) -> GetString()).Atoi();
    int val2 = 0;
    if (valArray->GetEntriesFast() > 1)
      val2 = (((TObjString *) valArray->At(1)) -> GetString()).Atoi();
    val = val1 + val2;
  }
  else
    return false;

  Add(new TParameter<Int_t>(name, val));
  return true;
}

Bool_t KBParameterContainer::SetPar(TString name, const char* val, Bool_t overwrite)
{
  return SetPar(name, TString(val), overwrite);
}

Bool_t KBParameterContainer::GetParBool(TString name)
{
  TObject *obj = FindObject(name);
  if (obj == nullptr) {
    kr_error(0) << "parameter with name " << name << " does not exist!" << endl;
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
    kr_error(0) << "parameter with name " << name << " does not exist!" << endl;
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
    kr_error(0) << "parameter with name " << name << " does not exist!" << endl;
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
    kr_error(0) << "parameter with name " << name << " does not exist!" << endl;
    if (fDebugMode)
      SetPar(TString("NEWPAR")+name, TString("DOES_NOT_EXIST"));
    else
      gApplication -> Terminate();
    return "DOES_NOT_EXIST";
  }

  return ((TNamed *) obj) -> GetTitle();
}

KBVector3::Axis KBParameterContainer::GetParAxis(TString name)
{
  TObject *obj = FindObject(name);

  if (obj != nullptr) {
    TString value = ((TNamed *) obj) -> GetTitle();
    if (value.Index("AXIS_PARAMETER_")==0) {
      value.ReplaceAll("AXIS_PARAMETER_","");
      return KBVector3::GetAxis(value);
    }
  }

  kr_error(0) << "parameter with name " << name << " does not exist!" << endl;
  if (fDebugMode)
    SetPar(TString("NEWPAR")+name, TString("AXIS_PARAMETER_DOES_NOT_EXIST"));
  else
    gApplication -> Terminate();
  return KBVector3::kNon;
}

Bool_t KBParameterContainer::CheckPar(TString name)
{
  if (FindObject(name) == nullptr)
    return false;
  return true;
}
