#ifndef TEST_TESTING_H__
#define TEST_TESTING_H__

#include <errno.h>
#include <pthread.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "./config.h"

char _T_MSG[4096];
jmp_buf JUMP_BUF;
int ARGC;
char** ARGV;
typedef void (*TEST_FN)(void);
TEST_FN TESTS[1000];
const char* TEST_NAMES[1000];
int TEST_MASK[1000];
int TESTS_COUNT = 0;
int EXIT_CODE = 0;
int PLAN_START_INDEX = 1;
int TEST_TIMEOUT = 20000;
int PRINT_TEST_PLAN = 1;
volatile int TEST_RESULT;
pthread_mutex_t TEST_RUNNER_FINISH_MUTEX;
pthread_cond_t TEST_RUNNER_FINISH_COND;

#define _TEST_RESULT_OK 0
#define _TEST_RESULT_ASSERTION_FAILED 1
#define _TEST_RESULT_TIMEOUT 2

int _t_fail(const char* file, int line, const char* msg, ...) {
  va_list args;
  char TEMP[sizeof(_T_MSG)];

  va_start(args, msg);
  vsnprintf(TEMP, sizeof(TEMP), msg, args);
  va_end(args);

  snprintf(_T_MSG, sizeof(_T_MSG), "[%s:%d] ASSERTION FAILED: %s", file, line,
           TEMP);
  longjmp(JUMP_BUF, 1);
  return 0;
}

#ifdef HAS_UNITTEST_TIMEOUT
int _t_bailout(const char* file, int line, const char* expr, int code) {
  printf("Bail out! [%s:%d] RUNTIME ERROR: '%s' FAILED WITH ERROR: %d [%s]\n",
         file, line, expr, code, strerror(code));
  fflush(stdout);
  exit(100);
  return 0;
}
#endif

void _t_help() {
  printf("%s v%s\n", PACKAGE_NAME, PACKAGE_VERSION);
  printf("\t-h, --help: show this help message\n");
  printf("\t-l, --list: show list of test cases available\n");
  printf("\t-c, --count: print number of tests in test suit and exit\n");
  printf("\t-t, --test: run specific test\n");
  printf("\t-e, --exclude: skip running specific test\n");
  printf(
      "\t-i, --index: starting test index to use for TAP(Test Anything "
      "Protocol), Default: 1\n");
  printf(
      "\t-w, --timeout: timeout in milliseconds to give to each test, Default: "
      "%d\n",
      TEST_TIMEOUT);
  printf(
      "\t--no-plan: do not print TAP test plan (used for aggregating multiple "
      "test files)\n");
  printf("\nsend bug reports to %s\n\n", PACKAGE_BUGREPORT);
}

#ifdef HAS_UNITTEST_TIMEOUT
static void _t_signal_handler(int signum) { pthread_exit(NULL); }
#endif

void _t_init(int argc, char** argv) {
  int i;
  int j;
  ARGC = argc;
  ARGV = argv;
  int specific_countered = 0;
  int found;

#ifdef HAS_UNITTEST_TIMEOUT
  signal(SIGUSR1, _t_signal_handler);
#endif

  for (i = 0; i < TESTS_COUNT; i++) {
    TEST_MASK[i] = 1;
  }

  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
      _t_help();
      exit(0);
    } else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--count")) {
      printf("%d\n", TESTS_COUNT);
      exit(0);
    } else if (!strcmp(argv[i], "--no-plan")) {
      PRINT_TEST_PLAN = 0;
    } else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--index")) {
      if ((i + 1) >= argc || !sscanf(argv[i + 1], "%d", &PLAN_START_INDEX)) {
        _t_help();
        exit(-1);
      }

      i = i + 1;
    } else if (!strcmp(argv[i], "-w") || !strcmp(argv[i], "--timeout")) {
      if ((i + 1) >= argc || !sscanf(argv[i + 1], "%d", &TEST_TIMEOUT)) {
        _t_help();
        exit(-1);
      }

      i = i + 1;
    } else if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--list")) {
      for (j = 0; j < TESTS_COUNT; j++) {
        printf("%s\n", TEST_NAMES[j]);
      }
      exit(0);
    } else if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--test")) {
      if (!specific_countered) {
        for (j = 0; j < TESTS_COUNT; j++) {
          TEST_MASK[j] = 0;
        }

        specific_countered = 1;
      }

      if ((i + 1) >= argc) {
        _t_help();
        exit(-1);
      }

      found = 0;
      for (j = 0; j < TESTS_COUNT; j++) {
        if (!strcmp(TEST_NAMES[j], argv[i + 1])) {
          found = 1;
          break;
        }
      }

      if (!found) {
        printf("no test matching name: %s\n", argv[i + 1]);
        exit(-1);
      }

      TEST_MASK[j] = 1;
      i = i + 1;
    } else if (!strcmp(argv[i], "-e") || !strcmp(argv[i], "--exclude")) {
      if ((i + 1) >= argc) {
        _t_help();
        exit(-1);
      }

      found = 0;
      for (j = 0; j < TESTS_COUNT; j++) {
        if (!strcmp(TEST_NAMES[j], argv[i + 1])) {
          found = 1;
          break;
        }
      }

      if (!found) {
        printf("no test matching name: %s\n", argv[i + 1]);
        exit(-1);
      }

      TEST_MASK[j] = 0;
      i = i + 1;
    } else {
      _t_help();
      exit(-1);
    }
  }
}

