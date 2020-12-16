#
# OMNeT++/OMNEST Makefile for $(LIB_PREFIX)FiCo4OMNeT
#
# This file was generated with the command:
#  opp_makemake --make-so -f --deep -I.
#

# Name of target to be created (-o option)
TARGET = $(LIB_PREFIX)FiCo4OMNeT$(D)$(SHARED_LIB_SUFFIX)
TARGET_DIR = .

# C++ include paths (with -I)
INCLUDE_PATH = -I.

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc, .msg and .sm files
OBJS = \
    $O/fico4omnet/applications/can/sink/CanTrafficSinkAppBase.o \
    $O/fico4omnet/applications/can/source/CanTrafficSourceAppBase.o \
    $O/fico4omnet/applications/can/source/CanTrafficSourceAppBaseStoppable.o \
    $O/fico4omnet/applications/can/source/colouredsourceapp/CanColouredSourceApp.o \
    $O/fico4omnet/applications/flexray/sink/FRTrafficSinkAppBase.o \
    $O/fico4omnet/applications/flexray/source/FRTrafficSourceAppBase.o \
    $O/fico4omnet/buffer/Buffer.o \
    $O/fico4omnet/buffer/can/CanBuffer.o \
    $O/fico4omnet/buffer/can/CanInputBuffer.o \
    $O/fico4omnet/buffer/can/CanOutputBuffer.o \
    $O/fico4omnet/buffer/flexray/FRBuffer.o \
    $O/fico4omnet/buffer/flexray/FRInputBuffer.o \
    $O/fico4omnet/buffer/flexray/FROutputBuffer.o \
    $O/fico4omnet/bus/BusPort.o \
    $O/fico4omnet/bus/can/CanBusLogic.o \
    $O/fico4omnet/bus/can/CanID.o \
    $O/fico4omnet/bus/flexray/FRBusLogic.o \
    $O/fico4omnet/bus/flexray/FRTopologyPort.o \
    $O/fico4omnet/linklayer/NodePort.o \
    $O/fico4omnet/linklayer/can/CanPortInput.o \
    $O/fico4omnet/linklayer/can/CanPortOutput.o \
    $O/fico4omnet/linklayer/flexray/FRPortInput.o \
    $O/fico4omnet/linklayer/flexray/FRPortOutput.o \
    $O/fico4omnet/nodes/can/ErrorConfinement.o \
    $O/fico4omnet/scheduler/can/CanClock.o \
    $O/fico4omnet/scheduler/flexray/FRScheduler.o \
    $O/fico4omnet/scheduler/flexray/SchedulerEvent.o \
    $O/fico4omnet/synchronisation/flexray/FRSync.o \
    $O/fico4omnet/utilities/HelperFunctions.o \
    $O/fico4omnet/utilities/ResultFilters.o \
    $O/fico4omnet/linklayer/can/messages/CanDataFrame_m.o \
    $O/fico4omnet/linklayer/can/messages/ErrorFlag_m.o \
    $O/fico4omnet/linklayer/can/messages/ErrorFrame_m.o \
    $O/fico4omnet/linklayer/flexray/messages/FRFrame_m.o \
    $O/fico4omnet/scheduler/flexray/SchedulerMessage_m.o \
    $O/fico4omnet/scheduler/flexray/SchedulerMessageEvents_m.o

# Message files
MSGFILES = \
    fico4omnet/linklayer/can/messages/CanDataFrame.msg \
    fico4omnet/linklayer/can/messages/ErrorFlag.msg \
    fico4omnet/linklayer/can/messages/ErrorFrame.msg \
    fico4omnet/linklayer/flexray/messages/FRFrame.msg \
    fico4omnet/scheduler/flexray/SchedulerMessage.msg \
    fico4omnet/scheduler/flexray/SchedulerMessageEvents.msg

# SM files
SMFILES =

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIBS = -loppenvir$D $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS) $(IMPORT_DEFINES)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)
SMCOPTS =

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# inserted from file 'makefrag':
# USAGE:
# Set Environment Variables:
# EXTENDED_WARNINGS=1
# for stricter warnings and
#
# WARNINGS_ERROR=1
# for -Werror

CFLAGS += -std=c++11

UNAME_S := $(shell uname -s)

#Currently forcing clang interface on OS X! Otherwise OMNeT++ will use gcc wrapper of clang
ifeq ($(UNAME_S),Darwin)
	CC = clang
	CXX = clang++
	SHLIB_LD = clang++ -shared -fPIC
endif

#Special Flags for OS X
ifeq ($(UNAME_S),Darwin)
    LDFLAGS += -headerpad_max_install_names 
