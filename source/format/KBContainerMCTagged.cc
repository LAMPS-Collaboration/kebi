#include "KBContainerMCTagged.hh"

KBContainerMCTagged::KBContainerMCTagged()
{
}

KBContainerMCTagged::~KBContainerMCTagged()
{
}

void KBContainerMCTagged::Clear(Option_t *option)
{
  KBContainer::Clear(option);
}

void KBContainerMCTagged::Copy(TObject &obj) const
{
  KBContainer::Copy(obj);
  auto container = (KBContainerMCTagged &) obj;

  container.SetMCID(fMCID, fMCError, fMCPurity);
}
