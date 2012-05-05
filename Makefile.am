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

SUBDIRS = src

data_DATA = bopm.conf.sample bopm.conf.blitzed exempt.conf.sample

EXTRA_DIST = ChangeLog contrib INSTALL README bopm.conf.sample bopm.conf.blitzed exempt.conf.sample network-bopm

install-data-local:
	@if test -f $(DESTDIR)$(sysconfdir)/bopm.conf ; then \
		echo "$@ will not overwrite existing $(DESTDIR)$(sysconfdir)/bopm.conf"; \
	else \
		$(mkinstalldirs) $(DESTDIR)$(sysconfdir); \
		echo "$(INSTALL_DATA) $(srcdir)/bopm.conf.sample $(DESTDIR)$(sysconfdir)/bopm.conf"; \
		$(INSTALL_DATA) $(srcdir)/bopm.conf.sample $(DESTDIR)$(sysconfdir)/bopm.conf; \
	fi
	@if test -f $(DESTDIR)$(sysconfdir)/exempt.conf ; then \
		echo "$@ will not overwrite existing $(DESTDIR)$(sysconfdir)/exempt.conf"; \
	else \
		$(mkinstalldirs) $(sysconfdir); \
		echo "$(INSTALL_DATA) $(srcdir)/exempt.conf.sample $(DESTDIR)$(sysconfdir)/exempt.conf"; \
		$(INSTALL_DATA) $(srcdir)/exempt.conf.sample $(DESTDIR)$(sysconfdir)/exempt.conf; \
	fi
	$(mkinstalldirs) $(localstatedir)

uninstall-local:
	@if test -f $(DESTDIR)$(sysconfdir)/bopm.conf ; then \
		echo "rm -f $(DESTDIR)$(sysconfdir)/bopm.conf"; \
		$(RM) -f $(DESTDIR)$(sysconfdir)/bopm.conf; \
	fi
	@if test -f $(DESTDIR)$(sysconfdir)/exempt.conf ; then \
		echo "rm -f $(DESTDIR)$(sysconfdir)/exempt.conf"; \
		$(RM) -f $(DESTDIR)$(sysconfdir)/exempt.conf; \
	fi
