# Building DTK on ALCF ressources

## Software environment
The MPICH wrappers of the GNU Compiler Collection (GCC) were used to build DTK on Mira/Cetus.
Below is the corresponding SoftEnv configuration file `$HOME/.soft.mira`.
```
+cmake-3.3.0
+git-2.3.0
+bgqtoolchain-gcc484
+mpiwrapper-gcc
@default
```
We were able to use the available versions of BLAS/LAPACK and Boost (1.58.0) that were on the machine in `/soft/libraries`.

## Configuring Trilinos with DTK
This directory contains a configuration script ([alcf_mira_cmake](alcf_mira_cmake)) to build DTK and its test suite on Mira/Cetus.

## Building and testing
```bash
resoft $HOME/.soft.mira
wget https://github.com/trilinos/Trilinos/archive/trilinos-release-12-8-1.tar.gz \
  -O trilinos-release.tar.gz
mkdir trilinos-release && tar -xf trilinos-release.tar.gz -C trilinos-release \
  --strip-components=1
cd trilinos-release && export TRILINOS_DIR=$PWD
git clone https://github.com/ORNL-CEES/DataTransferKit.git
cd DataTransferKit
mkdir build && cd build
../scripts/alcf_mira_cmake
make -j 8
qsub -n 1 -t 60 -I
ctest -V
```
