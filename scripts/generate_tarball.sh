#! /bin/bash

NAME=$1
DOS_PREFIX=${PREFIX}/${SUBPREFIX}
mkdir -p ${PREFIX}/bin/
#mkdir -p ${PREFIX}/lib/cmake/${PROJECT}
mkdir -p ${DOS_PREFIX}/bin 
mkdir -p ${DOS_PREFIX}/licenses/dosio
#mkdir -p ${DOS_PREFIX}/include
#mkdir -p ${DOS_PREFIX}/lib/cmake/${PROJECT}
#mkdir -p ${DOS_PREFIX}/cmake
#mkdir -p ${DOS_PREFIX}/scripts

# install binaries 
cp -R ${BUILD_DIR}/bin/* ${DOS_PREFIX}/bin 

# install licenses
cp -R ${BUILD_DIR}/licenses/dosio/* ${DOS_PREFIX}/licenses

# install libraries
#cp -R ${BUILD_DIR}/lib/* ${DOS_PREFIX}/lib

# install cmake modules
#sed "s/_PREFIX_/\/${SPREFIX}/g" ${BUILD_DIR}/modules/DosioTesterPackage.cmake &> ${DOS_PREFIX}/lib/cmake/${PROJECT}/DosioTester.cmake
#sed "s/_PREFIX_/\/${SPREFIX}\/${SSUBPREFIX}/g" ${BUILD_DIR}/modules/${PROJECT}-config.cmake.package &> ${DOS_PREFIX}/lib/cmake/${PROJECT}/${PROJECT}-config.cmake

# install includes
#cp -R ${BUILD_DIR}/include/* ${DOS_PREFIX}/include

# make symlinks
#pushd ${PREFIX}/lib/cmake/${PROJECT} &> /dev/null
#ln -sf ../../../${SUBPREFIX}/lib/cmake/${PROJECT}/${PROJECT}-config.cmake ${PROJECT}-config.cmake
#ln -sf ../../../${SUBPREFIX}/lib/cmake/${PROJECT}/DosioTester.cmake DosioTester.cmake
#popd &> /dev/null

pushd ${PREFIX}/bin &> /dev/null
for f in `ls ${BUILD_DIR}/bin/`; do
   bn=$(basename $f)
   ln -sf ../${SUBPREFIX}/bin/$bn $bn
done
popd &> /dev/null

tar -cvzf $NAME ./${PREFIX}/*
rm -r ${PREFIX}
