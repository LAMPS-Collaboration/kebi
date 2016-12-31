#ifndef KBTASK_HH
#define KBTASK_HH

#include "TTask.h"

class KBTask : public TTask
{
  public:
    KBTask();
    KBTask(const char* name, const char *title);
    virtual ~KBTask() {};

    bool InitTask();

  protected:
    virtual bool Init();
    bool InitTasks();

  ClassDef(KBTask, 1)
};

#endif
