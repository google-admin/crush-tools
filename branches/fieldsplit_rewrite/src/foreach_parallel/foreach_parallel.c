/********************************
   Copyright 2008 Google Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 ********************************/
#include "foreach_parallel_main.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>             /* _SC_CHILD_MAX */
#include <signal.h>
#include <err.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>

#define DEFAULT_MAX_PROCS 10

#ifdef CRUSH_DEBUG
#define DEBUG_PRINT(s) \
	fprintf(stderr, "%s" "\n", (s))
#else
#define DEBUG_PRINT(s)
#endif


static ssize_t read_code(FILE * in, FILE * out);
static int run_in_parallel(char *code, struct cmdargs *args, int argc,
                           char *argv[], int optind);
static void sig_handler(int signo);


/** @brief  
  * 
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  * 
  * @return exit status for main() to return.
  */
int foreach_parallel(struct cmdargs *args, int argc, char *argv[], int optind) {
  int tmp_fd = -1;
  char tmp_template[] = "/usr/tmp/foreach_parallel_XXXXXX";

  int ret;

  if (optind == argc) {
    fprintf(stderr, "%s: no input values provided.\n", getenv("_"));
    return EXIT_HELP;
  }

  /* no user-specified file - read stdin and put that into a
     tmp file */
  if (!args->file) {
    FILE *out;

    tmp_fd = mkstemp(tmp_template);
    if (tmp_fd < 0) {
      warn(args->file);
      return EXIT_FILE_ERR;
    }
    args->file = tmp_template;

    if (args->verbose) {
      printf("tmp fd: %d\ntmp name: %s\n", tmp_fd, args->file);
    }

    out = fopen(args->file, "w");
    ret = read_code(stdin, out);
    fclose(out);

    if (ret < 0) {
      fprintf(stderr, "%s: out of memory\n", getenv("_"));
      return EXIT_MEM_ERR;
    } else if (ret == 0) {
      fprintf(stderr, "%s: nothing to execute.\n", getenv("_"));
      return EXIT_HELP;
    }

  }

  ret = run_in_parallel(args->file, args, argc, argv, optind);

  /* see if these are pointing at the same string.  if so, we
     created a tmp file. */
  if (args->file == tmp_template)
    unlink(args->file);

  return ret;
}


static ssize_t read_code(FILE * in, FILE * out) {
  char buffer[1024];
  size_t n_read, code_len = 0;

  while (!feof(in)) {
    n_read = fread(buffer, 1, 1024, in);
    code_len += n_read;
    fwrite(buffer, 1, n_read, out);
  }

  return code_len;
}


/* reap a single exited child and return it's exit code.
 * return value of -1 indicates no children are active.
 *                 -2 => invalid wait_flags value
 */
static int wait_for_child(int wait_flags) {
  int status;
  int ret;
  for (;;) {
    if ((ret = waitpid(-1, &status, wait_flags)) < 0) {
      warn("waitpid");
      if (errno == EINTR) {
        continue;
      } else if (errno == EINVAL) {
        warn("MISUSE OF THE wait_for_child() FUNCTION");
        return -2;
      } else {
        /* ECHILD - no children to wait for. */
        return -1;
      }
    }
    break;
  }

  if (ret == 0 && wait_flags & WNOHANG) {
    return -1;
  }

  if (WIFEXITED(status))
    return WEXITSTATUS(status);

  if (WIFSIGNALED(status))
    return WTERMSIG(status);

  return status;
}


