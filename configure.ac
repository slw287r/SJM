AC_INIT([Simple Job Manager], [1.2.0], [lacroute@stanford.edu], [sjm])
AM_INIT_AUTOMAKE(foreign)
AM_CONFIG_HEADER(config.h)
AC_PREFIX_DEFAULT(/usr/local/)

# ckeck for compiler
AC_PROG_CXX
AC_PROG_CC

# check for presence of the Sun Grid Engine scheduler
AC_MSG_CHECKING([for Sun Grid Engine])
AC_ARG_VAR(SGE_ROOT, [Sun Grid Engine root directory])
AS_IF([test "x$SGE_ROOT" != "x"], [have_sge=yes], [have_sge=no])
AC_MSG_RESULT($have_sge)

# check for presence of the Platform LSF scheduler
AC_MSG_CHECKING([for Platform LSF])
AC_ARG_VAR(LSF_LIBDIR, [Platform LSF library directory])
AS_IF([test "x$LSF_LIBDIR" != "x"], [have_lsf=yes], [have_lsf=no])
AC_MSG_RESULT($have_lsf)

# let the user choose a scheduler (--with-scheduler command-line option)
AC_ARG_WITH([scheduler],
  [AS_HELP_STRING([--with-scheduler=value],
    [set the job scheduler to one of these supported values: SGE, LSF])],
  [AS_CASE([$with_scheduler],
           [SGE], [],
           [LSF], [],
           [AC_MSG_ERROR([job scheduler $with_scheduler is not supported])])],
  [with_scheduler=check])
  
# choose a scheduler if none was specified
AS_IF([test "x$with_scheduler" = "xcheck"],
      [AS_IF([test "x$have_sge" = "xyes"], [with_scheduler=SGE],
	     [test "x$have_lsf" = "xyes"], [with_scheduler=LSF],
	     [AC_MSG_ERROR(
               [you need a job scheduler (try --with-scheduler)])])])

# record the selected job scheduler
AC_MSG_NOTICE([using scheduler $with_scheduler])
AS_CASE([$with_scheduler],
        [SGE], [AC_DEFINE([HAVE_SGE], [1], [use Sun Grid Engine])],
        [LSF], [AC_DEFINE([HAVE_LSF], [1], [use Platform LSF])],
        [AC_MSG_ERROR([job scheduler $with_scheduler is not supported])])
AM_CONDITIONAL([SGE], [test x$with_scheduler = xSGE])
AM_CONDITIONAL([LSF], [test x$with_scheduler = xLSF])

# determine the architecture for Sun Grid Engine (for finding libs)
AS_IF([test "x$with_scheduler" = "xSGE"],
      [SGE_ARCH=$($SGE_ROOT/util/arch); AC_SUBST(SGE_ARCH)])

# check for environment modules
AC_ARG_WITH([modules],
  [AS_HELP_STRING([--with-modules], [support environment modules])],
  [],
  [with_modules=check])
AS_IF([test "x$with_modules" != "xno"],
      AC_DEFINE([HAVE_MODULES], [1], [Enable environment modules]))

# check for default SGE parallel environment
AC_ARG_WITH([pe],
  [AS_HELP_STRING([--with-pe],
                  [specify a default SGE PE for multicore jobs])],
                  [],
                  [with_pe="smp"])
AC_DEFINE_UNQUOTED([SGE_MULTICORE_PE], ["$with_pe"],
                   [SGE parallel environment for multicore jobs])

# XXX check for scheduler header files and libraries
AC_CHECK_HEADER([CLI.hpp])

# write variable into Makefile
AC_SUBST(AM_CXXFLAGS, "-std=c++11 -g -Wall")

AC_CONFIG_FILES([
  Makefile
  src/Makefile
])

AC_OUTPUT
