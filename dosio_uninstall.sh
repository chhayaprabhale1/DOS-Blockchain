#! /bin/bash

binaries=(cldos
          dosio-abigen
          dosio-launcher
          dosio-s2wasm
          dosio-wast2wasm
          dosiocpp
          kdosd
          noddos
          dosio-applesdemo)

if [ -d "/usr/local/dosio" ]; then
   printf "\tDo you wish to remove this install? (requires sudo)\n"
   select yn in "Yes" "No"; do
      case $yn in
         [Yy]* )
            if [ "$(id -u)" -ne 0 ]; then
               printf "\n\tThis requires sudo, please run ./dosio_uninstall.sh with sudo\n\n"
               exit -1
            fi

            pushd /usr/local &> /dev/null
            rm -rf dosio
            pushd bin &> /dev/null
            for binary in ${binaries[@]}; do
               rm ${binary}
            done
            # Handle cleanup of directories created from installation
            if [ "$1" == "--full" ]; then
               if [ -d ~/Library/Application\ Support/dosio ]; then rm -rf ~/Library/Application\ Support/dosio; fi # Mac OS
               if [ -d ~/.local/share/dosio ]; then rm -rf ~/.local/share/dosio; fi # Linux
            fi
            popd &> /dev/null
            break;;
         [Nn]* )
            printf "\tAborting uninstall\n\n"
            exit -1;;
      esac
   done
fi
