#include "KBTask.hh"

#include <iostream>
using namespace std;

KBTask::KBTask()
:TTask()
{
}

KBTask::KBTask(const char* name, const char *title)
:TTask(name, title)
{
}

void KBTask::Add(TTask *task)
{
  TTask::Add(task);

  auto kbtask = (KBTask *) task;
  kbtask -> SetRank(fRank+1);
}

bool KBTask::InitTask() 
{ if (!fActive) 
    return false;

  bool initialized = Init();
  if (!initialized)
    return false;

  return InitTasks();
}

bool KBTask::Init() 
{
  return true;
}

bool KBTask::InitTasks()
{
  TIter iter(GetListOfTasks());
  KBTask* task;

  while ( (task = dynamic_cast<KBTask*>(iter())) ) {
    cout << "  " << "Initializing " << task -> GetName() << "." << endl;
    if (task -> Init() == false) {
      cout << "  Initialization failed!" << endl;
      return false;
    }
  }

  return true;
}

void KBTask::SetRank(Int_t rank)
{
  fRank = rank;

  TIter iter(GetListOfTasks());
  KBTask* task;
  while ( (task = dynamic_cast<KBTask*>(iter())) )
    task -> SetRank(fRank+1);
}

Int_t KBTask::GetRank() { return fRank; }
