AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS = -I m4

BUILT_SOURCES = \
  testexternals.C

INCLUDES = -I$(includedir) -I$(OFFLINE_MAIN)/include -I$(ROOTSYS)/include 

lib_LTLIBRARIES = libAnaPHPythia.la   

AM_LDFLAGS = -L$(libdir) -L$(OFFLINE_MAIN)/lib

libAnaPHPythia_la_LIBADD = \
  -lphool -lSubsysReco -lPHHijing -lPHPythia @ROOTLIBS@ -lEG -lEGPythia6

noinst_HEADERS = \
  AnaPHPythiaLinkDef.h 

include_HEADERS = \
  RccAnaPHPythia.h \
  AnaPHPythia.h \
  AnaPHHijing.h \
  AnaPHPyBBar.h

libAnaPHPythia_la_SOURCES = \
  RccAnaPHPythia.C \
  AnaPHPythia.C \
  AnaPHHijing.C \
  AnaPHPyBBar.C \
  AnaPHPythia_Dict.C


AnaPHPythia_Dict.C: RccAnaPHPythia.h AnaPHPythia.h AnaPHHijing.h AnaPHPyBBar.h AnaPHPythiaLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libAnaPHPythia.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict*

