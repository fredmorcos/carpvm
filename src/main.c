#include <carp/machine.h>
#include <getopt.h>

const double carp_version = 0.4;

static void carp_print_version(void);
static void carp_print_license(void);
static void carp_print_warranty(void);
static void carp_print_conditions(void);
static void carp_print_help(void);

int main(int argc, char **argv) {
  int c;

  extern char *optarg;
  extern int optind, opterr, optopt;

  while ((c = getopt(argc, argv, ":vlwch")) != -1) {
    switch (c) {
    case 'v': carp_print_version();    return EXIT_SUCCESS;
    case 'l': carp_print_license();    return EXIT_SUCCESS;
    case 'w': carp_print_warranty();   return EXIT_SUCCESS;
    case 'c': carp_print_conditions(); return EXIT_SUCCESS;
    case 'h': carp_print_help();       return EXIT_SUCCESS;
    default:
      fprintf(stderr, "Error: Unrecognized option `%c`\n", optopt);
      fprintf(stderr, "       See `carp -h` for help\n");
      return EXIT_FAILURE;
    }
  }

  argv += optind;
  argc -= optind;

  if (argc > 0) {
    for (int i = 0, ret = 0; i < argc; i++) {
      if ((ret = carp_run_program(argv[i])) != 0) {
        return ret;
      }
    }
  } else {
    fprintf(stderr, "Error: No files given to run\n");
    fprintf(stderr, "       See `carp -h` for help\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void carp_print_version(void) {
  printf("CarpVM version %1.1f\n", carp_version);
}

void carp_print_license () {
  puts("CarpVM Copyright (C) 2014-2015 Fred Morcos");
  puts("  Based on carp by Maxwell Bernstein");
  puts("");
  puts("This program comes with ABSOLUTELY NO WARRANTY;");
  puts("for details type `carp -w`. This is free software,");
  puts("and you are welcome to redistribute it under");
  puts("certain conditions; type `carp -c` for details.");
  puts("");
  carp_print_conditions();
}

void carp_print_warranty () {
  puts("THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY");
  puts("APPLICABLE LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT");
  puts("HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM “AS IS” WITHOUT");
  puts("WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT");
  puts("LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR");
  puts("A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND");
  puts("PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE");
  puts("DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR");
  puts("CORRECTION.");
}

void carp_print_conditions () {
  puts("See LICENSE or http://www.gnu.org/licenses/gpl-3.0.txt");
}

void carp_print_help () {
  puts("Usage:");
  puts("  carp [ARG] FILE...");
  puts("");
  puts("Arguments:");
  puts("  -v   print version number");
  puts("  -l   print licensing information");
  puts("  -w   print warranty information");
  puts("  -c   print licensing conditions");
  puts("  -h   print this help");
  puts("");
  puts("Note: The above arguments will operate then stop the execution");
  puts("      of CarpVM regardless of whether files were passed.");
  puts("");
  puts("Files:");
  puts("  The file to be executed. Multiple files can be passed and");
  puts("  will be executed in the given order. If one of the programs");
  puts("  fails, the entire execution will be aborted and the non-zero");
  puts("  exit code of that particular program will be returned.");
}
