#!/system/bin/sh
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You can find a full copy of the GPL license in many formats in the root 
# directory of the distribution containing this file under the name
# COPYING, COPYING.dbk, COPYING.html, COPYING.odt, COPYING.dbk.tex, 
# and COPYING.texi
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#
######################################################

# Pterodon Recovery device tree build engine
# Written from scratch by ATG Droid @xda
# If you use any part of this script anywhere else then
# don't forget to give credits to me, thank you :)
# Also be sure that your work is compatible with the 
# GPLV3+ license as mentioned at the beggining of this script




# End of the script license and related informations
# Start of the shell script


#script starts at 2381


#true()
#false()
#pterodon_function_test()
#usage()
#print()
#contains()
#startswith()
#endswith()
#parse_args()
#parse_args_get_complete_counter()
#parse_args_get_full_key()
#parse_args_ensure_argument()
#LOGI()
#LOGE()
#LOGW()
#script_abort()
#check_args()
#typemanager()
#is_64bit()
#get_str_by_pos()
#getdir()
#find_object()
#sequence()
#filesystem_supported()
#partition_exists()
#defaultfunc()
#get_executable_path_pure_bash()
#get_executable_path()
#exists()
#execute()
#file_size()
#get_running_os()
#get_current_day()
#random_number()
#get_device_boot_time()
#reboot_device()
#remove_file()
#dump_partition()
#relink_executable()
#get_configfs_controller()
#get_kernel_version()
#get_memory_info()
#get_hardware_info()
#get_cpu_core_number()
#is_root()
#set_active_cores()
#is_modern_ksh()
#get_pwd()
#file_md5()
#get_hash_alg()
#get_hash_value()
#get_global()
#get_vendor_prop()
#derive_build_fingerprint()
#make_executable()
#get_dependencies()
#running_as_root()
#grep_line()
#is_mounted()
#echo_command()
#get_fstab_path()
#write_apache_license()
#write_author_copyright()
#write_author_copyright_xml()
#save_property_files_info()
#get_custom_id()
#get_stock_kernel()
#copy_dependency_file()
#write_device_make_file()
#write_make_file()
#generate_boardconfig()
#write_boardconfig_partition_size()
#process_dependency()
#create_readme_file()
#process_proprietary_requirements()
#copy_unspecified_proprietary_blob()
#copy_unspecified_proprietary_files()
#copy_unspecified_proprietary_blob_sbin()
#copy_unspecified_proprietary_files_sbin()
#copy_unspecified_proprietary_blob_root()
#copy_unspecified_proprietary_files_root()
#copy_proprietary_files()
#get_display_value()
#get_device_encryption_mode()
#get_twrp_partition_flags()
#get_partition_info()
#comment()
#write_value()
#write_empty()
#initialize_values()
#check_root()
#log_device_info()
#vendor_load_properties()


#Set logic functions in case that they don't exist already
function true() { return 0; }
function false() { return 1; }

#Check for correct logic
true || exit 1;
false && exit 1;

#Reset result value
true

if [ "$?" != "0" ]; then
exit 1;
#Something is really wrong, so try to force endless loop instead
while true; do
done
fi

###########################

# Some simple syntax tests before we let the
# whole script to run, we don't want to end up
# in some troubles due to the incompatible interpreter

###########################

if [ "quoted_value_is_positive" ]; then
true
else
exit 1;
fi

if [ -z "" ]; then
true
else
exit 1;
fi

case some_random_variable in
*"random"*) 
true; ;;
*)
exit 1; ;;
esac

if [[ "pterodon_recovery" == "pterodon"* ]]; then
true
else
exit 1;
fi

function pterodon_function_test() {
local pterodon_integer=0;
(( pterodon_integer++ ))
(( pterodon_integer != 1 )) && exit 1;
(( pterodon_integer-- ))
(( pterodon_integer != 0 )) && exit 1;
(( pterodon_integer+= 5 ))
[ "$pterodon_integer" != "5" ] && exit 1;
}

pterodon_function_test

# Reset result
true

###########################

#Detect which shell we use and if script is compatible with it

if [ "$FISH_VERSION" ]; then
CT_SHELL="fish";
elif [ "$BASH_VERSION" ]; then
CT_SHELL="bash";
elif [ "$KSH_VERSION" ]; then
CT_SHELL="ksh";
elif [ "$ZSH_VERSION" ]; then
CT_SHELL="zsh";
elif [ "$ION_VERSION" ]; then
CT_SHELL="ion";
elif [ "$SHELL" ]; then
case $SHELL in
*"/bash")
CT_SHELL="bash";
 ;;
 *"/fish")
CT_SHELL="fish";
 ;;
 *"/ksh")
CT_SHELL="ksh";
 ;;
 *"/zsh")
CT_SHELL="zsh";
;;
*"/ion")
CT_SHELL="ion";
 ;;
esac;
fi

if [ "$CT_SHELL" != "ksh" ]; then
echo "Warning: This script might won't work on shells other than ksh";
if [ "$CT_SHELL" ]; then
echo "Your current shell is: $CT_SHELL";
echo "Proceed with caution!";
fi
echo "";
fi


#variables with prefix CT_ are default globals
CT_SCRIPT_GENERATOR_VERSION="1.0";
#Set to true for debugging
CT_SCRIPT_DEBUG_MODE="true";
CT_SCRIPT_LOCAL_PATH="$0";
CT_DT_VENDOR="omni";

usage() {
	echo -e "\Pterodon Recovery device tree build engine V${CT_SCRIPT_GENERATOR_VERSION}" 
	echo -e "\Created by ATG Droid @xda" 
	echo -e "\nUsage:\n $CT_SCRIPT_LOCAL_PATH [options..]\n"
	echo -e "Options:\n"
	echo -e "-c | --codename <codename> - set a device code name for device tree\n"
	echo -e "-i | --info <device tree path> - get a bunch of informations from device tree by its path\n"
	echo -e "-m | --maintainer <maintainer name> - set maintainer of this device tree\n"
	echo -e "-u | --update <folder path> - update device tree by path to folder\n"
	echo -e "-h | --help - Display usage instructions.\n" 
	exit 0;
}

print() {
echo -e "$@" >&2;
}

contains() {
case $1 in
*"$2"*) 
return 0; ;;
*)
return 1; ;;
esac
}

startswith() {
case $1 in
"$2"*) 
return 0; ;;
*)
return 1; ;;
esac
}

endswith() {
case $1 in
*"$2") 
return 0; ;;
*)
return 1; ;;
esac
}

parse_args() {
parse_args_get_complete_counter() {
if [ "$parse_args_long_arguments" ] && [ "$parse_args_argument_position" ]; then
return $((parse_args_argument_position - parse_args_long_arguments))
elif [ "$parse_args_argument_position" ]; then
return $parse_args_argument_position;
else
return 0;
fi
}
if [ -z "$1" ]; then
parse_args_argument_position=0;
parse_args_long_arguments=0;
return 1;
fi;
if [ -z "$parse_args_long_arguments" ]; then
for argument in "$@"; do
(( parse_args_long_arguments++ ))
done
parse_args_long_arguments=$(( $parse_args_long_arguments - $# ));
fi
local tmp_arg_pos;
local require_arg_value=false;
parse_args_value="";
parse_args_key="";
if [ "$parse_args_argument_position" ]; then
tmp_arg_pos=$parse_args_argument_position;
else
tmp_arg_pos=0;
fi;
for argument in "$@"; do
if ((tmp_arg_pos > 0)); then
(( tmp_arg_pos-- ))
continue;
fi;
(( parse_args_argument_position++ ))
local arg_size=${#argument}
if (( arg_size == 2 )); then
if [[ "$argument" == "-"* ]]; then
if $require_arg_value; then
(( parse_args_argument_position-- ))
return 0;
fi
parse_args_key="${argument#?}";
parse_args_get_complete_counter;
if [ "$#" -eq $? ]; then
parse_args_value="";
return 0;
fi
require_arg_value=true;
continue;
fi
elif (( arg_size >= 3 )) && [[ "$argument" == "--"* ]]; then
if $require_arg_value; then
(( parse_args_argument_position-- ))
return 0;
fi
parse_args_key="${argument#??}";
parse_args_get_complete_counter;
if [ "$#" -eq $? ]; then
parse_args_value="";
return 0;
fi
require_arg_value=true;
continue;
fi
if $require_arg_value; then
if [ "$parse_args_value" ]; then
parse_args_value="$parse_args_value $argument";
else
parse_args_value="$argument";
fi
parse_args_get_complete_counter;
if [ "$#" -eq $? ]; then
return 0;
fi
continue;
fi
done;
parse_args_argument_position=0;
parse_args_long_arguments=0;
return 1;
}

parse_args_get_full_key() {
local parse_args_key_size=${#parse_args_key}
local full_argument;
if (( parse_args_key_size == 1 )); then
full_argument="-${parse_args_key}";
elif (( parse_args_key_size > 1 )); then
full_argument="--${parse_args_key}";
fi
if (( parse_args_key_size >= 1 )); then
   echo "\"$full_argument\"";
else
   echo "$parse_args_key";
fi
}

parse_args_ensure_argument() {
if [ -z "$parse_args_value" ]; then
   print "Please specify value for `parse_args_get_full_key`\nUse --help if needed.";
   exit 1;
 fi
}

while parse_args "$@"; do
case $parse_args_key in
		c | codename)
		    parse_args_ensure_argument
			CT_DEVICE_CODENAME="$parse_args_value";
			;;
		i | info)
		    parse_args_ensure_argument
			CT_INFO_PATH="$parse_args_value";
			;;
	    m | maintainer)
		    parse_args_ensure_argument;
			CT_MAINTAINER="$parse_args_value";
			;;
		u | update)
		    parse_args_ensure_argument;
			CT_UPDATE_PATH="$parse_args_value";
			;;
		h | help)
			usage
			;;
		 *)
			print "Unsupported argument: `parse_args_get_full_key`\nUse --help if needed.";
			exit 1;
			;;
	esac
done

if [ -z "$CT_INFO_PATH" ] && [ -z "$CT_UPDATE_PATH" ] && [ -z "$CT_MAINTAINER" ]; then
	print "Set a maintainer name if you want to build new device tree"
	exit 1
elif [ "$CT_INFO_PATH" ] && [ "$CT_UPDATE_PATH" ]; then
    print "Can't gather info from device tree and update it in the same time,\nplease select only one option"
	exit 1
fi

LOGI() {
echo "[INFO]: $@" >> "$WORKDIR/builder.log";
}

LOGE() {
print "$@";
echo "[ERROR]: $@" >> "$WORKDIR/builder.log";
}

LOGW() {
echo "[WARN]: $@" >> "$WORKDIR/builder.log";
}

script_abort() {
LOGE "Aborting...";
wait;
exit 1;
}

check_args() {
if (($# < 3 )); then
    LOGE "function 'check_args' received unsupported number of arguments!";
    LOGE "Expected at least: '3' but received: '$#'";
    script_abort;
fi
if ((($# - 2) < $2 )); then
    LOGE "function '$1' received unsupported number of arguments!";
    LOGE "Expected at least: '$2' but received: '$#'";
    script_abort;
fi
}

typemanager() {
local characters;
if [ "$1" = "lower" ]; then
characters="A B C D E F G H I J K L M N O Q R S T U V W X Y Z";
elif [ "$1" = "upper" ]; then
characters="a b c d e f g h i j k l m n o p q r s t u v w x y z";
else
return 1;
fi
local replace="";
local ret="$2";
for character in $characters; do
if [[ "$ret" == *"$character"* ]]; then
case $character in
*a*) replace="A";;
*b*) replace="B";;
*c*) replace="C";;
*d*) replace="D";;
*e*) replace="E";;
*f*) replace="F";;
*g*) replace="G";;
*h*) replace="H";;
*i*) replace="I";;
*j*) replace="J";;
*k*) replace="K";;
*l*) replace="L";;
*m*) replace="M";;
*n*) replace="N";;
*o*) replace="O";;
*p*) replace="P";;
*q*) replace="Q";;
*r*) replace="R";;
*s*) replace="S";;
*t*) replace="T";;
*u*) replace="U";;
*v*) replace="V";;
*w*) replace="W";;
*x*) replace="X";;
*y*) replace="Y";;
*z*) replace="Z";;
*A*) replace="a";;
*B*) replace="b";;
*C*) replace="c";;
*D*) replace="d";;
*E*) replace="e";;
*F*) replace="f";;
*G*) replace="g";;
*H*) replace="h";;
*I*) replace="i";;
*J*) replace="j";;
*K*) replace="k";;
*L*) replace="l";;
*M*) replace="m";;
*N*) replace="n";;
*O*) replace="o";;
*Q*) replace="q";;
*R*) replace="r";;
*S*) replace="s";;
*T*) replace="t";;
*U*) replace="u";;
*V*) replace="v";;
*W*) replace="w";;
*X*) replace="x";;
*Y*) replace="y";;
*Z*) replace="z";;
esac;
if [ "$replace" ]; then
ret=${ret//$character/$replace};
fi
fi
done;
echo "$ret";
}

is_64bit() {
#armv7 and lower is 32bit
uname -m
}

get_str_by_pos() {
check_args get_str_by_pos 1 $@;
arr=($1)
echo ${arr[$2-1]}
}