static int run_in_parallel(char *script, struct cmdargs *args, int argc,
                           char *argv[], int optind) {
  int max_procs = DEFAULT_MAX_PROCS;
  long system_max_procs;
  int active_children = 0;
  pid_t p, child_group;
  int i, ret = 0, saved_ret;

  struct sigaction sa_parent, sa_saved[4];

  if (args->max_procs) {
    if (sscanf(args->max_procs, "%d", &max_procs) != 1 || max_procs == 0) {
      fprintf(stderr, "%s: invalid value for --limit: %s - using default.\n",
              getenv("_"), args->max_procs);
      max_procs = DEFAULT_MAX_PROCS;
    }
  }

  /* sanity check - make sure the maximum number of processes is
     less than the system limit.  account for the fact that
     this application and the shell that invoked it count against
     that limit.  it's still unsafe to specify a really big value
     (slightly less so if the code to be executed doesn't spawn any
     more procs).
   */
  system_max_procs = sysconf(_SC_CHILD_MAX);
  if (system_max_procs > 0 && max_procs > system_max_procs - 2)
    max_procs = system_max_procs - 2;

  /* all children will be members of a new group having this
   * process's PID.  so if it's necessary to kill all of the 
   * children, that can be achieved by killing all members of 
   * child_group.  the parent process should not be a member of
   * that group if/when that happens, but it needs to be the
   * leader of the group while children are added to it.
   */
  child_group = getpid();
  setpgid(0, child_group);

  /* set up signal handling for the parent. */
  sigemptyset(&sa_parent.sa_mask);
  sa_parent.sa_handler = sig_handler;
  sa_parent.sa_flags = 0;

  if (sigaction(SIGINT, &sa_parent, &(sa_saved[0])) < 0)
    warn("sigaction(SIGINT)");
  if (sigaction(SIGHUP, &sa_parent, &(sa_saved[1])) < 0)
    warn("sigaction(SIGHUP)");
  if (sigaction(SIGTERM, &sa_parent, &(sa_saved[2])) < 0)
    warn("sigaction(SIGTERM)");
  if (sigaction(SIGABRT, &sa_parent, &(sa_saved[3])) < 0)
    warn("sigaction(SIGABRT)");

  for (i = optind; i < argc; i++) {
    p = fork();
    if (p < 0) {
      warn("fork error");
      return -1;
    } else if (p == 0) {
      /* in the child proc.
       * restore signal dispositions,
       * set the process group,
       * set the user-provided env var,
       * and exec the process.
       */

      if (sigaction(SIGINT, &(sa_saved[0]), NULL) < 0)
        warn("(child) sigaction(SIGINT)");
      if (sigaction(SIGHUP, &(sa_saved[1]), NULL) < 0)
        warn("(child) sigaction(SIGHUP)");
      if (sigaction(SIGTERM, &(sa_saved[2]), NULL) < 0)
        warn("(child) sigaction(SIGTERM)");
      if (sigaction(SIGABRT, &(sa_saved[3]), NULL) < 0)
        warn("(child) sigaction(SIGABRT)");

      if (setpgid(0, child_group) != 0) {
        warn("(child) setting group ID to the parent");
      }
      setenv(args->variable, argv[i], 1);
      execl(getenv("SHELL"), getenv("SHELL"), script, NULL);
    } else {
      /* in the parent proc */
      setpgid(p, child_group);
      active_children++;
      if (active_children >= max_procs) {
        DEBUG_PRINT("limit reached: waiting for a child to exit.");
        ret = wait_for_child(0);
      } else {
        /* just check to see if any children are
           waiting to be reaped. */
        ret = wait_for_child(WNOHANG);
#ifdef CRUSH_DEBUG
        if (ret >= 0) {
          DEBUG_PRINT("a child has been reaped.");
        }
#endif
      }

      /* ret == -1 means there were no children.
       * if one of the children exited with an
       * error condition (i.e. non-zero), stop
       * spawning new processes. 
       */
      if (ret >= 0) {
        active_children--;
        if (ret != 0)
          break;
      } else if (ret != -1) {
        break;
      }
    }
  }

  saved_ret = (ret == -1 ? 0 : ret);

  while (active_children > 0) {
    DEBUG_PRINT("cleanup: waiting for a child to exit.");
    ret = wait_for_child(0);

    if (ret != 0 && ret != -1)
      saved_ret = ret;

    if (ret == -1)
      active_children = 0;
    else
      active_children--;
  }

  return saved_ret;
}


/* change the group id and kill all members of the group having this
 * process's PID as the group id.
 */
static void sig_handler(int signo) {
  pid_t parent_group, child_group;

  child_group = getpid();
  parent_group = getpgrp();

  if (parent_group == child_group) {
    parent_group = getppid();

    if (setpgid(0, parent_group) != 0) {
      warn("setting group ID to the parent");
    }
  }

  kill(-child_group, signo);

  exit(signo);
}
