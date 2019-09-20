# DOS-Blockchain
DOS blockchain Source code

For Download

#### Ubuntu 18.04 Debian Package Install
```sh
$ git clone https://github.com/Akshay-Wagh/DOS-Blockchain.git
```
#### Ubuntu 16.04 Debian Package Install
```sh
$ git clone https://github.com/Akshay-Wagh/DOS-Blockchain.git
```
####
Please Check CMakeFile to compile code....

For compile and run You need following dependecies

Install the development toolkit:
#####
sudo apt-get update

wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add -
sudo apt-get install clang-4.0 lldb-4.0 libclang-4.0-dev cmake make \
                     libbz2-dev libssl-dev libgmp3-dev \
                     autotools-dev build-essential \
                     libbz2-dev libicu-dev python-dev \
                     autoconf libtool git mongodb
#####
Install Boost 1.66:

cd ~
wget -c 'https://sourceforge.net/projects/boost/files/boost/1.66.0/boost_1_66_0.tar.bz2/download' -O boost_1.66.0.tar.bz2
tar xjf boost_1.66.0.tar.bz2
cd boost_1_66_0/
echo "export BOOST_ROOT=$HOME/boost_1_66_0" >> ~/.bash_profile
source ~/.bash_profile
./bootstrap.sh "--prefix=$BOOST_ROOT"
./b2 install
source ~/.bash_profile

#####
Install mongo-cxx-driver (release/stable):

cd ~
curl -LO https://github.com/mongodb/mongo-c-driver/releases/download/1.9.3/mongo-c-driver-1.9.3.tar.gz
tar xf mongo-c-driver-1.9.3.tar.gz
cd mongo-c-driver-1.9.3
./configure --enable-static --enable-ssl=openssl --disable-automatic-init-and-cleanup --prefix=/usr/local
make -j$( nproc )
sudo make install
git clone https://github.com/mongodb/mongo-cxx-driver.git --branch releases/stable --depth 1
cd mongo-cxx-driver/build
cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
sudo make -j$( nproc )

#####
Install secp256k1-zkp (Cryptonomex branch):

cd ~
git clone https://github.com/cryptonomex/secp256k1-zkp.git
cd secp256k1-zkp
./autogen.sh
./configure
make
sudo make install
By default LLVM and clang do not include the WASM build target, so you will have to build it yourself:

#####
mkdir  ~/wasm-compiler
cd ~/wasm-compiler
git clone --depth 1 --single-branch --branch release_40 https://github.com/llvm-mirror/llvm.git
cd llvm/tools
git clone --depth 1 --single-branch --branch release_40 https://github.com/llvm-mirror/clang.git
cd ..
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=.. -DLLVM_TARGETS_TO_BUILD= -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=WebAssembly -DCMAKE_BUILD_TYPE=Release ../
make -j4 install

#####

cd ~/path-to/DOS-Blockchain 
make