# Pure bash dirname implementation
getdir() {
  check_args getdir 1 $@;
  case "$1" in
    */*) local dir=${1%/*}; [ -z $dir ] && echo "/" || echo $dir ;;
    *) echo "." ;;
  esac
}

find_object() {
check_args find_object 3 $@;
 for i in "$1"/*; do
    if [ -f "$i" ]; then
    if [ "$3" = "file" ] && [ "${i##*/}" = "$2" ]; then
    echo "$i";
    return 0;
    fi
    fi
 done
 for i in "$1"/*; do
    if [ -d "$i" ]; then
    if [ "$3" = "dir" ] && [ "${i##*/}" = "$2" ]; then
    echo "$i";
    return 0;
    fi
    if find_object "$i" "$2" "$3"; then
    return 0;
    fi
    fi
 done
 return 1;
}

#pure bash seq implementation
sequence() {
check_args sequence 1 $@;
local i=1;
local counter=0;
if [ "$2" ]; then
i=$1;
counter=$2;
else
counter=$1;
fi

while ((i<=counter)); do
echo "$i";
(( i++ ))
done
}

filesystem_supported() {
local filesystem_line
while read -r filesystem_line; do
case $filesystem_line in
*"${1}")
return 0;
 ;;
esac;
done < "/proc/filesystems";
return 1;
}

partition_exists() {
check_args partition_exists 1 $@;
for i in $PARTITIONS_PATH/*; do
if [ "$i" = "$1" ]; then
return 0;
fi;
done
return 1;
}

defaultfunc() {
local executable
if [ "${KSH_VERSION}" ]; then
executable="`whence $1`";
if [ "$executable" ]; then
case "$executable" in
*"/"*) 
return 1; ;;
*)
return 0; ;;
esac
fi;
fi
executable="`command -v $1`";
if [ "$executable" ]; then
case "$executable" in
*"/"*) 
return 1; ;;
*)
return 0; ;;
esac
fi;
return 1;
}

get_executable_path_pure_bash() {
[ -z "$1" ] && return 1;
if [ "$PATH" ]; then
for i in ${PATH//:/ }; do
if [ -x "$i/$1" ]; then
echo "$i/$1";
return 0;
fi;
done
fi;
if [ "$LD_LIBRARY_PATH" ]; then
for i in ${LD_LIBRARY_PATH//:/ }; do
if [ -x "$i/$1" ]; then
echo "$i/$1";
return 0;
fi;
done
fi;
local exec_lib_paths="
/system/bin
/vendor/bin
/system/xbin
/sbin
/bin
";
for i in $exec_lib_paths; do
if [ -x "$i/$1" ]; then
echo "$i/$1";
return 0;
fi;
done;
return 1;
}

get_executable_path() {
defaultfunc "$1" && echo "$1" && return 0;
local executable i;
if [ "${KSH_VERSION}" ]; then
executable="`whence $1`";
if [ "$executable" ] && [ -x "$executable" ]; then
echo "$executable";
return 0;
fi;
fi
executable="`command -v $1`";
if [ "$executable" ] && [ -x "$executable" ]; then
echo "$executable";
return 0;
fi;
get_executable_path_pure_bash "$1" && return 0;
# we also might just execute which at this point without any check
if get_executable_path_pure_bash "which"; then
executable="`which $1`";
if [ "$executable" ] && [ -x "$executable" ]; then
echo "$executable";
return 0;
fi;
fi
return 1;
}

exists() {
get_executable_path "$@" >>/dev/null 2>&1
return $?;
}

execute() {
check_args execute 1 $@;
local exec_path="$(get_executable_path $1)";
if [ "$exec_path" ]; then
case "$exec_path" in
*"/"*) 
exec_path=${exec_path%/*};
eval "${exec_path}/$@";
;;
*) 
eval "${exec_path} $@";
;;
esac
return $?
else
return 1;
fi
}

file_size() {
if [ "$1" ] && [ -f "$1" ]; then
local FILESIZE;
local exec="`get_executable_path stat`";
if [ "$exec" ]; then
FILESIZE="`eval $exec -c%s "$1"`";
if [ "$FILESIZE" ]; then
echo "$FILESIZE";
return 0;
else
return 1;
fi
fi
exec="`get_executable_path wc`";
if [ "$exec" ]; then
FILESIZE="`eval $exec -c "$1"`"
FILESIZE=${FILESIZE% *};
if [ "$FILESIZE" ]; then
echo "$FILESIZE";
return 0;
else
return 1;
fi
fi
exec="`get_executable_path ls`";
if [ "$exec" ]; then
FILESIZE="`eval $exec -l "$1"`"
FILESIZE=$(get_str_by_pos "$FILESIZE" 5);
if [ "$FILESIZE" ]; then
echo "$FILESIZE";
return 0;
else
return 1;
fi
fi
# this isn't in bytes
exec="`get_executable_path du`";
if [ "$exec" ]; then
FILESIZE="`eval $exec -h "$1"`"
FILESIZE=$(get_str_by_pos "$FILESIZE" 1);
if [ "$FILESIZE" ]; then
echo "$FILESIZE";
return 0;
else
return 1;
fi
fi
fi;
return 1;
} 

get_running_os() {
if [ "$OSTYPE" ]; then
case "$OSTYPE" in
  linux*)   echo "Linux / WSL" ;;
  darwin*)  echo "Mac OS" ;; 
  win*)     echo "Windows" ;;
  msys*)    echo "MSYS / MinGW / Git Bash" ;;
  cygwin*)  echo "Cygwin" ;;
  bsd*)     echo "BSD" ;;
  solaris*) echo "Solaris" ;;
  android*) echo "Android" ;;
  *)        echo "unknown: $OSTYPE" ;;
esac
return 0;
fi
local uname_path="`get_executable_path uname`";
if [ "$uname_path" ]; then
eval "$uname_path";
return 0;
fi
return 1;
}

get_current_day() {
local date_path="`get_executable_path date`";
if [ "$uname_path" ]; then
local DOW=$(date +%u)
local ret="";
case $DOW in
1) ret="Monday";;
2) ret="Tuesday";;
3) ret="Wednesday";;
4) ret="Thursday";;
5) ret="Friday";;
6) ret="Saturday";;
7) ret="Sunday";;
*) ret="*unknown*";;
esac;
echo "$ret";
return 0;
fi
return 1;
}

random_number() {
if [ "$1" ]; then
echo $(($RANDOM % $1))
else
echo $RANDOM;
fi
}

get_device_boot_time() {
date -d @$(( $(date +%s) - $(cut -f1 -d. /proc/uptime) ))
}

reboot_device() {
local reboot_exec="`get_executable_path reboot`";
if [ "$reboot_exec" ]; then
eval "$reboot_exec $@";
return 0;
fi
reboot_exec="`get_executable_path setprop`";
if [ "$reboot_exec" ]; then
eval "$reboot_exec sys.powerctl reboot,$@";
return 0;
fi
return 1;
}

remove_file() {
local delete_exec="`get_executable_path rm`";
if [ "$delete_exec" ]; then
eval "$delete_exec -f $@";
return 0;
fi
delete_exec="`get_executable_path unlink`";
if [ "$delete_exec" ]; then
eval "$delete_exec $@";
return 0;
fi
return 1;
}

dump_partition() {
local exec="`get_executable_path dd`";
if [ "$exec" ]; then
eval $exec if="$1" of="$2";
return $?
fi
exec="`get_executable_path cat`";
if [ "$exec" ]; then
eval $exec "$1" > "$2";
return $?;
fi
return 1;
}

relink_executable() {
check_args relink_executable 3 $@;
local actual_command="sed -i";
local dir_len=${#$2}
local linker_len=${#$3}
actual_command+=" \"s|${2}|";
while (( dir_len > linker_len )); do
actual_command+="/";
(( dir_len-- ))
done
actual_command+="${3}|g\" $1";
if grep -ali "$2" "$1"; then
echo "Relinking $2 to $1"
eval "$actual_command"
fi
}

get_configfs_controller() {
[ -d "/sys/class/udc" ] || return 1;
for controllers in /sys/class/udc/*; do
echo "${controllers##*/}"
return 0;
done
return 1;
}

get_kernel_version() {
while read -r line; do
echo $line
done < "/proc/version";
}

get_memory_info() {
local line_pos=0
while read -r line; do
(( line_pos++ ))
(( line_pos == 4 )) && return 0;
echo $line
done < "/proc/meminfo";
}

get_hardware_info() {
while read -r line; do
case $line in
"Hardware"*)
echo "$line"
 ;;
esac;
done < "/proc/cpuinfo";
}

get_cpu_core_number() {
local core_num=0
while read -r line; do
case $line in
"processor"*)
(( core_num++ ))
 ;;
esac;
done < "/proc/cpuinfo";
echo $core_num
}

is_root() {
for root_word in `ps -p $$`; do
[ "$root_word" = "root" ] && return 0;
done
return 1;
}


set_active_cores() {
local current_core_pos=0;
while (( current_core_pos < $1 )); do
if [ -f "/sys/devices/system/cpu/cpu${current_core_pos}/online" ]; then
echo "/sys/devices/system/cpu/cpu${current_core_pos}/online";
fi
(( current_core_pos++ ))
done
}

is_modern_ksh() {
    if [ -z "${KSH_VERSION}" ]; then
       return 1;
    fi

  if whence -a whence > /dev/null 2>&1 ; then
      return 0;
  else
      return 1;
  fi
}

get_pwd() {
if [ "$PWD" ]; then
echo "$PWD";
else
local exec="`get_executable_path pwd`";
if [ "$exec" ]; then
echo "`eval $exec`";
return 0;
fi
exec="`get_executable_path readlink`";
if [ "$exec" ]; then
echo "`eval $exec -f .`";
return 0;
fi
fi
return 1;
}

file_md5() {
if [ -f "$1" ]; then
local file_md5_sum="`md5sum ${1}`";
file_md5_sum=${file_md5_sum% *};
echo $file_md5_sum
else
echo "unknown";
fi
}

get_hash_alg() {
local hash_algorithms="
md5sum
sha256sum
sha512sum
sha224sum
sha384sum
sha1sum
cksum
";
local ret;
for algorithm in $hash_algorithms; do
ret="`get_executable_path $algorithm`";
[ "$ret" ] && echo "$algorithm" && return 0;
done
return 1;
}

get_hash_value() {
if [ "$1" ] && [ -f "$1" ]; then
local ret="`get_hash_alg`";
if [ "$ret" ]; then
ret="`get_executable_path $ret`";
if [ "$ret" ]; then
ret="`eval $ret "$1"`";
ret=${ret% *};
echo "$ret"
return 0;
fi
fi
fi
return 1;
}

