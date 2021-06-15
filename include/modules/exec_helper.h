#if !defined(EXEC_HELPER_H)
#define EXEC_HELPER_H

#include "task.h"
#include "config.h"

int exec_full(Config_Server cs, Task t);

int exec_partial(Config_Server cs, Task t);

#endif // EXEC_HELPER_H
