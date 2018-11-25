#ifndef KBCONTAINER_HH
#define KBCONTAINER_HH

#include "TObject.h"
#ifdef ACTIVATE_EVE
#include "TEveElement.h"
#endif

class KBContainer : public TObject
{
  public:
    KBContainer();
    virtual ~KBContainer();

    virtual void Clear(Option_t *option = "");
    virtual void Copy (TObject &object) const;

#ifdef ACTIVATE_EVE
    virtual bool DrawByDefault()              { return false; } ///< return true if to be displayed on eve.
    virtual bool IsEveSet()                   { return false; } ///< Check if this element should be a "set" of TEveElements (e.g. TEvePointSet, TEveStraightLineSet)
    virtual TEveElement *CreateEveElement()   { return nullptr; } ///< Create TEveElement
    virtual void SetEveElement(TEveElement *) {} ///< Set TEveElement. For when IsEveSet() is false.
    virtual void AddToEveSet(TEveElement *)   {} ///< Add TEveElement to this eve-set
#endif

  ClassDef(KBContainer, 3)
};

#define kc_raw     KBLogC(this->ClassName(),__FUNCTION__,-1)
#define kc_out     KBLogC(this->ClassName(),__FUNCTION__,0)
#define kc_print   KBLogC(this->ClassName(),__FUNCTION__,1)
#define kc_info    KBLogC(this->ClassName(),__FUNCTION__,2)
#define kc_warning KBLogC(this->ClassName(),__FUNCTION__,3)
#define kc_error   KBLogC(this->ClassName(),__FUNCTION__,4)
#define kc_debug   KBLogC(__FILE__,__LINE__)

#include <iostream>

class KBLogC
{
  /// Logger class for containers
  public:
    KBLogC(TString name, const std::string &title, int option)
    {
      if (option == -1)
        return;

      //for (auto i=0; i<rank; ++i)
      //  std::cout << "  ";

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

    KBLogC(const std::string &title ,int line)
    {
      TString header = Form("+%d %s ", line, title.c_str());
      std::cout<<"\033[0;36m"<<"debug "<<"\033[0m "<<header;
    }

    template <class T> KBLogC &operator<<(const T &v)
    {
      std::cout << v;
      return *this;
    }

    KBLogC &operator<<(std::ostream&(*f)(std::ostream&))
    {
      std::cout << *f;
      return *this;
    }
};

#endif