get_global() {
while true; do
if [ "$getprop_path" ]; then
local ret="`eval $getprop_path "$1"`";
echo "$ret";
return 0;
else
getprop_path="`get_executable_path getprop`";
if [ "$getprop_path" ]; then
continue;
fi
fi
break;
done

local PROPERTYFILE_PATHS="
/default.prop
/vendor/default.prop
/system/build.prop
/vendor/build.prop
/product/build.prop
/system/vendor/build.prop
/system/product/build.prop
/system/system_ext/build.prop
/odm/build.prop
/odm/etc/build.prop
/vendor/odm/build.prop
/vendor/odm/etc/build.prop
";

for property_file in $PROPERTYFILE_PATHS; do
[ -r $property_file ] || continue;
while read -r property_file_line; do
if [[ "$property_file_line" != "#"* ]] && [[ "$property_file_line" == *"="* ]] && [ "${property_file_line%=*}" = "$1" ]; then
echo "${property_file_line##*=}";
return 0;
fi
done < $property_file
done;

local env_value;
eval "env_value=\$$1" 

if [ "$env_value" ]; then
echo "$env_value";
return 0;
fi

local cmdline_key="";
local cmdline_val="";
if [ -r "/proc/cmdline" ]; then
while read -r cmdline_line; do
for line_value in $cmdline_line; do
if [[ "$line_value" == *"="* ]] && [[ "$line_value" != *"=\""* ]]; then
if [ "$cmdline_key" ] && [ "$cmdline_val" ]; then
[ "$cmdline_key" = "$1" ] && echo "$cmdline_val" && return 0;
cmdline_key="";
cmdline_val="";
fi
cmdline_key=${line_value%%=*}
cmdline_val=${line_value#*=}
[ "$cmdline_key" = "$1" ] && echo "$cmdline_val" && return 0;
cmdline_key="";
cmdline_val="";
elif [[ "$line_value" == *"=\""* ]]; then
cmdline_key=${line_value%%=*}
cmdline_val=${line_value#*=}
else
cmdline_val="$cmdline_val $line_value";
fi
done
done < "/proc/cmdline";
if [ "$cmdline_key" ] && [ "$cmdline_val" ]; then
[ "$cmdline_key" = "$1" ] && echo "$cmdline_val" && return 0;
fi
fi
echo "";
return 1;
}

get_vendor_prop() {
if [ "${CONST_VENDOR_PROPERTY}" ]; then
echo "${CONST_VENDOR_PROPERTY}";
return 0;
fi
CONST_VENDOR_PROPERTY="`get_global ro.product.brand`";
if [ "${CONST_VENDOR_PROPERTY}" ]; then
echo "${CONST_VENDOR_PROPERTY}";
return 0;
fi
return 1;
}

derive_build_fingerprint() {
local ret="`get_vendor_prop`";
ret+="/`get_global ro.product.name`";
ret+="/`get_global ro.product.device`";
ret+=":`get_global ro.build.version.release`";
ret+="/`get_global ro.build.id`";
ret+="/`get_global ro.build.version.incremental`";
ret+=":`get_global ro.build.type`";
ret+="/`get_global ro.build.tags`";
echo "$ret";
}

make_executable() {
if [ -f "$1" ] && [ ! -x "$1" ]; then
local executable_path="`get_executable_path chmod`";
if [ "$executable_path" ]; then
eval "$executable_path +x $1"
else
return 1;
fi
fi
return 0;
}

get_dependencies() {
if [ "$readelf_path" ]; then
print "$1"
local found_library=false;
local ret=1;
for shared_library in `eval $readelf_path -d $1 2> /dev/null`; do
if $found_library && [[ "$shared_library" == "["* ]] && [[ "$shared_library" == *"]" ]]; then
shared_library="${shared_library#?}";
shared_library="${shared_library%?}";
found_library=false;
(( ret == 1 )) && ret=0;
echo "$shared_library";
elif [ "$shared_library" = "(NEEDED)" ]; then
found_library=true;
fi
done;
return $ret
fi;
return 1;
}

running_as_root() {
if [ "$USER_ID" ]; then
return $(( $USER_ID == 0 ? 0 : 1 ));
elif [ "$EUID" ]; then
return $(( $EUID == 0 ? 0 : 1 ));
elif [ "`execute id -u`" = "0" ]; then
return 0;
elif [ "`execute whoami`" = "root" ]; then
return 0;
else
local ps_path="`get_executable_path ps`";
if [ "$ps_path" ]; then
for root_word in `execute ps -p $$`; do
[ "$root_word" = "root" ] && return 0;
done
fi
fi
return 1;
}

grep_line() {
if [ ! "$1" ]; then
print "grep_line require at least one argument!";
exit 1;
fi
local first_arg;
while read -r line; do
first_arg=true;
for local_args in "$@"; do
$first_arg && first_arg=false && continue;
[[ "$line" == *"$local_args"* ]] && echo "$line" && return 0;
done
done < "$1"
return 1;
}

is_mounted() {
  if exists stat; then
  local first_dev_id="`execute stat -c %d "$1/."`";
  local second_dev_id="`execute stat -c %d "$1/.."`";
  return $(( first_dev_id != second_dev_id ? 0 : 1 ));
  elif exists readlink; then
  grep_line /proc/mounts " `execute readlink -f $1` " &>/dev/null
  return $?
  fi
  return 1;
}

echo_command() {
local ret="$";
ret="$ret(";
ret="$ret$@";
ret="$ret)";
echo "$ret"
}

get_fstab_path() {
local hardware_prop="`get_global ro.hardware`";
if [ -z "$hardware_prop" ]; then
hardware_prop="`get_global ro.hardware.platform`";
fi
# As per fs_mgr 
local fstab_paths="
/odm/etc/
/vendor/etc/
/
";

#Use ro.hardware property when possible
if [ "$hardware_prop" ]; then
for fstab in $fstab_paths; do
if [ -f "${fstab}fstab.${hardware_prop}" ]; then
echo "${fstab}fstab.${hardware_prop}";
return 0;
fi
done
fi

for path_link in ${fstab_paths}; do
for fstab in ${path_link}fstab.*; do
if [ -f "$fstab" ]; then
echo "$fstab";
return 0;
fi
done
done;

return 1;
}


write_apache_license() {
echo "                                 Apache License
                           Version 2.0, January 2004
                        http://www.apache.org/licenses/

   TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION

   1. Definitions.

      \"License\" shall mean the terms and conditions for use, reproduction,
      and distribution as defined by Sections 1 through 9 of this document.

      \"Licensor\" shall mean the copyright owner or entity authorized by
      the copyright owner that is granting the License.

      \"Legal Entity\" shall mean the union of the acting entity and all
      other entities that control, are controlled by, or are under common
      control with that entity. For the purposes of this definition,
      \"control\" means (i) the power, direct or indirect, to cause the
      direction or management of such entity, whether by contract or
      otherwise, or (ii) ownership of fifty percent (50%) or more of the
      outstanding shares, or (iii) beneficial ownership of such entity.

      \"You\" (or \"Your\") shall mean an individual or Legal Entity
      exercising permissions granted by this License.

      \"Source\" form shall mean the preferred form for making modifications,
      including but not limited to software source code, documentation
      source, and configuration files.

      \"Object\" form shall mean any form resulting from mechanical
      transformation or translation of a Source form, including but
      not limited to compiled object code, generated documentation,
      and conversions to other media types.

      \"Work\" shall mean the work of authorship, whether in Source or
      Object form, made available under the License, as indicated by a
      copyright notice that is included in or attached to the work
      (an example is provided in the Appendix below).

      \"Derivative Works\" shall mean any work, whether in Source or Object
      form, that is based on (or derived from) the Work and for which the
      editorial revisions, annotations, elaborations, or other modifications
      represent, as a whole, an original work of authorship. For the purposes
      of this License, Derivative Works shall not include works that remain
      separable from, or merely link (or bind by name) to the interfaces of,
      the Work and Derivative Works thereof.

      \"Contribution\" shall mean any work of authorship, including
      the original version of the Work and any modifications or additions
      to that Work or Derivative Works thereof, that is intentionally
      submitted to Licensor for inclusion in the Work by the copyright owner
      or by an individual or Legal Entity authorized to submit on behalf of
      the copyright owner. For the purposes of this definition, \"submitted\"
      means any form of electronic, verbal, or written communication sent
      to the Licensor or its representatives, including but not limited to
      communication on electronic mailing lists, source code control systems,
      and issue tracking systems that are managed by, or on behalf of, the
      Licensor for the purpose of discussing and improving the Work, but
      excluding communication that is conspicuously marked or otherwise
      designated in writing by the copyright owner as \"Not a Contribution.\"

      \"Contributor\" shall mean Licensor and any individual or Legal Entity
      on behalf of whom a Contribution has been received by Licensor and
      subsequently incorporated within the Work.

   2. Grant of Copyright License. Subject to the terms and conditions of
      this License, each Contributor hereby grants to You a perpetual,
      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
      copyright license to reproduce, prepare Derivative Works of,
      publicly display, publicly perform, sublicense, and distribute the
      Work and such Derivative Works in Source or Object form.

   3. Grant of Patent License. Subject to the terms and conditions of
      this License, each Contributor hereby grants to You a perpetual,
      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
      (except as stated in this section) patent license to make, have made,
      use, offer to sell, sell, import, and otherwise transfer the Work,
      where such license applies only to those patent claims licensable
      by such Contributor that are necessarily infringed by their
      Contribution(s) alone or by combination of their Contribution(s)
      with the Work to which such Contribution(s) was submitted. If You
      institute patent litigation against any entity (including a
      cross-claim or counterclaim in a lawsuit) alleging that the Work
      or a Contribution incorporated within the Work constitutes direct
      or contributory patent infringement, then any patent licenses
      granted to You under this License for that Work shall terminate
      as of the date such litigation is filed.

   4. Redistribution. You may reproduce and distribute copies of the
      Work or Derivative Works thereof in any medium, with or without
      modifications, and in Source or Object form, provided that You
      meet the following conditions:

      (a) You must give any other recipients of the Work or
          Derivative Works a copy of this License; and

      (b) You must cause any modified files to carry prominent notices
          stating that You changed the files; and

      (c) You must retain, in the Source form of any Derivative Works
          that You distribute, all copyright, patent, trademark, and
          attribution notices from the Source form of the Work,
          excluding those notices that do not pertain to any part of
          the Derivative Works; and

      (d) If the Work includes a \"NOTICE\" text file as part of its
          distribution, then any Derivative Works that You distribute must
          include a readable copy of the attribution notices contained
          within such NOTICE file, excluding those notices that do not
          pertain to any part of the Derivative Works, in at least one
          of the following places: within a NOTICE text file distributed
          as part of the Derivative Works; within the Source form or
          documentation, if provided along with the Derivative Works; or,
          within a display generated by the Derivative Works, if and
          wherever such third-party notices normally appear. The contents
          of the NOTICE file are for informational purposes only and
          do not modify the License. You may add Your own attribution
          notices within Derivative Works that You distribute, alongside
          or as an addendum to the NOTICE text from the Work, provided
          that such additional attribution notices cannot be construed
          as modifying the License.

      You may add Your own copyright statement to Your modifications and
      may provide additional or different license terms and conditions
      for use, reproduction, or distribution of Your modifications, or
      for any such Derivative Works as a whole, provided Your use,
      reproduction, and distribution of the Work otherwise complies with
      the conditions stated in this License.

   5. Submission of Contributions. Unless You explicitly state otherwise,
      any Contribution intentionally submitted for inclusion in the Work
      by You to the Licensor shall be under the terms and conditions of
      this License, without any additional terms or conditions.
      Notwithstanding the above, nothing herein shall supersede or modify
      the terms of any separate license agreement you may have executed
      with Licensor regarding such Contributions.

   6. Trademarks. This License does not grant permission to use the trade
      names, trademarks, service marks, or product names of the Licensor,
      except as required for reasonable and customary use in describing the
      origin of the Work and reproducing the content of the NOTICE file.

   7. Disclaimer of Warranty. Unless required by applicable law or
      agreed to in writing, Licensor provides the Work (and each
      Contributor provides its Contributions) on an \"AS IS\" BASIS,
      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
      implied, including, without limitation, any warranties or conditions
      of TITLE, NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A
      PARTICULAR PURPOSE. You are solely responsible for determining the
      appropriateness of using or redistributing the Work and assume any
      risks associated with Your exercise of permissions under this License.

   8. Limitation of Liability. In no event and under no legal theory,
      whether in tort (including negligence), contract, or otherwise,
      unless required by applicable law (such as deliberate and grossly
      negligent acts) or agreed to in writing, shall any Contributor be
      liable to You for damages, including any direct, indirect, special,
      incidental, or consequential damages of any character arising as a
      result of this License or out of the use or inability to use the
      Work (including but not limited to damages for loss of goodwill,
      work stoppage, computer failure or malfunction, or any and all
      other commercial damages or losses), even if such Contributor
      has been advised of the possibility of such damages.

   9. Accepting Warranty or Additional Liability. While redistributing
      the Work or Derivative Works thereof, You may choose to offer,
      and charge a fee for, acceptance of support, warranty, indemnity,
      or other liability obligations and/or rights consistent with this
      License. However, in accepting such obligations, You may act only
      on Your own behalf and on Your sole responsibility, not on behalf
      of any other Contributor, and only if You agree to indemnify,
      defend, and hold each Contributor harmless for any liability
      incurred by, or claims asserted against, such Contributor by reason
      of your accepting any such warranty or additional liability.

   END OF TERMS AND CONDITIONS

   APPENDIX: How to apply the Apache License to your work.

      To apply the Apache License to your work, attach the following
      boilerplate notice, with the fields enclosed by brackets \"[]\"
      replaced with your own identifying information. (Don't include
      the brackets!)  The text should be enclosed in the appropriate
      comment syntax for the file format. We also recommend that a
      file or class name and description of purpose be included on the
      same \"printed page\" as the copyright notice for easier
      identification within third-party archives.

   Copyright [yyyy] [name of copyright owner]

   Licensed under the Apache License, Version 2.0 (the \"License\");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an \"AS IS\" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License." > $1;
}

write_author_copyright() {
 echo "#" >> $1;
 echo "# Copyright (C) `date +%Y` ATGDroid <bythedroid@gmail.com>" >> $1;
 echo "#" >> $1;
 echo "# Licensed under the Apache License, Version 2.0 (the \"License\");" >> $1;
 echo "# you may not use this file except in compliance with the License." >> $1;
 echo "# You may obtain a copy of the License at" >> $1;
 echo "#" >> $1;
 echo "#      http://www.apache.org/licenses/LICENSE-2.0" >> $1;
 echo "#" >> $1;
 echo "# Unless required by applicable law or agreed to in writing, software" >> $1;
 echo "# distributed under the License is distributed on an \"AS IS\" BASIS," >> $1;
 echo "# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied." >> $1;
 echo "# See the License for the specific language governing permissions and" >> $1;
 echo "# limitations under the License." >> $1;
 echo "#" >> $1;
 echo "" >> $1;
}

write_author_copyright_xml() {
echo "<!--
 Copyright (C) `date +%Y` ATGDroid <bythedroid@gmail.com>

 Licensed under the Apache License, Version 2.0 (the \"License\");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an \"AS IS\" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
-->
" >> "$1";
}

save_property_files_info() {
local PROPERTYFILE_PATHS="
/system/build.prop
/vendor/build.prop
/vendor/default.prop
/product/build.prop
";

echo "<?xml version=\"1.0\"?>" >> "$1";
write_author_copyright_xml "$1";
echo "<!-- Generated based on `derive_build_fingerprint` -->" >> "$1";
echo "<properties>" >> "$1";
echo "    <file_properties>" >> "$1";
for i in $PROPERTYFILE_PATHS; do
[ -r "$i" ] || continue;
echo "" >> "$1"
echo "        <file path=\"$i\">" >> "$1";
while read -r line; do
if [[ "$line" != "#"* ]] && [[ "$line" = *"="* ]] then
echo "                <property name=\"${line%=*}\" value=\"${line##*=}\"/>" >> "$1";
fi
done < $i
echo "        </file>" >> "$1";
done;
echo -e "\n    </file_properties>" >> "$1";
echo -n "</properties>" >> "$1";
}

get_custom_id() {
[ -e "/proc/sys/kernel/random/uuid" ] || return;
while read -r line; do
echo "`typemanager upper $line`";
return;
done < /proc/sys/kernel/random/uuid;
}

get_stock_kernel() {
if [ -f "/data/local/AIK-mobile/unpackimg.sh" ]; then
dd if="/dev/block/bootdevice/by-name/boot" of="/data/local/AIK-mobile/boot_dump.img" > /dev/null 2>&1;
/data/local/AIK-mobile/unpackimg.sh /data/local/AIK-mobile/boot_dump.img > /dev/null 2>&1
rm "/data/local/AIK-mobile/boot_dump.img"
cp /data/local/AIK-mobile/split_img/boot_dump.img-zImage "$WORKDIR/prebuilt/Image.gz-dtb";
fi
}

#todo: Make relink $PATH based, so that it's not hardcoded
relink()
{
    if ! grep -qs 'system/bin/linker64' $1; then
    return 1;
    fi
    LOGI "Relinking $2 from /system/bin/linker64 to /sbin/linker64";
    echo "$2" >> "$PROPRIETARY_FILES_RELINK";
	cp $1 ${1}_tmp
	sed 's|/system/bin/linker64|///////sbin/linker64|' "${1}_tmp" > "$1"
	rm ${1}_tmp
	return 0;
}

copy_dependency_file() {
local dependency_path="";
local device_tree_path="";
if [ -f "/vendor/lib64/$1" ]; then
if [ -f "${WORKDIR}/recovery/root/vendor/lib64/$1" ]; then
return;
fi
cp -f "/vendor/lib64/$1" "${WORKDIR}/recovery/root/vendor/lib64/$1";
if [ $? -eq 0 ]; then
dependency_path="/vendor/lib64/$1";
device_tree_path="${WORKDIR}/recovery/root/vendor/lib64/$1";
fi;
elif [ -f "/vendor/lib64/hw/$1" ]; then
if [ -f "${WORKDIR}/recovery/root/vendor/lib64/hw/$1" ]; then
return;
fi
cp -f "/vendor/lib64/hw/$1" "${WORKDIR}/recovery/root/vendor/lib64/hw/$1";
if [ $? -eq 0 ]; then
dependency_path="/vendor/lib64/hw/$1";
device_tree_path="${WORKDIR}/recovery/root/vendor/lib64/hw/$1";
fi;
elif [ -f "/system/lib64/$1" ]; then
if [ -f "${WORKDIR}/recovery/root/sbin/$1" ]; then
return;
fi
cp -f "/system/lib64/$1" "${WORKDIR}/recovery/root/sbin/$1";
if [ $? -eq 0 ]; then
dependency_path="/system/lib64/$1";
device_tree_path="${WORKDIR}/recovery/root/sbin/$1";
fi;
elif [ -f "/vendor/bin/$1" ]; then
if [ -f "${WORKDIR}/recovery/root/sbin/$1" ]; then
return;
fi
cp -f "/vendor/bin/$1" "${WORKDIR}/recovery/root/sbin/$1";
if [ $? -eq 0 ]; then
dependency_path="/vendor/bin/$1";
device_tree_path="${WORKDIR}/recovery/root/sbin/$1";
fi;
elif [ -f "/system/bin/$1" ]; then
if [ -f "${WORKDIR}/recovery/root/sbin/$1" ]; then
return;
fi
cp -f "/system/bin/$1" "${WORKDIR}/recovery/root/sbin/$1";
if [ $? -eq 0 ]; then
dependency_path="/system/bin/$1";
device_tree_path="${WORKDIR}/recovery/root/sbin/$1";
fi;
else
for i in /system/lib64/vndk*/$1; do
if [ -f "$i" ] && [ ! -f "${WORKDIR}/recovery/root/sbin/$1" ]; then
cp -f "$i" "${WORKDIR}/recovery/root/sbin/$1";
if [ $? -eq 0 ]; then
dependency_path="$i";
device_tree_path="${WORKDIR}/recovery/root/sbin/$1";
fi;
break;
fi
done
fi
if [ "$device_tree_path" ]; then
relink "$device_tree_path";
if [ $? -eq 0 ]; then
echo "        <file path=\"$dependency_path\" name=\"${dependency_path##*/}\" size_in_bytes=\"`file_size $dependency_path`\" md5sum=\"`file_md5 $dependency_path`\" require_relink=\"true\" />" >> "$PROPRIETARY_FILES_XML";
else
echo "        <file path=\"$dependency_path\" name=\"${dependency_path##*/}\" size_in_bytes=\"`file_size $dependency_path`\" md5sum=\"`file_md5 $dependency_path`\" require_relink=\"false\" />" >> "$PROPRIETARY_FILES_XML";
fi
fi;
echo $dependency_path;
}

write_device_make_file() {
LOGI "Writing ${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
write_author_copyright "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "`echo_command call inherit-product, build/target/product/embedded.mk`" >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "" >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "# Inherit from custom product configuration"  >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "`echo_command call inherit-product, vendor/omni/config/common.mk`"  >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "" >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "## Device identifier. This must come after all inclusions" >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "BOARD_VENDOR := `get_vendor_prop`" >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "PRODUCT_DEVICE := ${DEVICE_CODE_NAME}" >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "PRODUCT_NAME := ${CT_DT_VENDOR}_${DEVICE_CODE_NAME}" >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "PRODUCT_BRAND := `get_vendor_prop`" >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "PRODUCT_MODEL := `get_global ro.product.model`" >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "PRODUCT_MANUFACTURER := `get_vendor_prop`" >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "TARGET_VENDOR := `get_vendor_prop`" >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "PRODUCT_RELEASE_NAME := ${DEVICE_CODE_NAME}" >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "PRODUCT_GMS_CLIENTID_BASE := `get_global ro.com.google.clientidbase`" >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
echo "PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.hardware.keystore=`get_global ro.board.platform` ro.hardware.gatekeeper=`get_global ro.board.platform`" >> "$WORKDIR/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk";
}

write_make_file() {
write_author_copyright "$WORKDIR/Android.mk";
echo "LOCAL_PATH := `echo_command call my-dir`" >> "$WORKDIR/Android.mk";
local ret_tmp="($";
ret_tmp="$ret_tmp(";
ret_tmp="${ret_tmp}filter ${DEVICE_CODE_NAME},";
ret_tmp="$ret_tmp$";
ret_tmp="$ret_tmp(TARGET_DEVICE)),)";
echo "ifneq ${ret_tmp}" >> "$WORKDIR/Android.mk";
ret_tmp="$";
ret_tmp="$ret_tmp(";
ret_tmp="${ret_tmp}call all-makefiles-under,";
ret_tmp="$ret_tmp$";
ret_tmp="$ret_tmp(LOCAL_PATH))";
echo "include $ret_tmp" >> "$WORKDIR/Android.mk";
echo "include `echo_command CLEAR_VARS`" >> "$WORKDIR/Android.mk";
echo "endif" >> "$WORKDIR/Android.mk";
}

generate_boardconfig() {
  local val
  local ABI="`get_global ro.product.cpu.abi`"
  [ "$ABI" ] && ABI=${ABI:0:3}
  local ABI2="`get_global ro.product.cpu.abi2`"
  [ "$ABI2" ] && ABI=${ABI2:0:3}
  local ABILONG="`get_global ro.product.cpu.abi`"
  local ARCH=arm
  local ARCH32=arm
  local IS64BIT=false
  
  if [ "$ABI" = "x86" ]; then ARCH=x86; ARCH32=x86; fi;
  if [ "$ABI2" = "x86" ]; then ARCH=x86; ARCH32=x86; fi;
  if [ "$ABILONG" = "arm64-v8a" ]; then ARCH=arm64; ARCH32=arm; IS64BIT=true; fi;
  if [ "$ABILONG" = "x86_64" ]; then ARCH=x64; ARCH32=x86; IS64BIT=true; fi;

write_author_copyright "$WORKDIR/BoardConfig.mk";
echo "DEVICE_TREE_PATH := device/`get_vendor_prop`/${DEVICE_CODE_NAME}" >> "$WORKDIR/BoardConfig.mk";
echo "" >> "$WORKDIR/BoardConfig.mk";
echo "#Architecture" >> "$WORKDIR/BoardConfig.mk";
echo "TARGET_ARCH := $ARCH" >> "$WORKDIR/BoardConfig.mk";
if [ "$ARCH" = "arm64" ]; then
echo "TARGET_ARCH_VARIANT := armv8-a" >> "$WORKDIR/BoardConfig.mk";
fi

local long_abi_list="`get_global ro.product.cpu.abilist`";
if [ "$long_abi_list" ]; then
long_abi_list="${long_abi_list%%,*}"
echo "TARGET_CPU_ABI := $long_abi_list" >> "$WORKDIR/BoardConfig.mk";
else
echo "TARGET_CPU_ABI := $ABILONG" >> "$WORKDIR/BoardConfig.mk";
fi
long_abi_list="`get_global ro.product.cpu.abi2`";
echo "TARGET_CPU_ABI2 := $long_abi_list" >> "$WORKDIR/BoardConfig.mk";
echo "TARGET_CPU_VARIANT := generic" >> "$WORKDIR/BoardConfig.mk";
echo "" >> "$WORKDIR/BoardConfig.mk";
echo "TARGET_2ND_ARCH := $ARCH32" >> "$WORKDIR/BoardConfig.mk";
if [ "$ARCH32" = "arm" ]; then
echo "TARGET_2ND_ARCH_VARIANT := armv7-a-neon" >> "$WORKDIR/BoardConfig.mk";
fi
long_abi_list="`get_global ro.product.cpu.abilist32`";
if [ "$long_abi_list" ]; then
local long_abi_list_dump="${long_abi_list%%,*}"
echo "TARGET_2ND_CPU_ABI := $long_abi_list_dump" >> "$WORKDIR/BoardConfig.mk";
long_abi_list_dump="${long_abi_list##*,}"
echo "TARGET_2ND_CPU_ABI2 := $long_abi_list_dump" >> "$WORKDIR/BoardConfig.mk";
elif [ "$ARCH32" = "arm" ]; then
echo "TARGET_2ND_CPU_ABI := armeabi-v7a" >> "$WORKDIR/BoardConfig.mk";
echo "TARGET_2ND_CPU_ABI2 := armeabi" >> "$WORKDIR/BoardConfig.mk";
fi
if [ "$ARCH" = "arm64" ]; then
echo "TARGET_2ND_CPU_VARIANT := armv8-a" >> "$WORKDIR/BoardConfig.mk";
fi
val="`get_global ro.board.platform`";
if [ "$val" ]; then
echo "TARGET_BOARD_PLATFORM := $val" >> "$WORKDIR/BoardConfig.mk";
echo "TARGET_BOOTLOADER_BOARD_NAME := $val" >> "$WORKDIR/BoardConfig.mk";
else
val="`get_global ro.product.board`";
if [ "$val" ]; then
echo "TARGET_BOARD_PLATFORM := $val" >> "$WORKDIR/BoardConfig.mk";
echo "TARGET_BOOTLOADER_BOARD_NAME := $val" >> "$WORKDIR/BoardConfig.mk";
fi
fi

write_boardconfig_partition_size() {
local var_path=`getdir $(get_executable_path blockdev)`;
if partition_exists ${2}; then
local var_size="`${blockdev_path}/blockdev --getsize64 ${PARTITIONS_PATH}/${2}`";
if [ "$var_size" ]; then
echo "${1} := ${var_size}" >> "$WORKDIR/BoardConfig.mk";
fi
fi
}

echo "" >> "$WORKDIR/BoardConfig.mk";

local partition_sizes="
boot
cache
recovery
system
userdata
vendor
";

local size_iterator

for size_iterator in $partition_sizes; do
write_boardconfig_partition_size "BOARD_`typemanager upper ${size_iterator}`IMAGE_PARTITION_SIZE" "${size_iterator}"
done

val="`get_global ro.hardware`";
if [ -z "$val" ]; then
val="`get_global ro.hardware.platform`";
fi

if [ "$val" ] && [ "$val" = "qcom" ]; then
echo "BOARD_USES_QCOM_HARDWARE := true" >> "$WORKDIR/BoardConfig.mk";
fi

echo "" >> "$WORKDIR/BoardConfig.mk";

local filesystems_to_check="
ext4
f2fs
ntfs
vfat
exfat
squashfs
yaffs2
";

for supported_filesystems in ${filesystems_to_check}; do
if filesystem_supported "${supported_filesystems}"; then
echo "TARGET_USERIMAGES_USE_`typemanager upper ${supported_filesystems}` := true" >> "$WORKDIR/BoardConfig.mk";
fi
done


echo "" >> "$WORKDIR/BoardConfig.mk";

echo "TARGET_OTA_ASSERT_DEVICE := ${DEVICE_CODE_NAME}" >> "$WORKDIR/Android.mk";

}

process_dependency() {
if [ "$readelf_path" ]; then
local required_libs="`get_dependencies "$1"`";
if [ "$required_libs" ]; then
echo "" >> $PROPRIETARY_FILES_DEPENDENCIES_XML;
echo "           <library name=\"${1##*/}\">" >> $PROPRIETARY_FILES_DEPENDENCIES_XML;
echo "               <location>${1}</location>" >> $PROPRIETARY_FILES_DEPENDENCIES_XML;
echo "               <size>`file_size $1`</size>" >> $PROPRIETARY_FILES_DEPENDENCIES_XML;
echo "               <md5sum>`file_md5 $1`</md5sum>" >> $PROPRIETARY_FILES_DEPENDENCIES_XML;
echo "               <dependencies>" >> $PROPRIETARY_FILES_DEPENDENCIES_XML;
for lib in $required_libs; do
echo "                   <dependency name=\"$lib\"/>" >> $PROPRIETARY_FILES_DEPENDENCIES_XML;
done;
echo "               </dependencies>" >> $PROPRIETARY_FILES_DEPENDENCIES_XML;
echo "           </library>" >> $PROPRIETARY_FILES_DEPENDENCIES_XML;
for lib in $required_libs; do
local lib_path="`copy_dependency_file "$lib"`";
if [ "$lib_path" ]; then
process_dependency "$lib_path";
fi
done;
fi
fi;
}

create_readme_file() {
LOGI "Creating README.md file...";
echo "# android_device_$(getprop ro.product.brand)_${DEVICE_CODE_NAME}" >> "$WORKDIR/README.md";
echo "Device tree for bulding $RECOVERY_VARIANT for $(getprop ro.product.model)" >> "$WORKDIR/README.md";
echo "" >> "$WORKDIR/README.md";
echo "## Kernel Sources" >> "$WORKDIR/README.md";
echo "" >> "$WORKDIR/README.md";
echo "- Prebuilt kernel used" >> "$WORKDIR/README.md";
echo "" >> "$WORKDIR/README.md";
echo "## To compile" >> "$WORKDIR/README.md";
echo "" >> "$WORKDIR/README.md";
echo "export ALLOW_MISSING_DEPENDENCIES=true" >> "$WORKDIR/README.md";
echo "" >> "$WORKDIR/README.md";
echo ". build/envsetup.sh && lunch ${CT_DT_VENDOR}_${DEVICE_CODE_NAME}-eng" >> "$WORKDIR/README.md";
echo "" >> "$WORKDIR/README.md";
echo "make recoveryimage" >> "$WORKDIR/README.md";
}

process_proprietary_requirements() {
for proprietary_requirement in $1; do
local rootpath=${proprietary_requirement%:*}
local blob_size="`file_size /$rootpath`";
local blob_md5="`file_md5 /$rootpath`";
local devicepath=${proprietary_requirement#*:}
local device_dirname="`getdir $devicepath`";
if [ -f "/$rootpath" ]; then
if [ ! -d "$device_dirname" ]; then
mkdir -p "$WORKDIR/${device_dirname}";
fi;
echo "/$rootpath:${blob_size}:${blob_md5}" >> "$PROPRIETARY_FILES_INFO";
LOGI "Copying /$rootpath to $devicepath";
if [ ! -f "$WORKDIR/$devicepath" ]; then
cp -f "/$rootpath" "$WORKDIR/$devicepath";
fi;
echo "/$proprietary_requirement" >> $PROPRIETARY_FILES_FILE;
relink "$WORKDIR/$devicepath" "$devicepath";
if [ $? -eq 0 ]; then
echo "        <file path=\"/$rootpath\" name=\"${rootpath##*/}\" size_in_bytes=\"${blob_size}\" md5sum=\"${blob_md5}\" require_relink=\"true\" />" >> "$PROPRIETARY_FILES_XML";
else
echo "        <file path=\"/$rootpath\" name=\"${rootpath##*/}\" size_in_bytes=\"${blob_size}\" md5sum=\"${blob_md5}\" require_relink=\"false\" />" >> "$PROPRIETARY_FILES_XML";
fi
process_dependency "/$rootpath";
fi;
done;
}

copy_unspecified_proprietary_blob() {
for proprietary_blob in $@; do
if [ ! -f "$proprietary_blob" ]; then
continue;
fi
local file_basename="${proprietary_blob##*/}";
local device_dirname="`getdir $proprietary_blob`";
local blob_size="`file_size $proprietary_blob`";
local blob_md5="`file_md5 $proprietary_blob`";
if [ ! -f "${WORKDIR}/recovery/root${device_dirname}/${file_basename}" ]; then
if [ ! -d "$WORKDIR/recovery/root${device_dirname}" ]; then
mkdir -p "$WORKDIR/recovery/root${device_dirname}";
fi;
LOGI "Copying $proprietary_blob to recovery/root${device_dirname}/${file_basename}";
echo "${proprietary_blob}:${blob_size}:${blob_md5}" >> "$PROPRIETARY_FILES_INFO";
if [ ! -f "${WORKDIR}/recovery/root${device_dirname}/${file_basename}" ]; then
cp -f "$proprietary_blob" "${WORKDIR}/recovery/root${device_dirname}/${file_basename}";
fi
echo "${proprietary_blob}:recovery/root${device_dirname}/${file_basename}" >> $PROPRIETARY_FILES_FILE;
relink "${WORKDIR}/recovery/root${device_dirname}/${file_basename}" "recovery/root${device_dirname}/${file_basename}";
if [ $? -eq 0 ]; then
echo "        <file path=\"$proprietary_blob\" name=\"$file_basename\" size_in_bytes=\"${blob_size}\" md5sum=\"${blob_md5}\" require_relink=\"true\" />" >> "$PROPRIETARY_FILES_XML";
else
echo "        <file path=\"$proprietary_blob\" name=\"$file_basename\" size_in_bytes=\"${blob_size}\" md5sum=\"${blob_md5}\" require_relink=\"false\" />" >> "$PROPRIETARY_FILES_XML";
fi
process_dependency "$proprietary_blob";
fi;
done;
}


copy_unspecified_proprietary_files() {
copy_unspecified_proprietary_blob \
/firmware/image/cmnlib* \
/firmware/image/keymaste* \
/vendor/firmware/cmnlib* \
/vendor/firmware/keymaste* \
/vendor/firmware/keymaster/cmnlib* \
/vendor/firmware/keymaster/keymaste* \
/vendor/lib64/hw/gatekeeper.*.so \
/vendor/lib64/hw/keystore.*.so \
/vendor/lib64/hw/android.hardware.keymaster*.so \
/vendor/lib64/hw/android.hardware.gatekeeper*.so \
/system/etc/vintf/compatibility_matri*.xml \
/system/etc/vintf/manifes*.xml \
/system/compatibility_matri*.xml \
/vendor/lib/modules/exfat.ko \
/vendor/compatibility_matri*.xml \
/vendor/manifes*.xml \
/system/manifes*.xml \
/vendor/firmware/*DSP.bin \
/vendor/etc/vintf/compatibility_matri*.xml \
/vendor/etc/vintf/manifes*.xml \
/vendor/lib64/vendor.qti.hardware.tui_comm*.so
}

copy_unspecified_proprietary_blob_sbin() {
if [ ! -d "$WORKDIR/recovery/root/sbin" ]; then
mkdir -p "$WORKDIR/recovery/root/sbin";
fi;
local hash_alg="`get_hash_alg`";
local hash_val="";
for proprietary_blob_sbin in $@; do
if [ ! -f "$proprietary_blob_sbin" ]; then
continue;
fi
local file_basename="${proprietary_blob_sbin##*/}";
local blob_size="`file_size $proprietary_blob_sbin`";
local blob_md5="`file_md5 $proprietary_blob_sbin`";
if [ ! -f "${WORKDIR}/recovery/root/sbin/${file_basename}" ]; then
LOGI "Copying $proprietary_blob_sbin to recovery/root/sbin/${file_basename}";
if [ "$hash_val" ]; then
echo "$proprietary_blob_sbin:${blob_size}:${hash_val}" >> "$PROPRIETARY_FILES_INFO";
else
echo "$proprietary_blob_sbin:${blob_size}" >> "$PROPRIETARY_FILES_INFO";
fi
if [ ! -f "${WORKDIR}/recovery/root/sbin/${file_basename}" ]; then
cp -f "$proprietary_blob_sbin" "${WORKDIR}/recovery/root/sbin/${file_basename}";
fi;
echo "${proprietary_blob_sbin}:recovery/root/sbin/${file_basename}" >> $PROPRIETARY_FILES_FILE;
relink "${WORKDIR}/recovery/root/sbin/${file_basename}" "recovery/root/sbin/${file_basename}";
if [ $? -eq 0 ]; then
echo "        <file path=\"$proprietary_blob_sbin\" name=\"${proprietary_blob_sbin##*/}\" size_in_bytes=\"${blob_size}\" md5sum=\"${blob_md5}\" require_relink=\"true\" />" >> "$PROPRIETARY_FILES_XML";
else
echo "        <file path=\"$proprietary_blob_sbin\" name=\"${proprietary_blob_sbin##*/}\" size_in_bytes=\"${blob_size}\" md5sum=\"${blob_md5}\" require_relink=\"false\" />" >> "$PROPRIETARY_FILES_XML";
fi
process_dependency "$proprietary_blob_sbin";
fi;
done;
}

copy_unspecified_proprietary_files_sbin() {
copy_unspecified_proprietary_blob_sbin \
/system/lib64/android.hardware.gatekeeper*.so \
/system/lib64/android.hardware.keymaster*.so \
/system/lib64/android.hidl.base*.so \
/system/lib64/android.hidl.token*.so \
/system/lib64/android.system.wifi.keystore*.so \
/vendor/bin/hw/android.hardware.gatekeeper* \
/vendor/bin/hw/android.hardware.keymaster* \
/system/lib64/android.hardware.confirmationui*.so \
/system/lib64/vndk-*/android.hardware.gatekeeper*.so \
/system/lib64/vndk-*/android.hardware.keymaster*.so \
/system/lib64/vndk-*/android.hidl.base*.so \
/system/lib64/vndk-*/android.hidl.token*.so \
/system/lib64/vndk-*/android.system.wifi.keystore*.so \
/system/lib64/vndk-*/android.hardware.confirmationui*.so \
/system/lib64/vndk-*/android.hardware.weaver*.so \
/system/lib64/vndk-*/libpuresoftkeymasterdevice.so \
/system/lib64/vndk-*/libion.so
}

copy_unspecified_proprietary_blob_root() {
if [ ! -d "$WORKDIR/recovery/root" ]; then
mkdir -p "$WORKDIR/recovery/root";
fi;
local hash_alg="`get_hash_alg`";
local hash_val="";
local i
for i in $@; do
if [ ! -f "$i" ]; then
continue;
fi
local file_basename="${i##*/}";
local blob_size="`file_size $i`";
if [ "$hash_alg" ]; then
local hash_val="`get_hash_value $i`";
fi
if [ ! -f "${WORKDIR}/recovery/root/${file_basename}" ]; then
LOGI "Copying $i to recovery/root/${file_basename}";
if [ "$hash_val" ]; then
echo "$i:${blob_size}:${hash_val}" >> "$PROPRIETARY_FILES_INFO";
else
echo "$i:${blob_size}" >> "$PROPRIETARY_FILES_INFO";
fi
execute cp -f "$i" "${WORKDIR}/recovery/root/${file_basename}";
echo "${i}:recovery/root/${file_basename}" >> $PROPRIETARY_FILES_FILE;
if [ "$hash_val" ]; then
echo "        <file path=\"$i\" name=\"${i##*/}\" size_in_bytes=\"${blob_size}\" ${hash_alg}=\"${hash_val}\" require_relink=\"false\" />" >> "$PROPRIETARY_FILES_XML";
else
echo "        <file path=\"$i\" name=\"${i##*/}\" size_in_bytes=\"${blob_size}\" require_relink=\"false\" />" >> "$PROPRIETARY_FILES_XML";
fi
process_dependency "$i";
fi;
done;
}

copy_unspecified_proprietary_files_root() {
copy_unspecified_proprietary_blob_root \
/vendor/etc/selinux/*contexts \
/system/etc/selinux/*contexts \
/uevent*.rc
}


copy_proprietary_files() {
local proprietary_files="
system/usr/share/zoneinfo/tzdata:recovery/root/system/usr/share/zoneinfo/tzdata
vendor/bin/qseecomd:recovery/root/sbin/qseecomd
vendor/lib64/libtime_genoff.so:recovery/root/vendor/lib64/libtime_genoff.so
vendor/lib64/libStDrvInt.so:recovery/root/vendor/lib64/libStDrvInt.so
vendor/lib64/libsecureui.so:recovery/root/vendor/lib64/libsecureui.so
vendor/lib64/libGPreqcancel_svc.so:recovery/root/vendor/lib64/libGPreqcancel_svc.so
vendor/lib64/libkeymasterprovision.so:recovery/root/vendor/lib64/libkeymasterprovision.so
vendor/lib64/libGPreqcancel.so:recovery/root/vendor/lib64/libGPreqcancel.so
vendor/lib64/libkeymasterdeviceutils.so:recovery/root/vendor/lib64/libkeymasterdeviceutils.so
vendor/lib64/libQSEEComAPI.so:recovery/root/vendor/lib64/libQSEEComAPI.so
vendor/lib64/libsecureui_svcsock.so:recovery/root/vendor/lib64/libsecureui_svcsock.so
vendor/lib64/libdrmtime.so:recovery/root/vendor/lib64/libdrmtime.so
vendor/lib64/librpmb.so:recovery/root/vendor/lib64/librpmb.so
vendor/lib64/libdrmfs.so:recovery/root/vendor/lib64/libdrmfs.so
vendor/lib64/libssd.so:recovery/root/vendor/lib64/libssd.so
vendor/lib64/libSecureUILib.so:recovery/root/vendor/lib64/libSecureUILib.so
vendor/lib64/libqisl.so:recovery/root/vendor/lib64/libqisl.so
vendor/lib64/libdiag.so:recovery/root/vendor/lib64/libdiag.so
vendor/lib64/libkeymasterutils.so:recovery/root/vendor/lib64/libkeymasterutils.so
system/lib64/libpuresoftkeymasterdevice.so:recovery/root/sbin/libpuresoftkeymasterdevice.so
vendor/lib64/libspl.so:recovery/root/vendor/lib64/libspl.so
"
process_proprietary_requirements "$proprietary_files";
}


get_display_value() {
check_args get_display_value 1 $@;
local key="";
local value="";
local position=false;
for i in "${PARTITION_DISPLAY[@]}" ; do
  if ! $position; then
  key=$i;
  position=true;
  continue;
  else
  value=$i
  position=false;
  fi
  if [ "$key" = "$1" ]; then
  echo $value;
  return 0;
  fi
done
if [[ "$1" == *"bak" ]]; then
echo "${1%bak} backup";
return 0;
elif [[ "$1" == *"_image" ]]; then
local imagestring;
imagestring=${1%_image}
imagestring="$(tr '[:lower:]' '[:upper:]' <<< ${imagestring:0:1})${imagestring:1}"
echo "$imagestring Image"
return 0;
fi;
echo "$1";
}

get_device_encryption_mode() {
local encryption_property="`get_global ro.crypto.state`";
local ret="unknown";
local FDE="";
local FBE="";
[[ "`grep_line /proc/mounts ' /data '`" == *"dm-"* ]] && FDE=true || FDE=false
[ -d /data/unencrypted ] && FBE=true || FBE=false
if ! $FBE && ! $FDE; then
if [ "$encryption_property" = "block" ]; then
ret="FDE";
elif [ "$encryption_property" = "file" ]; then
ret="FBE";
fi;
elif $FBE; then
ret="FBE";
elif $FDE; then
ret="FDE";
fi
if [ $ret = "FDE" ]; then
if ! partition_exists metadata; then 
find /sys/devices -name mmcblk1 &>/dev/null;
if [ $? -eq 0 ]; then
local boot_mode=`get_global ro.boot.bootdevice`;
if [ "$boot_mode" ]; then
if [[ "$boot_mode" = *"ufs"* ]]; then
ret="FDE ICE UFS";
elif [[ "$boot_mode" = *"sdhc"* ]]; then
if [ -c "/dev/icesdcc" ]; then
ret="FDE ICE SDCC";
else
ret="FDE SDCC";
fi
fi
fi
fi
fi
fi
echo $ret;
}

get_twrp_partition_flags() {
check_args get_twrp_partition_flags 1 $@;
local key="";
local value="";
local position=false;
for i in "${TWRP_PARTITION_FLAGS[@]}" ; do
  if ! $position; then
  key=$i;
  position=true;
  continue;
  else
  value=$i
  position=false;
  fi
  if [ "$key" = "$1" ]; then
  echo $value;
  return 0;
  fi
done
}


get_partition_info() {
check_args get_partition_info 1 $@;
local key="";
local value="";
local position=false;
for i in "${PARTITION_INFO[@]}" ; do
  if ! $position; then
  key=$i;
  position=true;
  continue;
  else
  value=$i
  position=false;
  fi
  if [ "$key" = "$1" ]; then 
  echo $value;
  return 0;
  fi
done
if  [[ "$1" == *"bak" ]]; then
echo "Backup partition of ${1%bak}";
elif [[ "$1" == *"_image" ]]; then
echo "EMMC image of ${1%_image} partition";
else
echo "unknown";
fi
}

comment() {
check_args comment 1 $@;
echo "#$1" >> $RECOVERY_FSTAB_FILE;
}

write_value() {
check_args write_value 1 $@;
echo "$1" >> $RECOVERY_FSTAB_FILE;
}

write_empty() {
echo "" >> $RECOVERY_FSTAB_FILE;
}

initialize_values() {
PARTITION_INFO=(
"system" "Contains system applications and libraries that have AOSP source code. During normal operation, this partition is mounted read-only; its contents change only during an OTA update or when flashing a new OS."
"system_image" "EMMC image of system partition"
"boot" "Kernel and initramfs (modern form of of ramdisk and ramfs/tmpfs)"
"cache" "cached (frequently accessed) data from OS usage and contains the firmware update package downloaded from server during OTA updates"
"cache_image" "EMMC image of cache partition"
"data" "Android user data partition"
"data_image" "EMMC image of data partition"
"recovery" "Holds alternate boot partition and the recovery program that lets the device boot into a recovery console for performing advanced recovery and maintenance operations."
"vendor" "vendor components, contains system applications and libraries that do not have source code available on AOSP but added by vendors"
"vendor_image" "EMMC image of vendor partition"
"cust" "used by stock ROM's, holding some preloaded system apps and regional settings which are installed on first use."
"preload" "used by stock ROM's, holding some preloaded system apps and regional settings which are installed on first use."
"custom" "used by stock ROM's, holding some preloaded system apps and regional settings which are installed on first use."
"cust_image" "EMMC image of cust partition"
"misc" "boot mode selector used to pass data among various stages of the boot chain (boot into recovery mode, fastboot etc.). e.g. if it is empty (all zero), system boots normally. If it contains recovery mode selector, system boots into recovery mode."
"persist" "contains data which shouldn't be changed after the device is shipped, e.g. DRM related files, sensor reg file (sns.reg) and calibration data of chips; wifi, bluetooth, camera etc."
"persist_image" "EMMC image of persist partition"
"persistbak_image" "EMMC image of persist backup partition"
"splash" "splash image or boot logo which appears when device boots (at ABOOT stage)."
"modem" "radio partition, responsible for signals and on older devices may control wifi, bluetooth, and GPS"
"bluetooth" "bluetooth firmware"
"dsp" "(Digital Signal Processor) - by Qualcomm to assist in things like smooth video playback (realtime media and sensors processor)"
"modemst1" "modem storage 1"
"modemst2" "modem storage 2"
"fsc" "Modem File System Cookie partition"
"fsg" "Modem filesystem golden backup"
"tz" "TZ (TrustZone) - used by ARM processors as an additional lock to security features"
"oem" "like VENDOR, it incorporates OEM (Original Equipment Manufacturer i.e. hardware manufacturer or Mobile Phone brand) small customization (modifications) to original Android (AOSP) during OTA updates such as customized system properties values etc."
"pad" "related to OEM"
"ota" "OTA updates"
"fota" "OTA updates"
"ddr" "Double Data Rate RAM"
"fsc" "Modem FileSystem Cookies"
"ssd" "Secure Software Download, a memory based file system for secure storage, stores some encrypted RSA keys"
"devinfo" "device information including: is_unlocked (aboot), is_tampered, is_verified, charger_screen_enabled, display_panel, bootloader_version, radio_version etc. Contents of this partition are displayed by \"fastboot oem device-info\" command in human readable format. Before loading boot.img or recovery.img, bootloader verifies the locked state from this partition."
"devcfg" "used by TZ for upgrades"
"limits" "Qualcomm Limits Management Hardware (LMh) driver in SBL writes the data in this partition to use for later reboots"
"syscfg" "Qualcomm CPR (Core Power Reduction) Regulator for better performance and power saving of application processor by voltage control"
"dip" "boot verification, use Qualcomm SafeSwitch technology to lock and track theft phones"
"mdtp" "boot verification, use Qualcomm SafeSwitch technology to lock and track theft phones"
"keymaster" "verified boot"
"lksecapp" "LK (Little Kernel) Security App, related to RPM, TZ online verification / update"
"config" "saves state of Factory Reset Protection (FRP), Allow bootloader (OEM) unlocking (Developer Options), asks already associated account info"
"frp" "saves state of Factory Reset Protection (FRP), Allow bootloader (OEM) unlocking (Developer Options), asks already associated account info"
"pdb" "saves state of Factory Reset Protection (FRP), Allow bootloader (OEM) unlocking (Developer Options), asks already associated account info"
"keystore" "related to /data Full Disc Encryption (FDE)"
"mcfg" "(Modem Configuration Framework) - on dual SIM devices, loads MBN (modem binary) files depending on SIM/carrier"
"chglogo" "charging screen that appears when charger is connected to powered off device."
"msadp" "related to debug policies"
"apdp" "related to debug policies"
"dpo" "related to debug policies"
"grow" "empty for future expansion"
"sbl" "Secondary bootloader loaded by SoC, loads ABOOT in memory, also provides (Emergency) Download Mode (EDL) on many devices, a Firmware Update Protocol."
"aboot" "(bootloader.img or aboot.mbn file in Factory Firmware) - Applications Bootloader is the main bootloader responsible for loading kernel or recovey and fastboot - a Firmware Update Protocol - as well."
"abl" "(bootloader.img or aboot.mbn file in Factory Firmware) - Applications Bootloader is the main bootloader responsible for loading kernel or recovey and fastboot - a Firmware Update Protocol - as well."
"kernelflinger" "similar bootloader as aboot but on Intel devices."
"hyp" "Virtual Machine Monitor, to enable Virtual Machine platform"
"pbl" "controls power to radio, modem etc."
"rpm" "(Resource/Power Management) or Primary/Primitive"
"param" "stores a number of parameters, variables and settings of the hardware. It contains info whether MODEMST partitions are backed up or not. Also debug settings, custom ROMs flash count, current stage boot process etc."
"xbl" "Qualcomm's eXtensible Bootloader"
"vbmeta" "The top-level object for verified boot."
"sdcard1" "External SD card"
"usbotg" "Universal Serial Bus On-The-Go (USB OTG)"
"radio" "The radio partition contains the radio image. This partition is only necessary for devices that include a radio that have radio-specific software in a dedicated partition."
"metadata" "The metadata partition is used when device is encrypted and is 16MB or larger."
"cmnlib" "verified boot"
"cmnlib64" "verified boot"
)
 

PARTITION_DISPLAY=(
"abl" "aboot BL"
"apdp" "apdp"
"bk1" "bk1"
"bk2" "bk2"
"bluetooth" "Bluetooth FW"
"bt_firmware_image" "Bluetooth FW Image"
"ddr" "DDR"
"dsp" "Dsp FW"
"devinfo" "Device Info"
"dpo" "Debug Policy"
"fsc" "FS Cookie"
"fsg" "FS Golden"
"frp" "FRP"
"hyp" "Hypervisor"
"modem" "Modem FW"
"modemst1" "Modem Storage 1"
"modemst2" "Modem Storage 2"
"recovery" "Recovery"
"rpm" "RPM"
"rpmbak" "RPM Backup"
"splash" "Boot Logo"
"ssd" "Secure Software Download"
"sbl" "Secondary bootloader"
"sdcard1" "ExternalSD"
"switch" "Switch"
"system" "System"
"toolsfv" "ToolsFV"
"tz" "TrustZone"
"userdata" "Userdata"
"usbotg" "USB Storage"
"vbmeta" "VB Metadata"
"vendor_image" "Vendor Image"
"vendor" "Vendor"
"xbl" "eXtensible BL"
)

TWRP_PARTITION_FLAGS=(
"bluetooth" ";mounttodecrypt;backup=1"
"boot" ";backup=1;flashimg=1"
"cust" ";backup=1;wipeingui"
"dsp" ";backup=1"
"modem" ";mounttodecrypt;backup=1"
"persist" ";backup=1"
"persistbak" ";backup=0"
"pmic" ";backup=1;flashimg=1"
"recovery" ";backup=1;flashimg=1"
"rpm" ";backup=1;flashimg=1"
"sec" ";backup=1;flashimg=1"
"splash" ";backup=1;flashimg=1"
"storsec" ";backup=1;flashimg=1"
"system" ";backup=1"
"tz" ";backup=1;flashimg=1"
"userdata" ";backup=1"
"vendor" ";backup=1;wipeingui;mounttodecrypt"
"xbl" ";backup=1;flashimg=1"
)

PARTITION_FLAGS_TO_SYMLINK="
forceencrypt=
encryptable=
fileencryption=
forcefdeorfbe=
voldmanaged=
length=
";

PARTITIONS_TO_CREATE_IMAGE_SYMLINK="
cache
data
firmware
modem
bt_firmware
persist
system
vendor
dsp
cust
";

START_OF_EXECUTION=`date +%s`
echo "Initializing...";
DEVICE_CODE_NAME="`get_global ro.build.product`";
# PTERODON | TWRP
RECOVERY_VARIANT="TWRP";
if [ -d "/sdcard" ]; then
if [ ! -d "/sdcard/Git" ]; then
mkdir "/sdcard/Git";
fi
WORKDIR="/sdcard/Git/android_device_$(getprop ro.product.brand)_${DEVICE_CODE_NAME}";
else
WORKDIR="`getdir "${CT_SCRIPT_LOCAL_PATH}"`/android_device_$(getprop ro.product.brand)_${DEVICE_CODE_NAME}";
fi

PROPRIETARY_FILES_FILE="$WORKDIR/proprietary-files.txt";
PROPRIETARY_FILES_RELINK="$WORKDIR/proprietary-files-relink.txt";
PROPRIETARY_FILES_INFO="$WORKDIR/proprietary-files-info.txt";
PARTITIONS_XML="$WORKDIR/partitions.xml";
PROPRIETARY_FILES_XML="$WORKDIR/proprietary-files.xml";
PROPRIETARY_FILES_DEPENDENCIES_XML="$WORKDIR/proprietary-files-dependencies.xml";
}

check_root() {
if ! running_as_root; then
print "Script isn't running as root!"
print "Please call \"su\" first!"
exit 0;
fi
}

log_device_info() {
LOGI "Kernel version: `get_kernel_version`";
LOGI "CPU core number: `get_cpu_core_number`";
LOGI "Hardware info: `get_hardware_info`";
LOGI "Memory info: `get_memory_info`";
LOGI "Kernel version: `get_kernel_version`";
LOGI "Running OS: `get_running_os`";
LOGI "Device boot time: `get_device_boot_time`";
}




initialize_values;
check_root;

readelf_path="`get_executable_path readelf`"; 
if [ -z "$readelf_path" ]; then
readelf_path="`getdir "${CT_SCRIPT_LOCAL_PATH}"`/readelf"
if [ ! -f "$readelf_path" ]; then
readelf_path="";
else
make_executable "$readelf_path";
fi
fi;

if [ -d "$WORKDIR" ]; then
for i in $(sequence 1 1024); do 
if [ ! -d "${WORKDIR}_$i" ]; then
WORKDIR="${WORKDIR}_$i";
mkdir -p "$WORKDIR";
break;
fi;
done;
else
mkdir -p "$WORKDIR";
fi

LOGI "Starting device tree builder $CT_SCRIPT_GENERATOR_VERSION";
LOGI "©ATGDroid <bythedroid@gmail.com>"
LOGI "";

DIRS_TO_CREATE="
recovery/root
init
prebuilt
recovery/root/etc
recovery/root/sbin
recovery/root/vendor/lib64/hw
recovery/root/system
";
print "Creating directories...";
for i in $DIRS_TO_CREATE; do
LOGI "Creating directory: $i";
mkdir -p "$WORKDIR/$i";
done;

log_device_info;
print "Creating make files...";
write_apache_license "$WORKDIR/LICENSE";
LOGI "Writing license file...";
PROPRIETARY_FILES_FILE="$WORKDIR/proprietary-files.txt";
write_author_copyright "$PROPRIETARY_FILES_FILE";

echo -e "FILE_PATH:DEVICE_TREE_PATH\n" >> "$PROPRIETARY_FILES_FILE";

PROPRIETARY_FILES_RELINK="$WORKDIR/proprietary-files-relink.txt";
write_author_copyright "$PROPRIETARY_FILES_RELINK";

echo -e "DEVICE_TREE_PATH\n" >> "$PROPRIETARY_FILES_RELINK";

PROPRIETARY_FILES_INFO="$WORKDIR/proprietary-files-info.txt";
write_author_copyright "$PROPRIETARY_FILES_INFO";

PARTITIONS_XML="$WORKDIR/partitions.xml";

echo "<?xml version=\"1.0\"?>" >> "$PARTITIONS_XML";
write_author_copyright_xml "$PARTITIONS_XML";
echo "<!-- Generated based on `derive_build_fingerprint` -->" >> "$PARTITIONS_XML";
echo "<partitions>" >> "$PARTITIONS_XML";
echo "    <physical_partition>" >> "$PARTITIONS_XML";

PROPRIETARY_FILES_XML="$WORKDIR/proprietary-files.xml";
echo "<?xml version=\"1.0\"?>" >> "$PROPRIETARY_FILES_XML";
write_author_copyright_xml "$PROPRIETARY_FILES_XML";
echo "<proprietary>" >> "$PROPRIETARY_FILES_XML";
echo "    <proprietary_files>" >> "$PROPRIETARY_FILES_XML";

if [ "$readelf_path" ]; then
PROPRIETARY_FILES_DEPENDENCIES_XML="$WORKDIR/proprietary-files-dependencies.xml";
echo "<?xml version=\"1.0\"?>" >> "$PROPRIETARY_FILES_DEPENDENCIES_XML";
write_author_copyright_xml "$PROPRIETARY_FILES_DEPENDENCIES_XML";
echo "<!-- Generated based on `derive_build_fingerprint` -->" >> "$PROPRIETARY_FILES_DEPENDENCIES_XML";
echo "<proprietary>" >> "$PROPRIETARY_FILES_DEPENDENCIES_XML";
echo "    <library_dependencies>" >> "$PROPRIETARY_FILES_DEPENDENCIES_XML";
fi;

echo -e "FILE_PATH:SIZE_OF_FILE_IN_BYTES:MD5SUM_OF_FILE\n" >> "$PROPRIETARY_FILES_INFO";

echo "# List of proprietary files which are needed to be relinked from \"/system/bin/linker64\" to \"/sbin/linker64\"" >> "$PROPRIETARY_FILES_RELINK";

LOGI "Writing vendorsetup.sh...";
echo "add_lunch_combo ${CT_DT_VENDOR}_${DEVICE_CODE_NAME}-eng" >> "$WORKDIR/vendorsetup.sh";
echo "add_lunch_combo ${CT_DT_VENDOR}_${DEVICE_CODE_NAME}-userdebug" >> "$WORKDIR/vendorsetup.sh";
echo "export ALLOW_MISSING_DEPENDENCIES=true" >> "$WORKDIR/vendorsetup.sh";

write_device_make_file;
write_make_file;
generate_boardconfig;

write_author_copyright "$WORKDIR/AndroidProducts.mk";
echo "PRODUCT_MAKEFILES := `echo_command LOCAL_DIR`/${CT_DT_VENDOR}_${DEVICE_CODE_NAME}.mk" >> "$WORKDIR/AndroidProducts.mk";

echo -e "/*
 * Copyright (C) `date +%Y` ATGDroid <bythedroid@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the \"License\");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an \"AS IS\" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
 
/*
* This code is used to update specific properties based on their
* values from /system and /vendor. 
*
* Created by ATGDroid @xda for `get_global ro.product.model`
*/

#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <unistd.h>
#include <android-base/logging.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

static const std::string PROPERTY_LOADER = \"property_loader: \";


namespace android {
namespace init {

struct one_str_struct {
    const std::string prop;
};

/* set which properties should be updated from /system */
const struct one_str_struct required_system_properties[] = {
	{ \"ro.build.fingerprint\" },
	{ \"ro.build.description\" },
	{ \"ro.build.version.incremental\" },
	{ \"ro.build.version.release\" },
	{ \"ro.treble.enabled\" },
	{ \"ro.expect.recovery_id\" },
	{ \"ro.product.mod_device\" },
	{ \"ro.build.version.security_patch\" },
	{ \"ro.build.version.sdk\" },
	{ \"\" },
};

/* set which properties should be updated from /vendor */
const struct one_str_struct required_vendor_properties[] = {
	{ \"ro.vendor.build.fingerprint\" },
	{ \"ro.vendor.build.security_patch\" },
	{ \"ro.vendor.build.date\" },
	{ \"ro.vendor.build.date.utc\" },
	{ \"\" },
};

void property_update(char const prop[], char const value[]) {
    prop_info *pi;
    
    pi = (prop_info*) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void property_symlink(char const prop1[], char const prop2[]) {
    char value[PROP_VALUE_MAX];
    if (__system_property_get(prop1, value) <= 0)
    return;
    LOG(INFO) << PROPERTY_LOADER << \"Symlinking property value \" << prop1 << \" to \" << prop2;
    property_update(prop2, value);
}

void load_property_file_by_mount_point(const std::string mount_point, const struct one_str_struct *required_mount_point_properties) {
LOG(INFO) << PROPERTY_LOADER << \"Loading properties from \" << mount_point;
const std::string mnt = \"/s\", property_file = mnt + \"/build.prop\";
mkdir(mnt.c_str(), 755);
mount((\"/dev/block/bootdevice/by-name\" + mount_point).c_str(), mnt.c_str(), \"ext4\", MS_RDONLY, NULL);
  struct stat st;
  if(stat(property_file.c_str(), &st) == 0) {
  LOG(INFO) << PROPERTY_LOADER << \"Found property file: \" << mount_point << \"/build.prop\";
  std::ifstream file(property_file.c_str());
  if (file.is_open()) {
  std::string local, value;
  size_t pos;
  while (getline(file, local)) {
  const struct one_str_struct *required_properties = required_mount_point_properties;
  while (!required_properties->prop.empty()) {
      if (required_properties->prop[0] == local[0] && 
         ((pos = local.find_first_of(\"=\")) != std::string::npos) &&
          local.substr(0, pos) == required_properties->prop) {
          value = local.substr(pos + 1);
          LOG(INFO) << PROPERTY_LOADER << \"Overriding property: \" << required_properties->prop << \"=\" << value;
          property_update(required_properties->prop.c_str(), value.c_str());
       }
       required_properties++;
     }
    }
   }
  }
    umount(mnt.c_str());
    rmdir(mnt.c_str());
}

void vendor_load_properties() {
load_property_file_by_mount_point(\"/system\", required_system_properties);
load_property_file_by_mount_point(\"/vendor\", required_vendor_properties);
property_symlink(\"ro.build.fingerprint\", \"ro.bootimage.build.fingerprint\");
}
}  // namespace init
}  // namespace android
" > "$WORKDIR/init/init_${DEVICE_CODE_NAME}.cpp";

write_author_copyright "$WORKDIR/init/Android.mk";

echo "LOCAL_PATH := `echo_command call my-dir`" >> "$WORKDIR/init/Android.mk";
echo "" >> "$WORKDIR/init/Android.mk";
echo "include `echo_command CLEAR_VARS`" >> "$WORKDIR/init/Android.mk";
echo "" >> "$WORKDIR/init/Android.mk";
echo "LOCAL_MODULE_TAGS := optional" >> "$WORKDIR/init/Android.mk";
echo 'LOCAL_C_INCLUDES := \' >> "$WORKDIR/init/Android.mk";
echo 'system/core/init \' >> "$WORKDIR/init/Android.mk";
echo "system/core/base/include" >> "$WORKDIR/init/Android.mk";
echo "" >> "$WORKDIR/init/Android.mk";
echo "LOCAL_STATIC_LIBRARIES := libbase" >> "$WORKDIR/init/Android.mk";
echo "LOCAL_SRC_FILES := init_${DEVICE_CODE_NAME}.cpp" >> "$WORKDIR/init/Android.mk";
echo "LOCAL_MODULE := libinit_${DEVICE_CODE_NAME}" >> "$WORKDIR/init/Android.mk";
echo "" >> "$WORKDIR/init/Android.mk";
echo "include `echo_command BUILD_STATIC_LIBRARY`" >> "$WORKDIR/init/Android.mk";

specific_line_found=false;
while read -r line; do
if ! $specific_line_found; then
if [[ "$line" = *"end build properties"* ]]; then
specific_line_found=true;
continue;
fi
else
echo "$line" >> "$WORKDIR/system.prop";
fi
done < /system/build.prop
while read -r line; do
echo "$line" >> "$WORKDIR/system.prop";
done < /vendor/build.prop

fstab_path="`get_fstab_path`";
if (( $? == 0 )) then
print "Copying stock ${fstab_path##*/}";
cp -f "$fstab_path" "$WORKDIR/recovery/root/${fstab_path##*/}";
fi

write_author_copyright "$WORKDIR/recovery/root/init.recovery.`get_global ro.hardware`.rc";


LOGI "Processing proprietary files...";

print "Processing hard coded proprietary files...";

copy_proprietary_files;
print "Searching for version based proprietary files...";
copy_unspecified_proprietary_files;
copy_unspecified_proprietary_files_sbin;
copy_unspecified_proprietary_files_root;

echo "    </proprietary_files>" >> "$PROPRIETARY_FILES_XML";
echo -n "</proprietary>" >> "$PROPRIETARY_FILES_XML";

if [ "$readelf_path" ]; then
echo -e "\n    </library_dependencies>" >> "$PROPRIETARY_FILES_DEPENDENCIES_XML";
echo -n "</proprietary>" >> "$PROPRIETARY_FILES_DEPENDENCIES_XML";
fi;

print "Searching for stock kernel...";
get_stock_kernel;
print "Getting properties info";
save_property_files_info "$WORKDIR/properties-info.xml";
print "Creating readme file...";
create_readme_file;

RECOVERY_FSTAB_FILE="$WORKDIR/recovery.fstab";
PARTITIONS_PATH="/dev/block/bootdevice/by-name";
REAL_PARTITIONS_PATH="/dev/block/platform/soc/`get_global ro.boot.bootdevice`/by-name";
partition_counter=0;

if [ -f $RECOVERY_FSTAB_FILE ]; then
rm -rf $RECOVERY_FSTAB_FILE;
fi;


partition_counter=0;
blkid_path=`getdir $(get_executable_path blkid)`;
blockdev_path=`getdir $(get_executable_path blockdev)`;
mountpoint_path=`getdir $(get_executable_path mountpoint)`;
if [ "$blkid_path" ]; then
have_blkid=true;
else
have_blkid=false;
fi
if [ "$blockdev_path" ]; then
have_blockdev=true;
else
have_blockdev=false;
fi

if [ "$mountpoint_path" ]; then
have_mountpoint=true;
else
have_mountpoint=false;
fi

if ! $have_blockdev && ! $have_blkid; then
echo -e "Device is missing 'blkid/blockdev' executables\nscript is now highly limited in it's functionality!\n";
elif $have_blockdev && ! $have_blkid; then
echo -e "Device is missing 'blkid' executable\nscript is now highly limited in it's functionality!\n";
elif ! $have_blockdev && $have_blkid; then
echo -e "Device is missing 'blockdev' executable\nscript is now highly limited in it's functionality!\n";
fi;

for i in $PARTITIONS_PATH/*; do
(( partition_counter++ ))
done;

echo "Processing partitions...";


write_author_copyright "${RECOVERY_FSTAB_FILE}.minimal";
write_author_copyright $RECOVERY_FSTAB_FILE;

write_empty;

comment "Android fstab file." ;
comment "Created by atgdroid@teamhorizon";
comment "`date`";
comment "For recovery: $RECOVERY_VARIANT";
comment "For device: `get_global ro.product.model`";
comment "Brand: `get_global ro.product.brand`";
comment "Total number of partitions: $partition_counter";
comment "Bootdevice: `get_global ro.boot.bootdevice`";
comment "Device encryption mode: `get_device_encryption_mode`";
write_empty;

total_number_of_partitions=$partition_counter;
partition_counter=0;

fstab_path=`get_fstab_path`;
for i in $PARTITIONS_PATH/*; do
(( partition_counter++ ))
print "Processing partition: $partition_counter of: $total_number_of_partitions";
basename_partition=${i##*/}
if $have_blkid; then
filesystem=`$blkid_path/blkid -o value -s TYPE $i`;
uuid=`$blkid_path/blkid -o value -s UUID $i`;
label=`$blkid_path/blkid -o value -s LABEL $i`;
else
filesystem="";
uuid="";
label="";
fi
if $have_blockdev; then
partition_size=`$blockdev_path/blockdev --getsize64 $i`;
block_size=`$blockdev_path/blockdev --getbsz $i`
fi;
mount_point="";
fstab_line="";
can_be_mounted="";
mmcblk="`readlink -f $i`";
fstab_line="`grep_line /proc/mounts "$i" "$mmcblk" " /$basename_partition "`";
if [ "$fstab_line" ]; then
can_be_mounted=true;
else
can_be_mounted=false;
fi

if $can_be_mounted; then
fstab_line="`grep_line /proc/mounts "$i"`";
if [ "$fstab_line" ]; then
if [[ -z "$filesystem" ]]; then
filesystem=$(get_str_by_pos "$fstab_line" 3);
fi;
mount_point=$(get_str_by_pos "$fstab_line" 2);
else
fstab_line="`grep_line /proc/mounts "$REAL_PARTITIONS_PATH/$basename_partition"`";
if [ "$fstab_line" ]; then
if [[ -z "$filesystem" ]]; then
filesystem=$(get_str_by_pos "$fstab_line" 5);
fi
mount_point=$(get_str_by_pos "$fstab_line" 2);
fi
if [[ -z "$filesystem" ]]; then
echo "Error: Unable to determine filesystem for $i!";
script_abort;
fi
fi
fi

if $have_mountpoint; then
local tmp_mountpoint=$basename_partition;
if [ "$mount_point" ]; then
tmp_mountpoint="/${mount_point##*/}";
fi;
if [ -d "$tmp_mountpoint" ]; then
$mountpoint_path/mountpoint $tmp_mountpoint &>/dev/null;
if [ $? -eq 0 ]; then
can_be_mounted=true;
else
can_be_mounted=false;
fi;
fi;
fi

if [[ -z "$uuid" ]]; then
uuid="false";
else
uuid="true";
fi

if [[ -z "$label" ]]; then
label="unknown";
else
mount_point="$label";
fi

if [ "$fstab_path" ]; then
fstab_line=`grep_line $fstab_path $i`;
if [ "$fstab_line" ]; then
if [[ -z "$filesystem" ]]; then
filesystem=$(get_str_by_pos "$fstab_line" 3);
fi
mount_point=$(get_str_by_pos "$fstab_line" 2);
fi;
fi;

if $can_be_mounted; then
if [[ -z "$mount_point" ]]; then 
mount_point="/$basename_partition";
fi
fi;

if [[ -z "$filesystem" ]]; then
filesystem=emmc;
fi

if [ "$filesystem" != "emmc" ] && [ "$can_be_mounted" != "true" ]; then
can_be_mounted=true;
fi

comment "Partition number: $partition_counter";
comment "INFO=\"`get_partition_info $basename_partition`\"";
comment "ID=\"`get_custom_id`\"";
if $have_blockdev; then
comment "DEFAULT_SIZE_IN_BYTES=\"$partition_size\" DEFAULT_FORMAT=\"$filesystem\"";
else
comment "DEFAULT_FORMAT=\"$filesystem\"";
fi
comment "DEFAULT_LABEL=\"$label\" CAN_BE_MOUNTED=\"$can_be_mounted\"";
if $have_blockdev; then
comment "BLOCK_SIZE=\"$block_size\" PARTITION_HAVE_UUID=\"$uuid\"";
else
comment "PARTITION_HAVE_UUID=\"$uuid\"";
fi

if [ "$mount_point" ]; then
if [[ "$mount_point" != "/"* ]]; then
mount_point="/${mount_point}";
fi 
fi

comment "MMC_BLOCK=\"$mmcblk\" DEFAULT_MOUNTPOINT=\"$mount_point\"";

display_value="`get_display_value $basename_partition`";
flags_value="`get_twrp_partition_flags $basename_partition`";

if [ "$fstab_path" ]; then
while read -r line; do
if [[ "$line" != "#"* ]]; then
stock_fstab_flags=$(get_str_by_pos "$line" 1);
if [[ "$stock_fstab_flags" != "$i" ]]; then
continue;
fi
stock_fstab_flags=$(get_str_by_pos "$line" 5);
for stock_flag in "${stock_fstab_flags//,/ }"; do
for custom_flag in $PARTITION_FLAGS_TO_SYMLINK; do
if [[ "$stock_flag" == "$custom_flag"* ]]; then
flags_value="$flags_value;$stock_flag";
fi
done
done
stock_fstab_flags=$(get_str_by_pos "$line" 4);
for stock_flag in "${stock_fstab_flags//,/ }"; do
if [[ "$stock_flag" == "context="* ]]; then
flags_value="$flags_value;fsflags=$stock_fstab_flags";
break;
fi
done
fi;
done < "$fstab_path";
fi

if $have_blockdev; then
partition_ro=`$blockdev_path/blockdev --getro $i`;
if [ "$partition_ro" = "0" ]; then
partition_ro="false";
elif [ "$partition_ro" = "1" ]; then
partition_ro="true";
else
partition_ro="false";
fi
else
partition_ro="false";
fi;

echo "        <!-- `get_partition_info $basename_partition` -->" >> "$PARTITIONS_XML";
if [ ! "$mount_point" ]; then
echo "        <partition label=\"$basename_partition\" size_in_bytes=\"$partition_size\" type=\"`get_custom_id`\" readonly=\"$partition_ro\" partition_have_uuid=\"$uuid\" partitiondevice=\"$REAL_PARTITIONS_PATH/$basename_partition\" blockdevice=\"$mmcblk\" can_be_mounted=\"$can_be_mounted\" filesystem=\"$filesystem\"/>" >> "$PARTITIONS_XML";
else
echo "        <partition label=\"$basename_partition\" size_in_bytes=\"$partition_size\" type=\"`get_custom_id`\" readonly=\"$partition_ro\" partition_have_uuid=\"$uuid\" partitiondevice=\"$REAL_PARTITIONS_PATH/$basename_partition\" blockdevice=\"$mmcblk\" can_be_mounted=\"$can_be_mounted\" mount_point=\"$mount_point\" filesystem=\"$filesystem\"/>" >> "$PARTITIONS_XML";
fi

if [ "$mount_point" ]; then
basename_partition=${mount_point##*/}
fi;

if [[ "$basename_partition" = *"_mnt" ]]; then
basename_partition=${basename_partition%_mnt}
fi
write_value "/$basename_partition $filesystem $i flags=display=\"$display_value\"$flags_value";
echo "/$basename_partition $filesystem $i flags=display=\"$display_value\"$flags_value" >>  "${RECOVERY_FSTAB_FILE}.minimal";

write_empty;

if [ "$filesystem" != "emmc" ]; then
tmp_basename="${basename_partition}_image";
comment "Partition number: $partition_counter (custom emmc format)";
comment "INFO=\"`get_partition_info $tmp_basename`\"";
comment "ID=\"`get_custom_id`\"";
if $have_blockdev; then
comment "DEFAULT_SIZE_IN_BYTES=\"$partition_size\" DEFAULT_FORMAT=\"unknown\"";
else
comment "DEFAULT_FORMAT=\"unknown\"";
fi
comment "DEFAULT_LABEL=\"unknown\" CAN_BE_MOUNTED=\"false\"";
if $have_blockdev; then
comment "BLOCK_SIZE=\"$block_size\" PARTITION_HAVE_UUID=\"false\"";
else
comment "PARTITION_HAVE_UUID=\"false\"";
fi

comment "MMC_BLOCK=\"$mmcblk\" DEFAULT_MOUNTPOINT=\"unknown\"";

display_value="`get_display_value $tmp_basename`";
flags_value=";flashimg=1";

write_value "/$tmp_basename emmc $i flags=display=\"$display_value\"$flags_value";
echo "/$tmp_basename emmc $i flags=display=\"$display_value\"$flags_value" >>  "${RECOVERY_FSTAB_FILE}.minimal";
write_empty;

fi

done;

echo "    </physical_partition>" >> "$PARTITIONS_XML";
echo -n "</partitions>" >> "$PARTITIONS_XML";


comment "VoldManaged volumes:";

volumes_counter=0;
if [ "$fstab_path" ]; then
while read -r line; do
if [[ "$line" != "#"* ]]; then
stock_fstab_flags=$(get_str_by_pos "$line" 5);
for i in ${stock_fstab_flags//,/ }; do
if [[ "$i" = "voldmanaged="* ]]; then
(( volumes_counter++ ))
fi;
done;
fi
done < $fstab_path
fi;

comment "Total number of VoldManaged volumes: $volumes_counter";

if [ $volumes_counter -ne 0 ]; then
total_number_of_volumes=$volumes_counter;
volumes_counter=0;
write_empty;
while read -r line; do
if [[ "$line" != "#"* ]]; then
stock_fstab_flags=$(get_str_by_pos "$line" 5);
for i in ${stock_fstab_flags//,/ }; do
if [[ "$i" = "voldmanaged="* ]]; then
(( volumes_counter++ ))
print "Processing VoldManaged volume: $volumes_counter of: $total_number_of_volumes";
mount_point=$(get_str_by_pos "$line" 2);
block_device=$(get_str_by_pos "$line" 1);
filesystem=$(get_str_by_pos "$line" 3);
if [ "$mount_point" = "auto" ] || [ "$filesystem" = "auto" ]; then
local vold_mount_info=${i##*=}
local vold_mount_point=${vold_mount_info%:*};
local vold_mount_fs=${vold_mount_info##*:}
if [ "$mount_point" != "auto" ]; then
mount_point="/$vold_mount_point";
fi;
if [ "$filesystem" != "auto" ]; then
filesystem="$vold_mount_fs";
fi;
fi;
comment "VoldManaged volume number: $volumes_counter";
if [[ "$mount_point" = *"/"* ]] || [[ "$mount_point" = "/"* ]]; then 
basename_partition=${mount_point##*/}
else
basename_partition="$mount_point";
fi;
comment "INFO=\"`get_partition_info $basename_partition`\"";
comment "ID=\"`get_custom_id`\"";
comment "DEFAULT_MOUNTPOINT=\"$mount_point\"";
display_value="`get_display_value $basename_partition`";
flags_value=";storage;wipeingui;removable";
for stock_flag in ${stock_fstab_flags//,/ }; do
for custom_flag in $PARTITION_FLAGS_TO_SYMLINK; do
if [[ "$stock_flag" == "$custom_flag"* ]]; then
flags_value="$flags_value;$stock_flag";
fi
done
done
stock_fstab_flags=$(get_str_by_pos "$line" 4);
for stock_flag in ${stock_fstab_flags//,/ }; do
if [[ "$stock_flag" == "context="* ]]; then
flags_value="$flags_value;fsflags=$stock_fstab_flags";
break;
fi
done
write_value "/$basename_partition $filesystem $block_device flags=display=\"$display_value\"$flags_value";
echo "/$basename_partition $filesystem $block_device flags=display=\"$display_value\"$flags_value" >>  "${RECOVERY_FSTAB_FILE}.minimal";
write_empty;
fi;
done
fi
done < $fstab_path
fi;




END_OF_EXECUTION=`date +%s`
runtime=$((END_OF_EXECUTION-START_OF_EXECUTION))
echo -e "©`date +%Y` ATGDroid\nSIGNATURE=\"`get_custom_id`\"" >> "$WORKDIR/device_tree.info";
echo "Maintained by ${CT_MAINTAINER}" >> "$WORKDIR/device_tree.info";
echo "Generated based on `derive_build_fingerprint`" >> "$WORKDIR/device_tree.info";
echo "This device tree was created in $runtime seconds" >> "$WORKDIR/device_tree.info";
print "Script took $runtime seconds";
exit 0;


