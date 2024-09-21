#-- start of make_header -----------------

#====================================
#  Library PhysicsAnpRPC
#
#   Generated Thu Jun 15 10:51:00 2017  by hengli
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_PhysicsAnpRPC_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_PhysicsAnpRPC_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_PhysicsAnpRPC

PhysicsAnpRPC_tag = $(tag)

#cmt_local_tagfile_PhysicsAnpRPC = $(PhysicsAnpRPC_tag)_PhysicsAnpRPC.make
cmt_local_tagfile_PhysicsAnpRPC = $(bin)$(PhysicsAnpRPC_tag)_PhysicsAnpRPC.make

else

tags      = $(tag),$(CMTEXTRATAGS)

PhysicsAnpRPC_tag = $(tag)

#cmt_local_tagfile_PhysicsAnpRPC = $(PhysicsAnpRPC_tag).make
cmt_local_tagfile_PhysicsAnpRPC = $(bin)$(PhysicsAnpRPC_tag).make

endif

include $(cmt_local_tagfile_PhysicsAnpRPC)
#-include $(cmt_local_tagfile_PhysicsAnpRPC)

ifdef cmt_PhysicsAnpRPC_has_target_tag

cmt_final_setup_PhysicsAnpRPC = $(bin)setup_PhysicsAnpRPC.make
cmt_dependencies_in_PhysicsAnpRPC = $(bin)dependencies_PhysicsAnpRPC.in
#cmt_final_setup_PhysicsAnpRPC = $(bin)PhysicsAnpRPC_PhysicsAnpRPCsetup.make
cmt_local_PhysicsAnpRPC_makefile = $(bin)PhysicsAnpRPC.make

else

cmt_final_setup_PhysicsAnpRPC = $(bin)setup.make
cmt_dependencies_in_PhysicsAnpRPC = $(bin)dependencies.in
#cmt_final_setup_PhysicsAnpRPC = $(bin)PhysicsAnpRPCsetup.make
cmt_local_PhysicsAnpRPC_makefile = $(bin)PhysicsAnpRPC.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)PhysicsAnpRPCsetup.make

#PhysicsAnpRPC :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'PhysicsAnpRPC'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = PhysicsAnpRPC/
#PhysicsAnpRPC::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
#-- start of libary_header ---------------

PhysicsAnpRPClibname   = $(bin)$(library_prefix)PhysicsAnpRPC$(library_suffix)
PhysicsAnpRPClib       = $(PhysicsAnpRPClibname).a
PhysicsAnpRPCstamp     = $(bin)PhysicsAnpRPC.stamp
PhysicsAnpRPCshstamp   = $(bin)PhysicsAnpRPC.shstamp

PhysicsAnpRPC :: dirs  PhysicsAnpRPCLIB
	$(echo) "PhysicsAnpRPC ok"

#-- end of libary_header ----------------
#-- start of library_no_static ------

#PhysicsAnpRPCLIB :: $(PhysicsAnpRPClib) $(PhysicsAnpRPCshstamp)
PhysicsAnpRPCLIB :: $(PhysicsAnpRPCshstamp)
	$(echo) "PhysicsAnpRPC : library ok"

$(PhysicsAnpRPClib) :: $(bin)Utils.o
	$(lib_echo) "static library $@"
	$(lib_silent) cd $(bin); \
	  $(ar) $(PhysicsAnpRPClib) $?
	$(lib_silent) $(ranlib) $(PhysicsAnpRPClib)
	$(lib_silent) cat /dev/null >$(PhysicsAnpRPCstamp)

#------------------------------------------------------------------
#  Future improvement? to empty the object files after
#  storing in the library
#
##	  for f in $?; do \
##	    rm $${f}; touch $${f}; \
##	  done
#------------------------------------------------------------------

#
# We add one level of dependency upon the true shared library 
# (rather than simply upon the stamp file)
# this is for cases where the shared library has not been built
# while the stamp was created (error??) 
#

$(PhysicsAnpRPClibname).$(shlibsuffix) :: $(bin)Utils.o $(use_requirements) $(PhysicsAnpRPCstamps)
	$(lib_echo) "shared library $@"
	$(lib_silent) $(shlibbuilder) $(shlibflags) -o $@ $(bin)Utils.o $(PhysicsAnpRPC_shlibflags)
	$(lib_silent) cat /dev/null >$(PhysicsAnpRPCstamp) && \
	  cat /dev/null >$(PhysicsAnpRPCshstamp)

$(PhysicsAnpRPCshstamp) :: $(PhysicsAnpRPClibname).$(shlibsuffix)
	$(lib_silent) if test -f $(PhysicsAnpRPClibname).$(shlibsuffix) ; then \
	  cat /dev/null >$(PhysicsAnpRPCstamp) && \
	  cat /dev/null >$(PhysicsAnpRPCshstamp) ; fi

