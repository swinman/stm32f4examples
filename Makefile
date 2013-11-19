################################################################################
# MAKE for a ARM Cortex-M4 Micro (both SAM4SD32C and STM32F4 are used)
################################################################################

# =================== PROJECT CONFIG ============================         {{{1
UNAME := $(shell uname)

# project name
PROJECT = stm32f4_disc
#TARGET_NAME :=$(shell pwd | sed 's/\(.*\)\/src/\1/' | sed 's/.*\/\(.*\)/\1/')

# device name
#DEVICE = SAM4SD32C
DEVICE = STM32F4

# core type
CORE = cortex-m4

# float: hardware software
ifeq ($(DEVICE), SAM4SD32C)
FLOAT_TYPE = soft
else
FLOAT_TYPE = hard
endif

# output folder (absolute or relative path, leave empty for in-tree compilation)
OUT_DIR := bin

# base folder (absolute or relative path), leave empty if make in base dir
BASE_DIR := src

# linker script
ifeq ($(DEVICE), SAM4SD32C)
LD_SCRIPT = $(BASE_DIR)/ASF/sam/utils/linker_scripts/sam4s/sam4sd32/gcc/flash.ld
else
LD_SCRIPT = $(BASE_DIR)/lib/stm32f4/stm32_flash.ld
endif

# global definitions for C++, C and ASM
# (e.g. "symbol_with_value=0xDEAD symbol_without_value")
ifeq ($(DEVICE), SAM4SD32C)
GLOBAL_DEFS  = __SAM4SD32C__
GLOBAL_DEFS += ARM_MATH_CM4=true
GLOBAL_DEFS += printf=iprintf

GLOBAL_DEFS += BOARD=SAM4S_XPLAINED_PRO SD_MMC_ENABLE
#GLOBAL_DEFINES = BOARD=SAM4S_EK2 UDD_ENABLE

else
GLOBAL_DEFS  = STM32F4XX
GLOBAL_DEFS += USE_STDPERIPH_DRIVER
GLOBAL_DEFS += HSEVALUE=8000000 # HSE is the external high speed oscillator
endif

# C++ definitions
CXX_DEFS =

# C definitions
C_DEFS =

# ASM definitions
AS_DEFS =

ifeq ($(UNAME), Linux)
ALLDIRS := $(shell find "$(BASE_DIR)" -type d -print)
else
ALLDIRS := $(BASE_DIR)
ALLDIRS += $(filter %/, $(wildcard $(BASE_DIR)/*/))
ALLDIRS += $(filter %/, $(wildcard $(BASE_DIR)/*/*/))
ALLDIRS += $(filter %/, $(wildcard $(BASE_DIR)/*/*/*/))
ALLDIRS += $(filter %/, $(wildcard $(BASE_DIR)/*/*/*/*/))
ALLDIRS += $(filter %/, $(wildcard $(BASE_DIR)/*/*/*/*/*/))
endif

# include directories (absolute or relative paths to additional folders with
# headers, current folder is always included)
INC_DIRS  = $(filter-out $(BASE_DIR)/lib, $(ALLDIRS))

# directories with source files (absolute or relative paths to
# folders with source files, current folder is always included)
# OK if the directory doesn't exist
SRCS_DIRS  = $(filter-out $(BASE_DIR)/lib $(BASE_DIR)/config, $(ALLDIRS))

# library directories (abs or rel paths to additional folders with libraries)
LIB_DIRS  =
ifeq ($(DEVICE), SAM4SD32C)
LIB_DIRS += "$(BASE_DIR)/ASF/thirdparty/CMSIS/Lib/GCC"
endif

# libraries (additional libraries for linking, e.g. "-lm -lsome_name" to link
# math library libm.a and libsome_name.a)
LIBS = -lnosys -lc
LIBS += -lrdimon

# extension of C++, c, ASM files
CXX_EXT = cpp
C_EXT = c
AS_EXT = s

# optimization flags ("-O0" - no optimization, "-O1" - optimize, "-O2" -
# optimize even more, "-Os" - optimize for size or "-O3" - optimize yet more)
OPTIMIZATION = -O2
# -fdata-sections -> don't use because section anchors won't work
# -fsection-anchors : access to gloabals faster if they are in same section
# -funroll-loops : unroll loops - faster, bigger
# -mrelax or --linkrelax : let linker use relative pointers (i think)
# set to 1 to optimize size by removing unused code and data during link phase

