SUBDIRS = . tests

AM_CPPFLAGS = -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib
AM_CPPFLAGS += $(BOOST_INCLUDES)
AM_CPPFLAGS += -I$(top_srcdir)/src/lib/dns -I$(top_builddir)/src/lib/dns
AM_CPPFLAGS += -I$(top_srcdir)/src/lib/asiolink -I$(top_builddir)/src/lib/asiolink
AM_CPPFLAGS += -I$(top_srcdir)/src/lib/util -I$(top_builddir)/src/lib/util

AM_CXXFLAGS = $(BUNDY_CXXFLAGS)

CLEANFILES = *.gcno *.gcda asiodns_messages.h asiodns_messages.cc s-messages

# Define rule to build logging source files from message file
asiodns_messages.h asiodns_messages.cc: s-messages

s-messages: asiodns_messages.mes
	$(top_builddir)/src/lib/log/compiler/message $(top_srcdir)/src/lib/asiodns/asiodns_messages.mes
	touch $@

BUILT_SOURCES = asiodns_messages.h asiodns_messages.cc

lib_LTLIBRARIES = libbundy-asiodns.la
libbundy_asiodns_la_SOURCES = dns_answer.h
libbundy_asiodns_la_SOURCES += asiodns.h
libbundy_asiodns_la_SOURCES += dns_lookup.h
libbundy_asiodns_la_SOURCES += dns_server.h
libbundy_asiodns_la_SOURCES += dns_service.cc dns_service.h
libbundy_asiodns_la_SOURCES += tcp_server.cc tcp_server.h
libbundy_asiodns_la_SOURCES += udp_server.cc udp_server.h
libbundy_asiodns_la_SOURCES += sync_udp_server.cc sync_udp_server.h
libbundy_asiodns_la_SOURCES += io_fetch.cc io_fetch.h
libbundy_asiodns_la_SOURCES += logger.h logger.cc

nodist_libbundy_asiodns_la_SOURCES = asiodns_messages.cc asiodns_messages.h

EXTRA_DIST = asiodns_messages.mes

# Note: the ordering matters: -Wno-... must follow -Wextra (defined in
# BUNDY_CXXFLAGS)
libbundy_asiodns_la_CXXFLAGS = $(AM_CXXFLAGS)
libbundy_asiodns_la_CPPFLAGS = $(AM_CPPFLAGS)
libbundy_asiodns_la_LIBADD  = $(top_builddir)/src/lib/asiolink/libbundy-asiolink.la
libbundy_asiodns_la_LIBADD += $(top_builddir)/src/lib/log/libbundy-log.la
