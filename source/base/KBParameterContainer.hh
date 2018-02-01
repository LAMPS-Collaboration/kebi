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
 * nTbs         i  512\n
 * specialFile  s  /path/to/specialFile.dat  \#special\n 
 *
 * ============================================================================
 *
 * With fDebugMode true, KBParameterContainer will not terminate at attempt of
 * getting non-existing paramter, but print message and create empty parameter.
 *
*/

class KBParameterContainer : public TObjArray
{
  public:
    KBParameterContainer(bool debug = false);
    KBParameterContainer(const char *parName, bool debug = false); ///< Constructor with input parameter file name
    virtual ~KBParameterContainer() {}

    void SaveAs(const char *filename, Option_t *option = "") const;

    void SetDebugMode(bool val = true);

    virtual void Print(Option_t *option ="") const;

    /**
     * Add parameter by given fileName.
     * If fileName does not include path, file will be searched in path/to/KEBI/input.
     *
     * fileName will also be registered as parameter. 
     * If parNameForFile is not set, parameter name will be set as 
     * INPUT_PARAMETER_FILE[fNumInputFiles]
    */
    virtual Int_t AddFile(TString fileName, TString parNameForFile = "");
    Int_t GetNumInputFiles(); ///< Get number of input parameter files

    bool SetPar(TString name, Bool_t val);   ///< Set Bool_t   type parameter with given name
    bool SetPar(TString name, Int_t val);    ///< Set Int_t    type parameter with given name
    bool SetPar(TString name, Double_t val); ///< Set Double_t type parameter with given name
    bool SetPar(TString name, TString val);  ///< Set TString  type parameter with given name

    bool     GetParBool(TString name);   ///< Get Bool_t   type parameter by given name. Terminate if (parameter does-not-exist && fDebugMode == false).
    Int_t    GetParInt(TString name);    ///< Get Int_t    type parameter by given name. Terminate if (parameter does-not-exist && fDebugMode == false).
    Double_t GetParDouble(TString name); ///< Get Double_t type parameter by given name. Terminate if (parameter does-not-exist && fDebugMode == false).
    TString  GetParString(TString name); ///< Get TString  type parameter by given name. Terminate if (parameter does-not-exist && fDebugMode == false).

    void ReplaceEnvironmentVariable(TString &val);

  private:
    bool fDebugMode = false;
    Int_t fNumInputFiles = 0;

  ClassDef(KBParameterContainer, 1)
};

#endif