REMOVE_UNUSED = 1

# set to 1 to compile and link additional code required for C++
USES_CXX = 0

# C++ language standard ("c++98", "gnu++98" - default, "c++0x", "gnu++0x")
CXX_STD = gnu++98

# C language standard ("c89" / "iso9899:1990", "iso9899:199409",
# "c99" / "iso9899:1999", "gnu89" - default, "gnu99")
ifeq ($(DEVICE), SAM4SD32C)
C_STD = gnu99
else
C_STD = gnu89
endif

# ------------------ checks and format --------------------------         {{{2
# when using output folder, append trailing slash to its name
ifeq ($(strip $(OUT_DIR)), )
    OUT_DIR_F =
else
    OUT_DIR_F = $(strip $(OUT_DIR))/
endif

# Check for valid float argument
# NOTE that you have to run make clean after changing these as hardfloat and
# softfloat are not binary compatible
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
override FLOAT_TYPE = soft
endif
endif
# END: ------------- checks and format --------------------------         2}}}
# END: ============== PROJECT CONFIG ============================         1}}}

# ================== COMPILATION FLAGS ==========================         {{{1
# core flags
CORE_FLAGS = -mcpu=$(CORE) -mthumb

# flags for C++ compiler
CXX_FLAGS = -std=$(CXX_STD) -g -ggdb3 -fno-rtti -fno-exceptions -fverbose-asm
#CXX_FLAGS += -Wa,-ahlms=$(OUT_DIR_F)$(notdir $(<:.$(CXX_EXT)=.lst))

# -g3 adds more debug tags, default (-g) is -g2

# flags for C compiler
C_FLAGS = -std=$(C_STD) -g -ggdb3 -fverbose-asm
#C_FLAGS += -Wa,-ahlms=$(OUT_DIR_F)$(notdir $(<:.$(C_EXT)=.lst))
#C_FLAGS += -Wa,-ahlms=$(patsubst $(BASE_DIR)%, $(OUT_DIR)%, \
    $(<:.$(C_EXT)=.lst))
C_FLAGS += --param max-inline-insns-single=500
#C_FLAGS += -mthumb-interwork
#C_FLAGS += -nostartfiles
#C_FLAGS += -mlittle-endian
#C_FLAGS += -fno-strict-aliasing
# can't find no-strict-aliasing
#C_FLAGS += -pipe
# use pipes rather than temp files

ifeq ($(FLOAT_TYPE), hard)
C_FLAGS += -fsingle-precision-constant
C_FLAGS += -mfpu=fpv4-sp-d16
C_FLAGS += -mfloat-abi=hard
C_WARNINGS += -Wdouble-promotion
else
C_FLAGS += -msoft-float
endif

# flags for assembler
AS_FLAGS  = -g -ggdb3
#AS_FLAGS += -Wa,-amhls=$(OUT_DIR_F)$(notdir $(<:.$(AS_EXT)=.lst))

# flags for linker
LD_FLAGS  = -T$(LD_SCRIPT)
LD_FLAGS += -Wl,-Map=$(OUT_DIR_F)$(PROJECT).map
LD_FLAGS += -Wl,--cref
LD_FLAGS += -Wl,--no-warn-mismatch
LD_FLAGS += -g
ifeq ($(DEVICE), SAM4SD32C)
LD_FLAGS += -Wl,--start-group -larm_cortexM4l_math -lm  -Wl,--end-group
LD_FLAGS += -Wl,--gc-sections -Wl,--entry=Reset_Handler
endif

# process option for removing unused code
ifeq ($(REMOVE_UNUSED), 1)
    LD_FLAGS += -Wl,--gc-sections
    OPTIMIZATION += -ffunction-sections
    OPTIMIZATION += -fdata-sections
endif

# if __USES_CXX is defined for ASM then code for global/static constructors /
# destructors is compiled; if -nostartfiles option for linker is added then C++
# initialization / finalization code is not linked
ifeq ($(USES_CXX), 1)
    AS_DEFS += __USES_CXX
else
ifeq ($(DEVICE), STM32F4)
    LD_FLAGS += -nostartfiles
