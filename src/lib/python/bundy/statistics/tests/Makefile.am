PYCOVERAGE_RUN=@PYCOVERAGE_RUN@
PYTESTS = counters_test.py dns_test.py
EXTRA_DIST = $(PYTESTS)
EXTRA_DIST += testdata/test_spec1.spec
EXTRA_DIST += testdata/test_spec2.spec
EXTRA_DIST += testdata/test_spec3.spec

# If necessary (rare cases), explicitly specify paths to dynamic libraries
# required by loadable python modules.
LIBRARY_PATH_PLACEHOLDER =
if SET_ENV_LIBRARY_PATH
LIBRARY_PATH_PLACEHOLDER += $(ENV_LIBRARY_PATH)=$(abs_top_builddir)/src/lib/cryptolink/.libs:$(abs_top_builddir)/src/lib/dns/.libs:$(abs_top_builddir)/src/lib/dns/python/.libs:$(abs_top_builddir)/src/lib/cc/.libs:$(abs_top_builddir)/src/lib/config/.libs:$(abs_top_builddir)/src/lib/log/.libs:$(abs_top_builddir)/src/lib/util/.libs:@abs_top_builddir@/src/lib/util/threads/.libs:$(abs_top_builddir)/src/lib/exceptions/.libs:$(abs_top_builddir)/src/lib/datasrc/.libs:$$$(ENV_LIBRARY_PATH)
else
# Some systems need the ds path even if not all paths are necessary
LIBRARY_PATH_PLACEHOLDER += $(ENV_LIBRARY_PATH)=$(abs_top_builddir)/src/lib/datasrc/.libs
endif

# test using command-line arguments, so use check-local target instead of TESTS
check-local:
if ENABLE_PYTHON_COVERAGE
	touch $(abs_top_srcdir)/.coverage
	rm -f .coverage
	${LN_S} $(abs_top_srcdir)/.coverage .coverage
endif
	for pytest in $(PYTESTS) ; do \
	echo Running test: $$pytest ; \
	PYTHONPATH=$(COMMON_PYTHON_PATH) \
	$(LIBRARY_PATH_PLACEHOLDER) \
	BUNDY_FROM_BUILD=$(abs_top_builddir) \
	BUNDY_FROM_SOURCE=$(abs_top_srcdir) \
	TESTDATASRCDIR=$(abs_top_srcdir)/src/lib/python/bundy/statistics/tests/testdata \
	$(PYCOVERAGE_RUN) $(abs_srcdir)/$$pytest || exit ; \
	done