PhysicsAnpRPCclean ::
	$(cleanup_echo) objects PhysicsAnpRPC
	$(cleanup_silent) /bin/rm -f $(bin)Utils.o
	$(cleanup_silent) /bin/rm -f $(patsubst %.o,%.d,$(bin)Utils.o) $(patsubst %.o,%.dep,$(bin)Utils.o) $(patsubst %.o,%.d.stamp,$(bin)Utils.o)
	$(cleanup_silent) cd $(bin); /bin/rm -rf PhysicsAnpRPC_deps PhysicsAnpRPC_dependencies.make

#-----------------------------------------------------------------
#
#  New section for automatic installation
#
#-----------------------------------------------------------------

install_dir = ${CMTINSTALLAREA}/$(tag)/lib
PhysicsAnpRPCinstallname = $(library_prefix)PhysicsAnpRPC$(library_suffix).$(shlibsuffix)

PhysicsAnpRPC :: PhysicsAnpRPCinstall ;

install :: PhysicsAnpRPCinstall ;

PhysicsAnpRPCinstall :: $(install_dir)/$(PhysicsAnpRPCinstallname)
ifdef CMTINSTALLAREA
	$(echo) "installation done"
endif

$(install_dir)/$(PhysicsAnpRPCinstallname) :: $(bin)$(PhysicsAnpRPCinstallname)
ifdef CMTINSTALLAREA
	$(install_silent) $(cmt_install_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(PhysicsAnpRPCinstallname)" \
	    -out "$(install_dir)" \
	    -cmd "$(cmt_installarea_command)" \
	    -cmtpath "$($(package)_cmtpath)"
endif

##PhysicsAnpRPCclean :: PhysicsAnpRPCuninstall

uninstall :: PhysicsAnpRPCuninstall ;

PhysicsAnpRPCuninstall ::
ifdef CMTINSTALLAREA
	$(cleanup_silent) $(cmt_uninstall_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(PhysicsAnpRPCinstallname)" \
	    -out "$(install_dir)" \
	    -cmtpath "$($(package)_cmtpath)"
endif

#-- end of library_no_static ------
#-- start of cpp_library -----------------

ifneq (-MMD -MP -MF $*.d -MQ $@,)

ifneq ($(MAKECMDGOALS),PhysicsAnpRPCclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)Utils.d

$(bin)$(binobj)Utils.d :

$(bin)$(binobj)Utils.o : $(cmt_final_setup_PhysicsAnpRPC)

$(bin)$(binobj)Utils.o : $(src)Utils.cxx
	$(cpp_echo) $(src)Utils.cxx
	$(cpp_silent) $(cppcomp) -MMD -MP -MF $*.d -MQ $@ -o $@ $(use_pp_cppflags) $(PhysicsAnpRPC_pp_cppflags) $(lib_PhysicsAnpRPC_pp_cppflags) $(Utils_pp_cppflags) $(use_cppflags) $(PhysicsAnpRPC_cppflags) $(lib_PhysicsAnpRPC_cppflags) $(Utils_cppflags) $(Utils_cxx_cppflags)  $(src)Utils.cxx
endif
endif

else
$(bin)PhysicsAnpRPC_dependencies.make : $(Utils_cxx_dependencies)

$(bin)PhysicsAnpRPC_dependencies.make : $(src)Utils.cxx

$(bin)$(binobj)Utils.o : $(Utils_cxx_dependencies)
	$(cpp_echo) $(src)Utils.cxx
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(PhysicsAnpRPC_pp_cppflags) $(lib_PhysicsAnpRPC_pp_cppflags) $(Utils_pp_cppflags) $(use_cppflags) $(PhysicsAnpRPC_cppflags) $(lib_PhysicsAnpRPC_cppflags) $(Utils_cppflags) $(Utils_cxx_cppflags)  $(src)Utils.cxx

endif

#-- end of cpp_library ------------------
#-- start of cleanup_header --------------

clean :: PhysicsAnpRPCclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(PhysicsAnpRPC.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

PhysicsAnpRPCclean ::
#-- end of cleanup_header ---------------
#-- start of cleanup_library -------------
	$(cleanup_echo) library PhysicsAnpRPC
	-$(cleanup_silent) cd $(bin) && \rm -f $(library_prefix)PhysicsAnpRPC$(library_suffix).a $(library_prefix)PhysicsAnpRPC$(library_suffix).$(shlibsuffix) PhysicsAnpRPC.stamp PhysicsAnpRPC.shstamp
#-- end of cleanup_library ---------------
