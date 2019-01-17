#ifndef KBGLOBAL_HH
#define KBGLOBAL_HH

#include <iostream>
#include <fstream>
#include "KBParameterContainer.hh"

/**
 * Virtual class for stear classes of KEBI
*/

class KBGlobal
{
  public:
    KBGlobal() {};
    virtual ~KBGlobal() {};

    void CreateParameterContainer();
    void SetParameterContainer(KBParameterContainer *par);
    void SetParameterContainer(TString file);
    void AddParameterFile(TString file);
    KBParameterContainer *GetParameterContainer();

    virtual void SetRank(Int_t rank);
    Int_t GetRank();

  protected:
    KBParameterContainer *fPar = nullptr;

    Int_t fRank = 0;

  ClassDef(KBGlobal, 1)
};

#define kb_raw     KBLog(0,-1)
#define kb_out     KBLog(fRank,0)
#define kb_debug   KBLog(__FILE__,__LINE__)

#define kb_print   KBLog(fName,__FUNCTION__,fRank,1)
#define kb_info    KBLog(fName,__FUNCTION__,fRank,2)
#define kb_warning KBLog(fName,__FUNCTION__,fRank,3)
#define kb_error   KBLog(fName,__FUNCTION__,fRank,4)

#define kr_print(LRANK)   KBLog(this->ClassName(),__FUNCTION__,LRANK,1)
#define kr_info(LRANK)    KBLog(this->ClassName(),__FUNCTION__,LRANK,2)
#define kr_warning(LRANK) KBLog(this->ClassName(),__FUNCTION__,LRANK,3)
#define kr_error(LRANK)   KBLog(this->ClassName(),__FUNCTION__,LRANK,4)

class KBLog
{
  public:
    KBLog(TString name, const std::string &title ,int rank, int option)
    {
      if (option == -1)
        return;

      for (auto i=0; i<rank; ++i)
        std::cout << "  ";

      TString header = Form("[%s::%s] ", name.Data(), title.c_str());

      switch (option)
      {
        case 0:  break;
        case 1:  std::cout<<header; break;
        case 2:  std::cout<<header<<"\033[0;32m"<<"info "    <<"\033[0m"; break;
        case 3:  std::cout<<header<<"\033[0;33m"<<"warnning "<<"\033[0m"; break;
        case 4:  std::cout<<header<<"\033[0;31m"<<"error "   <<"\033[0m"; break;
        default: ;
      }
    }

    KBLog(int rank = 0, int option = 1)
    {
      if (option == -1)
        return;

      for (auto i=0; i<rank; ++i)
        std::cout << "  ";

      switch (option)
      {
        case 0:  break;
        case 1:  break;
        case 2:  std::cout<<"\033[0;32m"<<"info "    <<"\033[0m"; break;
        case 3:  std::cout<<"\033[0;33m"<<"warnning "<<"\033[0m"; break;
        case 4:  std::cout<<"\033[0;31m"<<"error "   <<"\033[0m"; break;
        default: ;
      }
    }

    KBLog(const std::string &title ,int line)
    {
      TString header = Form("+%d %s ", line, title.c_str());
      std::cout<<"\033[0;36m"<<"debug "<<"\033[0m "<<header;
    }

    template <class T> KBLog &operator<<(const T &v)
    {
      std::cout << v;
      return *this;
    }

    KBLog &operator<<(std::ostream&(*f)(std::ostream&))
    {
      std::cout << *f;
      return *this;
    }
};

#endif
