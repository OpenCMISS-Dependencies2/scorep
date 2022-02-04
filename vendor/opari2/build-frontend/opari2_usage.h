"Usage: opari2 [OPTION] ... infile [outfile]\n"
"\n"
"****************** general options and parameters *******************\n"
"\n"
"[--f77|--f90|--c|--c++]\n"
"  [OPTIONAL] Specifies the programming language of the input source\n"
"  file. This option is only necessary if the automatic language\n"
"  detection based on the input file suffix fails.\n"
"\n"
"[--free-form]\n"
"  [OPTIONAL] Specifies that free formatting is used for Fortran source\n"
"  files. This is the default for Fortran 90/95.\n"
"\n"
"[--fix-form]\n"
"  [OPTIONAL] Specifies that fixed formatting is used for Fortran source\n"
"  files. This is the default for Fortran 77.\n"
"\n"
"[--nosrc]\n"
"  [OPTIONAL] If specified, OPARI2 does not generate #line constructs,\n"
"  which allow to preserve the original source file and line number\n"
"  information, in the transformation process. This option might be\n"
"  necessary if the OpenMP compiler does not understand #line\n"
"  constructs.  The default is to generate #line constructs.\n"
"\n"
"[--disable=paradigm[:directive|group[:inner],...][+paradigm...]\n"
"  [OPTIONAL] Disable the instrumentation of whole paradigms, or\n"
"  specific directives or groups of directives of a paradigm.\n"
"  Furthermore it gives the possibility to suppress the insertion of\n"
"  instrumentation functions inside code regions, i.e. only the\n"
"  surrounding instrumentation is inserted. See the paradigm \n"
"  sections below.\n"
"\n"
"[--preprocessed]\n"
"  [OPTIONAL] Indicates that the source file is already\n"
"  preprocessed. It requires that necessary instrumentation interface\n"
"  headers are already included. Furthermore, it requires a marker,\n"
"  e.g.  ___POMP2_INCLUDE___ immediately after the respective include\n"
"  file.\n"
"\n"
"[--version]\n"
"  [OPTIONAL] Prints version information.\n"
"\n"
"[--help]\n"
"  [OPTIONAL] Prints this help text.\n"
"\n"
"infile\n"
"  Input file name.\n"
"\n"
"[outfile]\n"
"  [OPTIONAL] Output file name. If not specified, OPARI2 uses the name\n"
"  infile.mod.suffix if the input file is called infile.suffix.\n"
"\n"
"********************** OpenMP specific options **********************\n"
"\n"
"[--disable=omp[:directive|group,...]\n"
"  [OPTIONAL] Accepted directives are 'atomic', 'critical', 'master',\n"
"  'flush', 'single', 'ordered' or 'locks'. These directives form the\n"
"  group 'sync', that disables all of them. The group 'task' prevents\n"
"  the instrumentation of task directives.\n"
"  E.g., --disable=omp:master,atomic diables the instrumentation of\n"
"  master and atomic directives.\n"
"\n"
"[--omp-nodecl]\n"
"  [OPTIONAL] Disables the generation of POMP2_DLISTXXXXX macros. These\n"
"  are used in the parallel directives of the instrumentation to make\n"
"  the region handles shared. By using this option the shared clause is\n"
"  used directly on the parallel directive with the respective region\n"
"  handles.\n"
"\n"
"[--omp-tpd]\n"
"  [OPTIONAL] Adds the clause 'copyin(<pomp_tpd>)' to any parallel\n"
"  construct. This allows to pass data from the creating thread to its\n"
"  children. The variable is declared externally in all files, so it\n"
"  needs to be defined by the pomp library. This option is not\n"
"  supported when using the Fujitsu compiler.\n"
"\n"
"[--omp-tpd-mangling=gnu|intel|sun|pgi|ibm|cray]\n"
"  [OPTIONAL] If programming languages are mixed(C and Fortran), the\n"
"  <pomp_tpd> needs to use the Fortran mangled name also in C files.\n"
"  This option specifies to use the mangling scheme of the gnu, intel,\n"
"  sun, pgi or ibm compiler. The default is to use the mangling scheme\n"
"  of the compiler used to build OPARI2.\n"
"\n"
"[--omp-task=abort|warn|remove]\n"
"  Special treatment for the task directive\n"
"  abort:  Stop instrumentation with an error message when encountering\n"
"          a task directive.\n"
"  warn:   Resume but print a warning.\n"
"  remove: Remove all task directives.\n"
"\n"
"[--omp-task-untied=abort|keep|no-warn]\n"
"  Special treatment for the untied task attribute. The default behavior\n"
"  is to remove the untied attribute, thus making all tasks tied, and\n"
"  print out a warning.\n"
"  abort:   Stop instrumentation with an error message when\n"
"           encountering a task directive with the untied attribute.\n"
"  keep:    Do not remove the untied attribute.\n"
"  no-warn: Do not print out a warning.\n"
"\n"
"**********************************************************************\n"
"\n"
"Please report bugs to <support@score-p.org>.\n"