endif
endif

OBJCPY_REMOVE  =
ifeq ($(DEVICE), SAM4SD32C)
OBJCPY_REMOVE += .eeprom .fuse .lock .signature
endif

EEP_FLAGS = -j .eeprom --set-section-flags=.eeprom=alloc,load \
	    --change-section-lma .eeprom=0 --no-change-warnings

# define warning options here
CXX_WARNINGS = -Wall -Wextra
C_WARNINGS   = -Wall
ifeq ($(DEVICE), SAM4SD32C)
C_WARNINGS  += -Wstrict-prototypes -Wmissing-prototypes -Wpointer-arith \
	    -Wformat=2 -Wfloat-equal -Wundef -Wshadow -Wbad-function-cast \
	    -Wwrite-strings -Waggregate-return -Wmissing-declarations \
	    -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked \
	    -Wredundant-decls -Wnested-externs -Wlong-long -Wcast-align
else
#C_WARNINGS += -Wextra
#C_WARNINGS += -Werror
endif
# END: ============= COMPILATION FLAGS ==========================         1}}}

# ================== TOOLCHAIN CONFIG ===========================         {{{1
ifeq ($(UNAME), Linux)
TOOLCHAIN = arm-none-eabi-
TOOLEXT =
MKDIR_CMD = -p $@
TAGHL = ~/.vim/bundle/taghighlight/plugin/TagHighlight/TagHighlight.py
CTAGS = ctags-exuberant
else ifeq ($(UNAME), windows32)
    ifeq ($(shell ls /c/ | grep Program\ Files | wc -l), 1)
	PROG_FILES = "C:\Program Files"
    else
	PROG_FILES = "C:\Program Files (x86)"
    endif
ATMEL_FOLDER = "Atmel\Atmel Toolchain"
ARM_GCC_FOLDER = "ARM GCC\Native\4.7.3.1029\arm-gnu-toolchain\bin"
TOOLPATH = "$(PROG_FILES)\$(ATMEL_FOLDER)\$(ARM_GCC_FOLDER)\arm-none-eabi-"
TOOLEXT = .exe
MKDIR_CMD = $(subst /,\\,$@)
TAGHL = ~/vimfiles/bundle/taghighlight/plugin/TagHighlight/TagHighlight.py
CTAGS = ctags
else ifeq ($(UNAME), MINGW32_NT-6.1)
    ifeq ($(shell ls /c/ | grep Program\ Files | wc -l), 1)
	PROG_FILES = "/c/Program Files"
    else
	PROG_FILES = "/c/Program Files (x86)"
    endif
ATMEL_FOLDER = "Atmel/Atmel Toolchain"
ARM_GCC_FOLDER = "ARM GCC/Native/4.7.3.1029/arm-gnu-toolchain/bin"
TOOLPATH = "$(PROG_FILES)/$(ATMEL_FOLDER)/$(ARM_GCC_FOLDER)/arm-none-eabi-"
TOOLEXT = .exe
TAGHL = ~/vimfiles/bundle/taghighlight/plugin/TagHighlight/TagHighlight.py
MKDIR_CMD = -p $@
CTAGS = ctags
endif

CXX := $(TOOLCHAIN)g++$(TOOLEXT)
CC := $(TOOLCHAIN)gcc$(TOOLEXT)
AS := $(TOOLCHAIN)gcc$(TOOLEXT) -x assembler-with-cpp
OBJCOPY := $(TOOLCHAIN)objcopy$(TOOLEXT)
OBJDUMP := $(TOOLCHAIN)objdump$(TOOLEXT)
SIZE := $(TOOLCHAIN)size$(TOOLEXT)

RM = rm -f
# END: ============= TOOLCHAIN CONFIG ===========================         1}}}

# ===================== FORMATTING ==============================         {{{1
OBJDIR := $(patsubst $(BASE_DIR)%, $(OUT_DIR)%, $(ALLDIRS))

