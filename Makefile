CC_PREFIX := aarch64-none-elf-
CC := $(CC_PREFIX)gcc
CXX := $(CC_PREFIX)g++
AS := $(CC_PREFIX)as
LD := $(CC_PREFIX)gcc
OBJCOPY := $(CC_PREFIX)objcopy

define bin2o
        $(eval _tmpasm := $(shell mktemp))
        $(SILENTCMD)bin2s -a 4 -H `(echo $(<F) | tr . _)`.h $< > $(_tmpasm)
        $(SILENTCMD)$(CC) -x assembler-with-cpp $(CPPFLAGS) $(ASFLAGS) -c $(_tmpasm) -o $(<F).o
        @rm $(_tmpasm)
endef

export TARGET		:=	$(shell basename $(CURDIR))
export BUILD		?=	debug

R_SOURCES			:=	source ../library
SOURCES				:=	

R_INCLUDES			:=	source ../library
INCLUDES 			:=

DATA				:=	data

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH			:=	-march=armv8-a -mcpu=cortex-a57

CFLAGS			:=	-g -std=c11 -Wall -Werror -Os -fomit-frame-pointer \
				-ffunction-sections -Wno-error=unused-function -Wno-unused-function \
				-fno-builtin-printf -mgeneral-regs-only \
				$(ARCH) $(INCLUDE)

CFLAGS			+=	-D_GNU_SOURCE -D_FILE_OFFSET_BITS=64

CXXFLAGS		:=	$(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11

ASFLAGS			:=	-g $(ARCH)
LDFLAGS			 =	-nostartfiles -g --specs=../stub.specs $(ARCH) -Wl,--gc-sections,-Map,$(TARGET).map -Wl,--strip-all -Wl,-z,max-page-size=0x1000

LIBS			:=

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS			:=

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export ROOTDIR	:=	$(CURDIR)
export OUTPUT	:=	$(CURDIR)/$(TARGET)

SOURCES         := $(SOURCES) $(foreach dir,$(R_SOURCES), $(dir) $(filter %/, $(wildcard $(dir)/*/)))
INCLUDES        := $(INCLUDES) $(foreach dir,$(R_INCLUDES), $(dir) $(filter %/, $(wildcard $(dir)/*/)))

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

EXCLUSIONS	:=	ccplex

SOURCE_LIST	 =	$(filter-out $(foreach exc,$(EXCLUSIONS),$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.$(exc).$(1))))), $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.$(1)))))

CFILES		:=	$(call SOURCE_LIST,c)
CPPFILES	:=	$(call SOURCE_LIST,cpp)
SFILES		:=	$(call SOURCE_LIST,s)
SCCFILES	:=	$(call SOURCE_LIST,S)
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o) $(SCCFILES:.S=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(OUTPUT).elf $(OUTPUT).bin $(BUILD)


#---------------------------------------------------------------------------------
else

DEPENDS		:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).elf: $(OFILES)
	$(LD) $(LDFLAGS) $(OFILES) -o $(OUTPUT).elf

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.bin: %.elf
	@$(OBJCOPY) -O binary $< $@
	@echo built ... $(notdir $@)


-include $(DEPENDS)


#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
