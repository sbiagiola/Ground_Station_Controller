#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Ground_Station_Controller.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Ground_Station_Controller.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS
SUB_IMAGE_ADDRESS_COMMAND=--image-address $(SUB_IMAGE_ADDRESS)
else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=Clock.c Main.c IO_Config.c UART.c interupts.c ADC.c RingBuffer.c DAC.c timer.c Protocolo_Comm_Yaesu.c IO_Accionamiento.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/Clock.o ${OBJECTDIR}/Main.o ${OBJECTDIR}/IO_Config.o ${OBJECTDIR}/UART.o ${OBJECTDIR}/interupts.o ${OBJECTDIR}/ADC.o ${OBJECTDIR}/RingBuffer.o ${OBJECTDIR}/DAC.o ${OBJECTDIR}/timer.o ${OBJECTDIR}/Protocolo_Comm_Yaesu.o ${OBJECTDIR}/IO_Accionamiento.o
POSSIBLE_DEPFILES=${OBJECTDIR}/Clock.o.d ${OBJECTDIR}/Main.o.d ${OBJECTDIR}/IO_Config.o.d ${OBJECTDIR}/UART.o.d ${OBJECTDIR}/interupts.o.d ${OBJECTDIR}/ADC.o.d ${OBJECTDIR}/RingBuffer.o.d ${OBJECTDIR}/DAC.o.d ${OBJECTDIR}/timer.o.d ${OBJECTDIR}/Protocolo_Comm_Yaesu.o.d ${OBJECTDIR}/IO_Accionamiento.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/Clock.o ${OBJECTDIR}/Main.o ${OBJECTDIR}/IO_Config.o ${OBJECTDIR}/UART.o ${OBJECTDIR}/interupts.o ${OBJECTDIR}/ADC.o ${OBJECTDIR}/RingBuffer.o ${OBJECTDIR}/DAC.o ${OBJECTDIR}/timer.o ${OBJECTDIR}/Protocolo_Comm_Yaesu.o ${OBJECTDIR}/IO_Accionamiento.o

# Source Files
SOURCEFILES=Clock.c Main.c IO_Config.c UART.c interupts.c ADC.c RingBuffer.c DAC.c timer.c Protocolo_Comm_Yaesu.c IO_Accionamiento.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/Ground_Station_Controller.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ128GP804
MP_LINKER_FILE_OPTION=,--script=p33FJ128GP804.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/Clock.o: Clock.c  .generated_files/flags/default/2c4be7ac87a4bf2ecd5bf14b448391169734a8a0 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Clock.o.d 
	@${RM} ${OBJECTDIR}/Clock.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Clock.c  -o ${OBJECTDIR}/Clock.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Clock.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/Main.o: Main.c  .generated_files/flags/default/8e77923acb0775eea69de24461b8532309404533 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Main.o.d 
	@${RM} ${OBJECTDIR}/Main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Main.c  -o ${OBJECTDIR}/Main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/IO_Config.o: IO_Config.c  .generated_files/flags/default/517fe7df9c56f015172f903ac4c361c794813f2 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/IO_Config.o.d 
	@${RM} ${OBJECTDIR}/IO_Config.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  IO_Config.c  -o ${OBJECTDIR}/IO_Config.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/IO_Config.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/UART.o: UART.c  .generated_files/flags/default/a48f28346d0f6a51d3f019cf0f6104527c483b37 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UART.o.d 
	@${RM} ${OBJECTDIR}/UART.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UART.c  -o ${OBJECTDIR}/UART.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UART.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/interupts.o: interupts.c  .generated_files/flags/default/e7284b767eacb6e091cbcfe6c6b1d155fff35966 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/interupts.o.d 
	@${RM} ${OBJECTDIR}/interupts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interupts.c  -o ${OBJECTDIR}/interupts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/interupts.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/ADC.o: ADC.c  .generated_files/flags/default/8c91d9abf066f49fb4378a58e9cb47be41dd817e .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ADC.o.d 
	@${RM} ${OBJECTDIR}/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ADC.c  -o ${OBJECTDIR}/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ADC.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/RingBuffer.o: RingBuffer.c  .generated_files/flags/default/182a56c9f95f874128e8f071b903fc439247ae81 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/RingBuffer.o.d 
	@${RM} ${OBJECTDIR}/RingBuffer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  RingBuffer.c  -o ${OBJECTDIR}/RingBuffer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/RingBuffer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/DAC.o: DAC.c  .generated_files/flags/default/6116c6d1d556de6cff5d0e55b312ed3e14082830 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DAC.o.d 
	@${RM} ${OBJECTDIR}/DAC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DAC.c  -o ${OBJECTDIR}/DAC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/DAC.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/timer.o: timer.c  .generated_files/flags/default/8186559a51047df77780989b780885737347f93c .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/timer.o.d 
	@${RM} ${OBJECTDIR}/timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  timer.c  -o ${OBJECTDIR}/timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/timer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/Protocolo_Comm_Yaesu.o: Protocolo_Comm_Yaesu.c  .generated_files/flags/default/9d08b9522b6692604c4b79c8feda67bf9e560804 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Protocolo_Comm_Yaesu.o.d 
	@${RM} ${OBJECTDIR}/Protocolo_Comm_Yaesu.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Protocolo_Comm_Yaesu.c  -o ${OBJECTDIR}/Protocolo_Comm_Yaesu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Protocolo_Comm_Yaesu.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/IO_Accionamiento.o: IO_Accionamiento.c  .generated_files/flags/default/847704e116b73bfa8a24b37a900ce12f7feba0e8 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/IO_Accionamiento.o.d 
	@${RM} ${OBJECTDIR}/IO_Accionamiento.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  IO_Accionamiento.c  -o ${OBJECTDIR}/IO_Accionamiento.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/IO_Accionamiento.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
