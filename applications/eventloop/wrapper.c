#include <glib-unix.h>
#include <glib.h>
#include <sched.h> // scheduler.
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "wrapper.h"

static gboolean myloop(void *user_data __attribute__((unused))) {
  loop();
  return G_SOURCE_CONTINUE;
}

int main(int argc __attribute__((unused)),
         char **argv __attribute__((unused))) {

  struct sched_param param = {
      0,
  };
  param.sched_priority = sched_get_priority_max(SCHED_RR);
  if (sched_setscheduler(0, SCHED_RR, &param) != 0) {
    fprintf(stderr, "Failed to set realtime priority: %s\r\n", strerror(errno));
  }

  GMainLoop *mloop = g_main_loop_new(NULL, FALSE);

  if (setup()) {
    g_idle_add((GSourceFunc)myloop, NULL);
    g_main_loop_run(mloop);
  }
  destroy();
  g_main_loop_unref(mloop);
  return EXIT_SUCCESS;
}