# wildcard for C++ source files (all files with CXX_EXT extension found in
# current folder and SRCS_DIRS folders will be compiled and linked)
CXX_SRCS = $(wildcard $(patsubst %, %/*.$(CXX_EXT), $(SRCS_DIRS)))

# wildcard for C source files (all files with C_EXT extension found in current
# folder and SRCS_DIRS folders will be compiled and linked)
C_SRCS = $(wildcard $(patsubst %, %/*.$(C_EXT), $(SRCS_DIRS)))

# wildcard for ASM source files (all files with AS_EXT extension found in
# SRCS_DIRS folders will be compiled and linked)
AS_SRCS = $(wildcard $(patsubst %, %/*.$(AS_EXT), $(SRCS_DIRS)))

# set the VPATH according to SRCS_DIRS
VPATH = $(SRCS_DIRS)

CXX_OBJS = $(patsubst $(BASE_DIR)%, $(OUT_DIR)%, $(CXX_SRCS:.$(CXX_EXT)=.o))
C_OBJS = $(patsubst $(BASE_DIR)%, $(OUT_DIR)%, $(C_SRCS:.$(C_EXT)=.o))
AS_OBJS = $(patsubst $(BASE_DIR)%, $(OUT_DIR)%, $(AS_SRCS:.$(AS_EXT)=.o))
OBJS = $(AS_OBJS) $(C_OBJS) $(CXX_OBJS) $(USER_OBJS)
DEPS = $(OBJS:.o=.d)
INC_DIRS_F = $(patsubst %, -I%, $(INC_DIRS))
LIB_DIRS_F = $(patsubst %, -L%, $(LIB_DIRS))
GLOBAL_DEFS_F = $(patsubst %, -D%, $(GLOBAL_DEFS))
CXX_DEFS_F = $(patsubst %, -D%, $(CXX_DEFS))
C_DEFS_F = $(patsubst %, -D%, $(C_DEFS))
AS_DEFS_F = $(patsubst %, -D%, $(AS_DEFS))

ELF = $(OUT_DIR_F)$(PROJECT).elf
EEP = $(OUT_DIR_F)$(PROJECT).eep
HEX = $(OUT_DIR_F)$(PROJECT).hex
BIN = $(OUT_DIR_F)$(PROJECT).bin
LSS = $(OUT_DIR_F)$(PROJECT).lss
DMP = $(OUT_DIR_F)$(PROJECT).dmp
SREC = $(OUT_DIR_F)$(PROJECT).srec

# format final flags for tools, request dependancies for C++, C and asm
CXX_FLAGS_F = $(CORE_FLAGS) $(OPTIMIZATION) $(CXX_WARNINGS) $(CXX_FLAGS) \
	$(GLOBAL_DEFS_F) $(CXX_DEFS_F) \
	-MD -MP -MF "$(@:%.o=%.d)" -MT "$(@:%.o=%.d)" -MT "$(@:%.o=%.o)" \
	$(INC_DIRS_F)
C_FLAGS_F = $(CORE_FLAGS) $(OPTIMIZATION) $(C_WARNINGS) $(C_FLAGS) \
	$(GLOBAL_DEFS_F) $(C_DEFS_F) \
	-MD -MP -MF "$(@:%.o=%.d)" -MT "$(@:%.o=%.d)" -MT "$(@:%.o=%.o)" \
	$(INC_DIRS_F)
AS_FLAGS_F = $(CORE_FLAGS) $(AS_FLAGS) $(GLOBAL_DEFS_F) $(AS_DEFS_F) \
	     -MD -MP -MF "$(@:%.o=%.d)" -MT "$(@:%.o=%.d)" -MT "$(@:%.o=%.o)" \
	     $(INC_DIRS_F)
LD_FLAGS_F = $(CORE_FLAGS) $(LD_FLAGS) $(LIB_DIRS_F)

OBJCPY_REMOVE_F = $(patsubst %, -R %, $(OBJCPY_REMOVE))

# from atmel makefile
#	    -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" \

#contents of output directory
GENERATED = $(wildcard $(patsubst %, $(OUT_DIR_F)*.%, \
	    bin dmp elf eep hex lss lst map srec))

GENERATED += $(OBJS)
GENERATED += $(DEPS)
# END: ================ FORMATTING ==============================         1}}}

# ====================== MAKE ALL ===============================         {{{1
# ------------------------- all ---------------------------------         {{{2
all : ctags make_output_dir $(ELF) $(LSS) $(DMP) $(HEX) $(BIN) $(EEP) $(SREC) print_size

# make object files dependent on Makefile
$(OBJS) : Makefile | $(OBJDIR)

# create the desired output directory
$(OBJDIR):
	mkdir $(MKDIR_CMD)

make_output_dir :
	$(shell mkdir $(OUT_DIR_F) 2>/dev/null)

link :
	@echo 'Linking target: $(ELF)'
	$(CC) $(LD_FLAGS_F) $(OBJS) $(LIBS) -o $@
# END: -------------------- all ---------------------------------         2}}}

# ------------------------- elf ---------------------------------         {{{2
# linking - objects -> elf
$(ELF) : $(OBJS)
	@echo
	@echo 'Linking target: $(ELF)'
	@$(CC) $(LD_FLAGS_F) $(OBJS) $(LIBS) -o $@

# make .elf file dependent on linker script
$(ELF) : $(LD_SCRIPT)
# END: -------------------- elf ---------------------------------         2}}}

# ------------------------- cpp ---------------------------------         {{{2
# compiling - C++ source -> objects
$(OUT_DIR_F)%.o : %.$(CXX_EXT)
	@echo 'Compiling c++ file: $<'
	@$(CXX) -c $(CXX_FLAGS_F) $< -o $@
# END: -------------------- cpp ---------------------------------         2}}}

# -------------------------- c ----------------------------------         {{{2
# compiling - C source -> objects
$(OUT_DIR_F)%.o : %.$(C_EXT)
	@echo 'Compiling file: $<'
	@$(CC) -c $(C_FLAGS_F) $< -o $@
# END: --------------------- c ----------------------------------         2}}}

# ------------------------- asm ---------------------------------         {{{2
# assembling - ASM source -> objects
$(OUT_DIR_F)%.o : %.$(AS_EXT)
	@echo 'Assembling file: $<'
	@$(AS) -c $(AS_FLAGS_F) $< -o $@
# END: -------------------- asm ---------------------------------         2}}}

# ------------------------- hex ---------------------------------         {{{2
# memory images - elf -> hex
$(HEX) : $(ELF)
	@echo 'Creating IHEX image: $(HEX)'
	@$(OBJCOPY) -O ihex $(OBJCPY_REMOVE_F) $< $@
# END: -------------------- hex ---------------------------------         2}}}

# ------------------------- bin ---------------------------------         {{{2
# memory images - elf -> bin
$(BIN) : $(ELF)
	@echo 'Creating binary image: $(BIN)'
	@$(OBJCOPY) -O binary $< $@
# END: -------------------- bin ---------------------------------         2}}}

# ------------------------ srec ---------------------------------         {{{2
# memory images - elf -> srec
$(SREC) : $(ELF)
	@echo 'Creating srec image: $(SREC)'
	@$(OBJCOPY) -O srec $(OBJCPY_REMOVE_F) $< $@
# END: ------------------- srec ---------------------------------         2}}}

# ------------------------ dump ---------------------------------         {{{2
# memory dump - elf -> dmp
$(DMP) : $(ELF)
	@echo 'Creating memory dump: $(DMP)'
	@$(OBJDUMP) -x --syms $< > $@
# END: ------------------- dump ---------------------------------         2}}}

# ------------------------- eep ---------------------------------         {{{2
# memory images - elf -> eep
$(EEP) : $(ELF)
	@echo 'Creating binary eep image: $(EEP)'
	@$(OBJCOPY) $(EEP_FLAGS) -O binary $< $@ || exit 0
# END: -------------------- eep ---------------------------------         2}}}

# ------------------------- lss ---------------------------------         {{{2
# extended listing - elf -> lss
$(LSS) : $(ELF)
	@echo 'Creating extended listing: $(LSS)'
	@$(OBJDUMP) -h -S $< > $@
# note: -h flag to display summary info from section headers
# END: -------------------- lss ---------------------------------         2}}}

# ------------------------ size ---------------------------------         {{{2
# print the size of the objects and the .elf file
print_size :
	@echo 'Size of target .elf file:'
	@$(SIZE) -B $(ELF)
	@echo ' '

module_size :
	@echo 'Size of modules:'
	@$(SIZE) -B -t --common $(OBJS) $(USER_OBJS)
# END: ------------------- size ---------------------------------         2}}}

# ------------------------ clean --------------------------------         {{{2
clean:
ifeq ($(strip $(OUT_DIR_F)), )
	@echo 'Removing all generated output files'
else
	@echo 'Removing all generated output files from: $(OUT_DIR_F)'
endif
ifneq ($(strip $(GENERATED)), )
	$(RM) $(GENERATED)
else
	@echo 'Nothing to remove...'
endif
# END: ------------------- clean --------------------------------         2}}}

# ------------------------ ctags --------------------------------         {{{2
ctags:
	@echo "Making tags, cscope.out and taghighlight files"
	-@(cd $(BASE_DIR) && \
	    echo "$$(find . -name "*.[ch]")" > cscope.files && \
	    cscope -Rkb -i cscope.files)
	@(cd $(BASE_DIR) && $(CTAGS) -R --exclude="*~" --exclude=".git")
	@(cd $(BASE_DIR) && python $(TAGHL) --ctags-file tags --source-root .)
# END: ------------------- ctags --------------------------------         2}}}

# ----------------------- install -------------------------------         {{{2
# ...................... sam4sd32c ..............................         {{{3
ifeq ($(DEVICE), SAM4SD32C)
install: $(ELF)
	JLinkGDBServer -vd -device AT91SAM4SD32C &
	@echo ""
	@echo "calling sleep 1 sec"
	@sleep 1
	@echo ""
	@echo "wake from sleep"
	@echo ""
	arm-none-eabi-gdb -n --batch -se $(ELF) \
	    -ex 'target remote localhost:2331' \
	    -ex 'monitor reset' \
	    -ex 'load $(ELF)' \
	    -ex 'monitor reset' \
	    -ex 'monitor go'
	@echo ""
	killall JLinkGDBServer

else
ifeq ($(DEVICE), SAM4SD32C)
BOARD = atmel_sams4_xplained_pro.cfg
else
BOARD = stm32f4discovery.cfg
endif

# END: ................. sam4sd32c ..............................         3}}}
# find in /usr/share/openocd/scripts

install: $(ELF)
	openocd -f board/$(BOARD) \
	    -c init -c "reset init" \
	    -c "flash write_image erase $(ELF)" \
	    -c "verify_image $(ELF) 0x00000000 elf" \
	    -c "reset run" \
	    -c "shutdown"

swdinstall: $(ELF)
	openocd -f interface/jlink.cfg \
	    -f target/stm32f4x.cfg \
	    -c reset_config srst_only srst_nogate \
	    -c init -c "reset init" \
	    -c "flash write_image erase $(ELF)" \
	    -c "verify_image $(ELF) 0x00000000 elf" \
	    -c "reset run" \
	    -c "shutdown"

# -vd : verify after load
jinstall: $(ELF)
	JLinkGDBServer -vd -device STM32F407VG -if SWD &
	@echo ""
	@echo "calling sleep 1 sec"
	@sleep 2
	@echo ""
	@echo "wake from sleep"
	@echo ""
	arm-none-eabi-gdb -n --batch -se $(ELF) \
	    -ex 'target remote localhost:2331' \
	    -ex 'monitor reset' \
	    -ex 'load $(ELF)' \
	    -ex 'monitor reset' \
	    -ex 'monitor go'
	@echo ""
	killall JLinkGDBServer
endif
# END: ------------------ install -------------------------------         2}}}

# END: ================= MAKE ALL ===============================         1}}}

# =================== GLOBAL EXPORTS ============================         {{{1
.PHONY: ctags install all clean dependents link

.SECONDARY:

# include dependancy files
-include $(DEPS)
# END: ============== GLOBAL EXPORTS ============================         1}}}

# ======================== NOTES ================================         {{{1
# Make file found on:
# https://my.st.com/public/STe2ecommunities/mcu/Lists/cortex_mx_stm32/
# Flat.aspx?RootFolder=https%3a%2f%2fmy%2est%2ecom%2fpublic%2fSTe2ecommunities
# %2fmcu%2fLists%2fcortex_mx_stm32%2ftrouble%20with%20make%20file&FolderCTID=
# 0x01200200770978C69A1141439FE559EB459D7580009C4E14902C3CDE46A77F0FFD06506F5B
# &currentviews=474

# vim: ft=make
# vim: foldmethod=marker
# END: =================== NOTES ================================         1}}}
