#!/bin/sh -f

###########################################################################
#
# AMENDMENTS
#
###########################################################################


if [ "" != "$1" ]
then
   if [ "-i" = "$1" ]
   then
      echo "build_data.ksh 2.0A"
      exit 0

   elif [ "-h" == "$1" ]
   then
      echo 'build_data.ksh [<MACHINE> <PLATFORM>] > build_v.h'
      echo
      echo " To override the automatic setting set \"MACHINE\" "
      echo " and \"PLATFORM\" in the environment."
      exit 0

   else
      MACHINE=$1  ; export MACHINE
      PLATFORM=$2 ; export PLATFORM
   fi
fi



if [ -f "`which uname`" ]
then

SYSTEM_NAME="`uname`" ; export SYSTEM_NAME
echo '#define SIRE_UNAME    ' \"`uname`\"
echo '#define SIRE_HOST     ' \"`uname -n`\"
echo '#define SIRE_HARDWARE ' \"`uname -m`\"
echo '#define SIRE_UMRKISS  ' \"`uname -r`\"
echo '#define SIRE_UMARK    ' \"`uname -r | awk -F. '{print $1}'`\"
echo '#define SIRE_UISSUE   ' \"`uname -r | awk -F. '{print $2}'`\"
echo '#define SIRE_UREV     ' \"`uname -r | awk -F. '{print $3}'`\"

else

SYSTEM_NAME="UNKNOWN" ; export SYSTEM_NAME
echo '#define SIRE_UNAME    ' \"UNKNOWN\"
echo '#define SIRE_HOST     ' \"UNKNOWN\"
echo '#define SIRE_HARDWARE ' \"UNKNOWN\"
echo '#define SIRE_UMRKISS  ' \"UNKNOWN\"
echo '#define SIRE_UMARK    ' \"0\"
echo '#define SIRE_UISSUE   ' \"0\"
echo '#define SIRE_UREV     ' \"0\"

fi

if [ "" = "$MACHINE" ]
then
    case "$SYSTEM_NAME" in

    "Window_NT" ) 
             MACHINE=INTEL_NT ; export MACHINE
             PLATFORM=winnt   ; export PLATFORM
             ;;
    
    "SunOS" )
             setenv v `uname -r | awk -F. '{print $1}'`
             if [ $v -gt 4 ]
             then
                 MACHINE=SOLARIS  ; export MACHINE
                 PLATFORM=posix   ; export PLATFORM
             else
                 MACHINE=SUN4     ; export MACHINE
                 PLATFORM=unix    ; export PLATFORM
             fi 
             ;;
    
    "Linux" )
             case `uname -m` in

                "i386" )
                    MACHINE=INTEL_LINUX  ; export MACHINE
                    PLATFORM=posix       ; export PLATFORM
                ;;

                "i486" )
                    MACHINE=INTEL_LINUX  ; export MACHINE
                    PLATFORM=posix       ; export PLATFORM
                ;;

                "i586" )
                    MACHINE=INTEL_LINUX  ; export MACHINE
                    PLATFORM=posix       ; export PLATFORM
                ;;

                "i686" )
                    MACHINE=INTEL_LINUX  ; export MACHINE
                    PLATFORM=posix       ; export PLATFORM
                ;;

                "*" )
                    MACHINE=LINUX  ; export MACHINE
                    PLATFORM=unix ; export PLATFORM
                ;;
                
             esac
             ;;

    "dgux" ) 
             MACHINE=AVIION   ; export MACHINE
             PLATFORM=unix    ; export PLATFORM
             ;;

    "UNIX_System_V" )
             MACHINE=NEC_RISC   ; export MACHINE
             PLATFORM=unix      ; export PLATFORM
             ;;

    "ULTRIX" ) 
             MACHINE=DECSTATION ; export MACHINE
             PLATFORM=unix      ; export PLATFORM
             ;;

    "HP-UX" ) 
             MACHINE=HP700  ; export MACHINE
             PLATFORM=posix ; export PLATFORM
             ;;

    "AIX" )
             MACHINE=IBM    ; export MACHINE
             PLATFORM=unix  ; export PLATFORM
             ;;

    "mamips" ) 
             MACHINE=MIPS_V  ; export MACHINE
             PLATFORM=unix   ; export PLATFORM
             ;;

    "OSF1" ) 
             MACHINE=ALPHA_OSF ; export MACHINE
             PLATFORM=posix    ; export PLATFORM
             ;;

    "DomainIX" ) 
             MACHINE=APOLLO   ; export MACHINE
             PLATFORM=unix    ; export PLATFORM
             ;;

    "DomainOS" ) 
             MACHINE=APOLLO   ; export MACHINE
             PLATFORM=unix    ; export PLATFORM
             ;;

     * )
             MACHINE=UNKNOWN  ; export MACHINE
             PLATFORM=unknown ; export PLATFORM
             ;;
     esac
fi


if [ "" != "${OSTYPE}" ]
then
echo '#define SIRE_OSTYPE   ' \"${OSTYPE}\"
else
echo '#define SIRE_OSTYPE   ' \"${SYSTEM_NAME}\"
fi

if [ "" != "${MACHINE}" ]
then
echo '#define SIRE_MACHINE  ' \"${MACHINE}\"
echo "#define ${MACHINE}  1"
fi

if [ "" != "${PLATFORM}" ]
then
echo '#define SIRE_PLATFORM ' \"${PLATFORM}\"
else
    case "$MACHINE" in

    "INTEL_NT" )
        echo '#define SIRE_PLATFORM winnt'
             ;;

    "SOLARIS" | \
    "HP-UX"   | \
    "OSF1"    | \
    "Linux" )
        echo '#define SIRE_PLATFORM posix'
             ;;

    "SUN4"     | \
    "dgux"     | \
    "UNIX_System_V" | \
    "ULTRIX"   | \
    "AIX"      | \
    "mamips"   | \
    "DomainIX" | \
    "DomainOS" )
        echo '#define SIRE_PLATFORM unix'
             ;;

     * )
             MACHINE=UNKNOWN  ; export MACHINE
             PLATFORM=unknown ; export PLATFORM
             ;;
     esac
fi

echo '#define SIRE_DATE     ' \"`date '+%y/%m/%d %H:%M:%S'`\"
echo '#define SIRE_PATH     ' \"`pwd`\"
