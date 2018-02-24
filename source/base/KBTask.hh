#ifndef KBTASK_HH
#define KBTASK_HH

#include "TTask.h"
#include <string>
#include <iostream>
using namespace std;

class KBTask : public TTask
{
  public:
    KBTask();
    KBTask(const char* name, const char *title);
    virtual ~KBTask() {};

    virtual void Add(TTask *task);

    void SetRank(Int_t rank);
    Int_t GetRank();

    bool InitTask();
    bool InitTasks();
    virtual bool Init();

  protected:
    Int_t fRank = 0;

    ClassDef(KBTask, 1)
};

#define kb_raw     KBLog(fName,__FUNCTION__,fRank,-1)
#define kb_out     KBLog(fName,__FUNCTION__,fRank,0)
#define kb_print   KBLog(fName,__FUNCTION__,fRank,1)
#define kb_info    KBLog(fName,__FUNCTION__,fRank,2)
#define kb_warning KBLog(fName,__FUNCTION__,fRank,3)
#define kb_error   KBLog(fName,__FUNCTION__,fRank,4)
#define kb_debug   KBLog(__FILE__,__LINE__)

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