void _t_add(void (*fn)(void), const char* name) {
  TESTS[TESTS_COUNT] = fn;
  TEST_NAMES[TESTS_COUNT] = name;
  TESTS_COUNT++;
}

void _format_dtime(struct timeval* start, struct timeval* stop, char* buffer,
                   size_t n) {
  int delta;

  delta = (((int)(stop->tv_sec - start->tv_sec)) * 1000) +
          (((int)(stop->tv_usec - start->tv_usec)) / 1000);
  snprintf(buffer, n, "%d ms", delta);
}

#define _T_RUNTIME_OK(expr) \
  ((err = (expr)) == 0 ? (0) : (_t_bailout(__FILE__, __LINE__, #expr, err)))

static void* _t_runner(void* data) {
  void (*fn)(void) = (void (*)(void))data;

#ifdef HAS_UNITTEST_TIMEOUT
  static sigset_t mask;
  int err;

  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);
#endif

  if (setjmp(JUMP_BUF) != 0) {
    TEST_RESULT = _TEST_RESULT_ASSERTION_FAILED;
  } else {
    fn();
    TEST_RESULT = _TEST_RESULT_OK;
  }

#ifdef HAS_UNITTEST_TIMEOUT
  _T_RUNTIME_OK(pthread_mutex_lock(&TEST_RUNNER_FINISH_MUTEX));
  _T_RUNTIME_OK(pthread_cond_signal(&TEST_RUNNER_FINISH_COND));
  _T_RUNTIME_OK(pthread_mutex_unlock(&TEST_RUNNER_FINISH_MUTEX));
#endif

  return NULL;
}

#ifdef HAS_UNITTEST_TIMEOUT

static void* _t_runner_watchdog(void* data) {
  int err;
  pthread_t runner;
  struct timespec wait;
  struct timeval now;
  int is_timeout = 0;

  _T_RUNTIME_OK(pthread_mutex_init(&TEST_RUNNER_FINISH_MUTEX, NULL));
  _T_RUNTIME_OK(pthread_cond_init(&TEST_RUNNER_FINISH_COND, NULL));

  _T_RUNTIME_OK(pthread_mutex_lock(&TEST_RUNNER_FINISH_MUTEX));

  _T_RUNTIME_OK(pthread_create(&runner, NULL, _t_runner, data));

  gettimeofday(&now, NULL);
  wait.tv_sec = now.tv_sec + ((TEST_TIMEOUT) / 1000L);
  wait.tv_nsec = (now.tv_usec * 1000L) + (((TEST_TIMEOUT) % 1000L) * 1000000L);
  wait.tv_sec += wait.tv_nsec / 1000000000L;
  wait.tv_nsec = wait.tv_nsec % 1000000000L;

  err = pthread_cond_timedwait(&TEST_RUNNER_FINISH_COND,
                               &TEST_RUNNER_FINISH_MUTEX, &wait);

  if (err == ETIMEDOUT) {
    err = pthread_kill(runner, SIGUSR1);

    if (!err) {
      is_timeout = 1;
    } else if (err != ESRCH) {
      _T_RUNTIME_OK(err);
    }
  } else {
    _T_RUNTIME_OK(err);
  }

  _T_RUNTIME_OK(pthread_join(runner, NULL));

  if (is_timeout) {
    TEST_RESULT = _TEST_RESULT_TIMEOUT;
  }

  _T_RUNTIME_OK(pthread_mutex_unlock(&TEST_RUNNER_FINISH_MUTEX));
  _T_RUNTIME_OK(pthread_cond_destroy(&TEST_RUNNER_FINISH_COND));
  _T_RUNTIME_OK(pthread_mutex_destroy(&TEST_RUNNER_FINISH_MUTEX));

  return NULL;
}

#endif

void _t_run_test(void (*fn)(void), const char* name, int index) {
  struct timeval stop, start;
  char dtime[100];
  char* token;
#ifdef HAS_UNITTEST_TIMEOUT
  pthread_t watchdog;
  int err;
#endif
  char* saveptr;

  gettimeofday(&start, NULL);
#ifdef HAS_UNITTEST_TIMEOUT
  _T_RUNTIME_OK(pthread_create(&watchdog, NULL, _t_runner_watchdog, fn));
  _T_RUNTIME_OK(pthread_join(watchdog, NULL));
#endif
  _t_runner(fn);
  gettimeofday(&stop, NULL);

  _format_dtime(&start, &stop, dtime, sizeof(dtime));

  if (TEST_RESULT == _TEST_RESULT_ASSERTION_FAILED) {
    printf("not ok %d - %s # ASSERTION FAILED (took: %s)\n", index, name,
           dtime);
    token = strtok_r(_T_MSG, "\n", &saveptr);
    while (token) {
      printf("# %s\n", token);
      token = strtok_r(NULL, "\n", &saveptr);
    }
    fflush(stdout);

    EXIT_CODE = -1;
  } else if (TEST_RESULT == _TEST_RESULT_TIMEOUT) {
    printf("not ok %d - %s # TIMEOUT EXCEEDED (took: %s)\n", index, name,
           dtime);
    fflush(stdout);

    EXIT_CODE = -1;
  } else {
    printf("ok %d - %s # (took: %s)\n", index, name, dtime);
    fflush(stdout);
  }
}

int _t_run(int argc, char** argv) {
  struct timeval stop, start;
  char dtime[100];
  int i;

  _t_init(argc, argv);
  gettimeofday(&start, NULL);

  printf("\n");
  printf("##\n");
  printf("## %s\n", argv[0]);
  printf("##\n");
  printf("\n");
  fflush(stdout);

  if (PRINT_TEST_PLAN) {
    printf("%d..%d\n", PLAN_START_INDEX, PLAN_START_INDEX + TESTS_COUNT - 1);
    printf("\n");
    fflush(stdout);
  }

  for (i = 0; i < TESTS_COUNT; i++) {
    if (TEST_MASK[i]) {
      _t_run_test(TESTS[i], TEST_NAMES[i], PLAN_START_INDEX + i);
    } else {
      printf("ok %d - %s # SKIP\n", PLAN_START_INDEX + i, TEST_NAMES[i]);
      fflush(stdout);
    }
  }

  gettimeofday(&stop, NULL);
  _format_dtime(&start, &stop, dtime, sizeof(dtime));

  printf("\n");
  printf("## took (%s)\n", dtime);
  fflush(stdout);

  return EXIT_CODE;
}

#define _T_COMMA ,
#define _T_PARAN_OPEN (
#define _T_PARAN_CLOSE )
#define _T_ASSERT(check, msg) \
  ((check) ? (1) : (_t_fail(__FILE__, __LINE__, msg)))
#define T_ASSERT(expr) (_T_ASSERT((expr), #expr))
#define T_TRUE(expr) (_T_ASSERT((expr), "expected \"" #expr "\" to be true"))
#define T_FALSE(expr) (_T_ASSERT(!(expr), "expected \"" #expr "\" to be false"))
#define T_OK(expr) (_T_ASSERT(!(expr), "expected \"" #expr "\" to be ok"))
#define T_ZERO(expr) (_T_ASSERT(!(expr), "expected \"" #expr "\" to be zero"))
#define T_CMPINT(expr, op, value)              \
  (_T_ASSERT(                                  \
      ((expr)op(value)),                       \
      "expected \"" #expr "\"[=%d] to be " #op \
      " to %d" _T_COMMA _T_PARAN_OPEN expr _T_PARAN_CLOSE _T_COMMA value))
#define T_EQUAL_PTR(expr, value)                                     \
  (_T_ASSERT(((expr) == (value)),                                    \
             "expected \"" #expr                                     \
             "\"[=%X] to be equal to %X" _T_COMMA _T_PARAN_OPEN expr \
                 _T_PARAN_CLOSE _T_COMMA value))
#define T_NULL(expr)             \
  (_T_ASSERT(!(expr),            \
             "expected \"" #expr \
             "\"[=%X] to be null" _T_COMMA _T_PARAN_OPEN expr _T_PARAN_CLOSE))
#define T_NOT_NULL(expr) \
  (_T_ASSERT((expr), "expected \"" #expr "\" to non-null"))
#define T_EQUAL_STR(expr, value)                                             \
  (_T_ASSERT(!(strcmp((expr), (value))),                                     \
             "expected \"" #expr                                             \
             "\"[=\'%s\"] to be equal to \"%s\"" _T_COMMA _T_PARAN_OPEN expr \
                 _T_PARAN_CLOSE _T_COMMA value))
#define T_NOT_EQUAL_STR(expr, value)                                        \
  (_T_ASSERT((strcmp((expr), (value))),                                     \
             "expected \"" #expr                                            \
             "\"[=\'%s\"] to not be equal to \"%s\"" _T_COMMA _T_PARAN_OPEN \
                 expr _T_PARAN_CLOSE _T_COMMA value))
#define T_FAIL(msg) (_t_fail(__FILE__, __LINE__, (msg)))

#define T_RUN(argc, argv) (_t_run((argc), (argv)))
#define T_ADD(test) (_t_add(test, #test))

#endif  // TEST_TESTING_H__