endif

#Warnings generate errors
ifdef WARNINGS_ERROR
	CFLAGS += -Werror
endif


#Stricter warnings in clang and gcc:
ifeq ($(CC),clang)
	#clang
	ifdef EXTENDED_WARNINGS
    CFLAGS += -Wall -Wunused -Wextra -Weverything
    endif
    CFLAGS += -Wno-padded -Wno-global-constructors -Wno-exit-time-destructors
    CFLAGS += -Wno-c++98-compat-pedantic -Wno-unknown-pragmas -Wno-unknown-warning-option
    CFLAGS += -Wno-error=unused-parameter -Wno-error=header-hygiene -Wno-error=documentation -Wno-error=c++98-compat-pedantic
    CFLAGS += -Wno-error=shadow -Wno-error=missing-noreturn -Wno-error=unreachable-code -Wno-error=unreachable-code-return -Wno-error=old-style-cast
    CFLAGS += -Wno-error=conversion -Wno-error=unreachable-code-break -Wno-error=documentation-unknown-command -Wno-error=reserved-id-macro
else ifeq ($(CC),gcc)
	#gcc
	ifdef EXTENDED_WARNINGS
    CFLAGS += -Wall -Wunused -Wextra -Winit-self -Wswitch-enum -Wuninitialized -Wfloat-equal -Wsuggest-attribute=pure
    CFLAGS += -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wsuggest-attribute=format -Wconversion -Wuseless-cast
    CFLAGS += -Wshadow
    #CFLAGS += -ansi -pedantic-errors
    endif
    CFLAGS += -Wno-unknown-pragmas -Wno-error=unused-parameter -Wno-error=conversion -Wno-error=suggest-attribute=noreturn
    CFLAGS += -Wno-error=shadow
endif

# add -isystem for referenced projects (-I will be ignored for those also provided with -Isystem)
RELPRJDOTS = $(subst o,,$(subst o ,/,$(patsubst %,..o,$(subst /, ,$(PROJECTRELATIVE_PATH)))))
ifneq (,$(findstring MSYS,$(shell uname -s)))
	# on windows we need this hack for pathes like -Id:/...
	SYSINCLUDES := $(shell echo '$(subst -I$(RELPRJDOTS)/.,-isystem $(RELPRJDOTS)/.,$(INCLUDE_PATH))' |sed -e 's/-I\($(subst \| ,\|,$(addsuffix \|,$(filter %:,$(subst :,: ,$(patsubst -I%,%,$(INCLUDE_PATH))))) \/)\)/-isystem \1/g')
else
    # on unix we can use this	
	SYSINCLUDES := $(subst -I/,-isystem /,$(subst -I$(RELPRJDOTS)/.,-isystem $(RELPRJDOTS)/.,$(INCLUDE_PATH)))
endif

COPTS += $(SYSINCLUDES) -isystem $(OMNETPP_INCL_DIR)

# use legacy message compiler (available since OMNeT++ 5.3)
MSGC += --msg4

# <<<
#------------------------------------------------------------------------------

# Main target
all: $(TARGET_DIR)/$(TARGET)

$(TARGET_DIR)/% :: $O/%
	@mkdir -p $(TARGET_DIR)
	$(Q)$(LN) $< $@
ifeq ($(TOOLCHAIN_NAME),clangc2)
	$(Q)-$(LN) $(<:%.dll=%.lib) $(@:%.dll=%.lib)
endif

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile $(CONFIGFILE)
	@$(MKPATH) $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) -o $O/$(TARGET) $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS) $(LDFLAGS)
	$(Q)$(SHLIB_POSTPROCESS) $O/$(TARGET)

.PHONY: all clean cleanall depend msgheaders smheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE) | msgheaders smheaders
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc -MD -MP -MF $O/$(basename $<)_m.h.d $(MSGCOPTS) $?

%_sm.cc %_sm.h: %.sm
	$(qecho) SMC: $<
	$(Q)$(SMC) -c++ -suffix cc $(SMCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

smheaders: $(SMFILES:.sm=_sm.h)

clean:
	$(qecho) Cleaning $(TARGET)
	$(Q)-rm -rf $O
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET)
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET:%.dll=%.lib)
	$(Q)-rm -f $(call opp_rwildcard, . , *_m.cc *_m.h *_sm.cc *_sm.h)

cleanall:
	$(Q)$(MAKE) -s clean MODE=release
	$(Q)$(MAKE) -s clean MODE=debug
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

# include all dependencies
-include $(OBJS:%=%.d) $(MSGFILES:%.msg=$O/%_m.h.d)
