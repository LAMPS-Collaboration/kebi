#ifndef KBPARAMETERCONTAINER
#define KBPARAMETERCONTAINER

#include "TObjArray.h"
#include "TNamed.h"
#include "TParameter.h"

/**
 * List of parameters <[parameter name], [parameter values]>
 * parameter type features Bool_t, Int_t, Double_t, TString.
 *
 * Structure of parameter file should be list of : [name] [type_initial] [value]
 * Each elemets are divided by space.
 * Comments are used by #.
 *
 * @param name Name of parameter file with no space
 * @param type_initial
 *   - b for Bool_t
 *   - i for Int_t
 *   - d for Double_t
 *   - s for TString
 * @param value Value of parameter. TString value do not accept space.
 *
 * ex)\n
 * \#example parameter file\n
 * worldSize    d  1000   # [mm]\n
 * nTbs         n  512\n
 * specialFile  s  /path/to/specialFile.dat  \#special\n 
*/

class KBParameterContainer : public TObjArray
{
  public:
    KBParameterContainer();
    KBParameterContainer(TString parName); ///< Constructor with input parameter file name
    virtual ~KBParameterContainer() {}

    virtual void Print(Option_t *option="") const; ///< Print list of parameters

    /**
     * Add parameter by given fileName.
     * If fileName do not include path, file will be searched in path/to/KEBI/input.
     *
     * fileName will also be registered as parameter. 
     * If parNameForFile is not set, parameter name will be set as 
     * INPUT_PARAMETER_FILE[fNumInputFiles]
    */
    Int_t AddFile(TString fileName, TString parNameForFile = "");
    Int_t GetNumInputFiles(); ///< Get number of input parameter files

    bool SetPar(TString name, Bool_t val);   ///< Set Bool_t type parameter with given name
    bool SetPar(TString name, Int_t val);    ///< Set Int_t type parameter with given name
    bool SetPar(TString name, Double_t val); ///< Set Double_t type parameter with given name
    bool SetPar(TString name, TString val);  ///< Set TString type parameter with given name

    bool GetParBool(TString name, Bool_t &val, bool exitElse = true)     const; ///< Get Bool_t type parameter to val with given name. Terminate if parameter do not exist.
    bool GetParInt(TString name, Int_t &val, bool exitElse = true)       const; ///< Get Int_t type parameter to val with given name. Terminate if parameter do not exist.
    bool GetParDouble(TString name, Double_t &val, bool exitElse = true) const; ///< Get Double_t type parameter to val with given name. Terminate if parameter do not exist.
    bool GetParString(TString name, TString &val, bool exitElse = true)  const; ///< G get TString type parameter to val with given name. Terminate if parameter do not exist.

    void ReplaceEnvironmentVariable(TString &val);

  private:
    Int_t fNumInputFiles = 0;

  ClassDef(KBParameterContainer, 1)
};

#endif
