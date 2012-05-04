AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

MAINTAINERCLEANFILES = \
	$(srcdir)/configure \
	$(srcdir)/Makefile.in \
	$(srcdir)/aclocal.m4 \
	$(srcdir)/config.guess \
	$(srcdir)/config.sub \
	$(srcdir)/depcomp \
	$(srcdir)/install-sh \
	$(srcdir)/missing \
	$(srcdir)/ltmain.sh \
	$(srcdir)/ylwrap \
	$(srcdir)/m4/libtool.m4 \
	$(srcdir)/m4/ltoptions.m4 \
	$(srcdir)/m4/ltsugar.m4 \
	$(srcdir)/m4/ltversion.m4 \
	$(srcdir)/m4/lt~obsolete.m4

SUBDIRS = src doc

EXTRA_DIST = LICENSE

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

docs:
	-cd doc && $(MAKE) $(AM_MAKEFLAGS) $@