else
${OBJECTDIR}/Clock.o: Clock.c  .generated_files/flags/default/4a472e7a5ef004b9c947d519efec75d5692e0f26 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Clock.o.d 
	@${RM} ${OBJECTDIR}/Clock.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Clock.c  -o ${OBJECTDIR}/Clock.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Clock.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/Main.o: Main.c  .generated_files/flags/default/d039b107544f23dd232ac953f325afb448eb5cf5 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Main.o.d 
	@${RM} ${OBJECTDIR}/Main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Main.c  -o ${OBJECTDIR}/Main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Main.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/IO_Config.o: IO_Config.c  .generated_files/flags/default/4c77ae40fd100a32cbb5745d200f3af501d0117a .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/IO_Config.o.d 
	@${RM} ${OBJECTDIR}/IO_Config.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  IO_Config.c  -o ${OBJECTDIR}/IO_Config.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/IO_Config.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/UART.o: UART.c  .generated_files/flags/default/50ca6e3f412da07a8da9cb8d9e9753317f23ef7b .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UART.o.d 
	@${RM} ${OBJECTDIR}/UART.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UART.c  -o ${OBJECTDIR}/UART.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UART.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/interupts.o: interupts.c  .generated_files/flags/default/3cbecdbdb35b03856ce4728191e8aa4fccf03f35 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/interupts.o.d 
	@${RM} ${OBJECTDIR}/interupts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interupts.c  -o ${OBJECTDIR}/interupts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/interupts.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/ADC.o: ADC.c  .generated_files/flags/default/68bf12699b90c535fa6be9f3d653c731435e42d7 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ADC.o.d 
	@${RM} ${OBJECTDIR}/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ADC.c  -o ${OBJECTDIR}/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ADC.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/RingBuffer.o: RingBuffer.c  .generated_files/flags/default/dc249aa3059723711ea43606ea5f12c40c70720d .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/RingBuffer.o.d 
	@${RM} ${OBJECTDIR}/RingBuffer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  RingBuffer.c  -o ${OBJECTDIR}/RingBuffer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/RingBuffer.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/DAC.o: DAC.c  .generated_files/flags/default/800ff82880da0c6ec752883e57ebc3d38cadac25 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DAC.o.d 
	@${RM} ${OBJECTDIR}/DAC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DAC.c  -o ${OBJECTDIR}/DAC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/DAC.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/timer.o: timer.c  .generated_files/flags/default/fbb0fad30b6a7e38d3b2c531a0103b1599cc83f8 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/timer.o.d 
	@${RM} ${OBJECTDIR}/timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  timer.c  -o ${OBJECTDIR}/timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/timer.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/Protocolo_Comm_Yaesu.o: Protocolo_Comm_Yaesu.c  .generated_files/flags/default/f50a1b0e8d0a32f60c85aecdbb4164aebab72871 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Protocolo_Comm_Yaesu.o.d 
	@${RM} ${OBJECTDIR}/Protocolo_Comm_Yaesu.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Protocolo_Comm_Yaesu.c  -o ${OBJECTDIR}/Protocolo_Comm_Yaesu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Protocolo_Comm_Yaesu.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
${OBJECTDIR}/IO_Accionamiento.o: IO_Accionamiento.c  .generated_files/flags/default/99963b09094806c852dc860736ba194e7cf79877 .generated_files/flags/default/bb9c332fb0c2e171c9cb4f01b7667c80b6e23b10
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/IO_Accionamiento.o.d 
	@${RM} ${OBJECTDIR}/IO_Accionamiento.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  IO_Accionamiento.c  -o ${OBJECTDIR}/IO_Accionamiento.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/IO_Accionamiento.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -I"../../../../../../../Program Files (x86)/Microchip/xc16/v1.50/support/dsPIC33F/h" -msmart-io=1 -Wall -msfr-warn=off   
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Ground_Station_Controller.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Ground_Station_Controller.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)   -mreserve=data@0x800:0x81F -mreserve=data@0x820:0x821 -mreserve=data@0x822:0x823 -mreserve=data@0x824:0x825 -mreserve=data@0x826:0x84F   -Wl,,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST)  
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/Ground_Station_Controller.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Ground_Station_Controller.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -Wl,,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST)  
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/Ground_Station_Controller.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf   
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
