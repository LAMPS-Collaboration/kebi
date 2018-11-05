#include "KBContainer.hh"

KBContainer::KBContainer()
{
}

KBContainer::~KBContainer()
{
}

void KBContainer::Clear(Option_t *)
{
  fMCID = -1;
  fMCError = -1;
}

void KBContainer::Copy(TObject &obj) const
{
  TObject::Copy(obj);
  auto container = (KBContainer &) obj;

  container.SetMCID(fMCID, fMCError, fMCPurity);
}
