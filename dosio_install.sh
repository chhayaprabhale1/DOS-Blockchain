#!/bin/bash


if [ "$(id -u)" -ne 0 ]; then
        printf "\n\tThis requires sudo. Please run with sudo.\n\n"
        exit -1
fi   

   CWD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
   if [ "${CWD}" != "${PWD}" ]; then
      printf "\\n\\tPlease cd into directory %s to run this script.\\n \\tExiting now.\\n\\n" "${CWD}"
      exit 1
   fi

   BUILD_DIR="${PWD}/build"
   CMAKE_BUILD_TYPE=Release
   TIME_BEGIN=$( date -u +%s )
   INSTALL_PREFIX="/usr/local/dosio"
   VERSION=1.2

   txtbld=$(tput bold)
   bldred=${txtbld}$(tput setaf 1)
   txtrst=$(tput sgr0)

   create_symlink() {
      pushd /usr/local/bin &> /dev/null
      ln -sf ../dosio/bin/$1 $1
      popd &> /dev/null
   }

   create_cmake_symlink() {
      mkdir -p /usr/local/lib/cmake/dosio
      pushd /usr/local/lib/cmake/dosio &> /dev/null
      ln -sf ../../../dosio/lib/cmake/dosio/$1 $1
      popd &> /dev/null
   }

   install_symlinks() {
      printf "\\n\\tInstalling DOSIO Binary Symlinks\\n\\n"
      create_symlink "cldos"
      create_symlink "dosio-abigen"
      create_symlink "dosio-launcher"
      create_symlink "dosio-s2wasm"
      create_symlink "dosio-wast2wasm"
      create_symlink "dosiocpp"
      create_symlink "kdosd"
      create_symlink "noddos"
   }

   if [ ! -d "${BUILD_DIR}" ]; then
      printf "\\n\\tError, dosio_build.sh has not ran.  Please run ./dosio_build.sh first!\\n\\n"
      exit -1
   fi

   ${PWD}/scripts/clean_old_install.sh
   if [ $? -ne 0 ]; then
      printf "\\n\\tError occurred while trying to remove old installation!\\n\\n"
      exit -1
   fi

   if ! pushd "${BUILD_DIR}"
   then
      printf "Unable to enter build directory %s.\\n Exiting now.\\n" "${BUILD_DIR}"
      exit 1;
   fi
   
   if ! make install
   then
      printf "\\n\\t>>>>>>>>>>>>>>>>>>>> MAKE installing DOSIO has exited with the above error.\\n\\n"
      exit -1
   fi
   popd &> /dev/null 

   install_symlinks   
   create_cmake_symlink "dosio-config.cmake"

   printf "\n\n${bldred}\t _______  _______  _______ _________ _______\n"
   printf '\t(  ____ \(  ___  )(  ____ \\\\__   __/(  ___  )\n'
   printf "\t| (    \/| (   ) || (    \/   ) (   | (   ) |\n"
   printf "\t| (__    | |   | || (_____    | |   | |   | |\n"
   printf "\t|  __)   | |   | |(_____  )   | |   | |   | |\n"
   printf "\t| (      | |   | |      ) |   | |   | |   | |\n"
   printf "\t| (____/\| (___) |/\____) |___) (___| (___) |\n"
   printf "\t(_______/(_______)\_______)\_______/(_______)\n${txtrst}"

   printf "\\tFor more information:\\n"
   printf "\\tDOSIO website: https://dos.io\\n"
   printf "\\tDOSIO Telegram channel @ https://t.me/DOSProject\\n"
   printf "\\tDOSIO resources: https://dos.io/resources/\\n"
   printf "\\tDOSIO Stack Exchange: https://dosio.stackexchange.com\\n"
   printf "\\tDOSIO wiki: https://github.com/DOSIO/dos/wiki\\n\\n\\n"
