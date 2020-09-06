#/bin/bash -f

##########################################################
# Version Settings
##########################################################
BOOST_VERSION="boost_1_74_0"
BOOST_DOWNLOAD_URL="http://sourceforge.net/projects/boost/files/boost/1.74.0/${BOOST_VERSION}.tar.bz2"
BOOST_REBUILD=false

CLHEP_VERSION="2.3.4.6"
CLHEP_DOWNLOAD_URL="http://proj-clhep.web.cern.ch/proj-clhep/dist1/clhep-${CLHEP_VERSION}.tgz"
CLHEP_REBUILD=false

RAVE_DOWNLOAD_URL="https://github.com/LAMPS-Collaboration/rave.git"
RAVE_BRANCH_NAME="spirit"
RAVE_REBUILD=false

GENFIT_DOWNLOAD_URL="https://github.com/LAMPS-Collaboration/GenFit.git"
#GENFIT_BRANCH_NAME="lamps"
GENFIT_BRANCH_NAME="v20180201"
GENFIT_REBUILD=false


##########################################################
# General
##########################################################
TOOLS_PATH=${KEBIPATH}/tools
TOOLS_INSTALL_PATH=${TOOLS_PATH}/install
TOOLS_LIBRARY_PATH=${TOOLS_INSTALL_PATH}/lib
TOOLS_INCLUDE_PATH=${TOOLS_INSTALL_PATH}/include
TOOLS_SHARE_PATH=${TOOLS_INSTALL_PATH}/share
mkdir -p ${TOOLS_PATH}
mkdir -p ${TOOLS_INSTALL_PATH}

if ! command -v git &> /dev/null; then
  echo "Git is not found!"
  echo "Cannot clone Rave repo. Download and install RAVE from ${RAVE_DOWNLOAD_URL} to ${RAVE_SOURCE_PATH}.${RAVE_BRANCH_NAME}"
  echo "Cannot clone GenFit repo. Download and install Genfit from ${GENFIT_DOWNLOAD_URL} to ${GENFIT_SOURCE_PATH}.${GENFIT_BRANCH_NAME}"
  exit
fi

DYLIBEXTENSION="so"
if [[ $OSTYPE == *"darwin"* ]]; then
  DYLIBEXTENSION="dylib"
fi

function download_file__ {
  if [[ $OSTYPE == *"darwin"* ]]; then
    curl -O -L $1
  else
    wget $1
  fi
}


##########################################################
# BOOST
##########################################################
BOOST_BASE_PATH="${TOOLS_PATH}/boost"
BOOST_TARFILE_FULL="${BOOST_BASE_PATH}/${BOOST_VERSION}.tar.bz2"
BOOST_SOURCE_PATH="${BOOST_BASE_PATH}/${BOOST_VERSION}/"
BOOST_BUILD_PATH="${BOOST_SOURCE_PATH}/tmp"

echo ""
if ! test -f "${BOOST_TARFILE_FULL}"; then
  BOOST_REBUILD=true
  echo "Downloading BOOST from ${BOOST_DOWNLOAD_URL} ..."
  mkdir -p ${BOOST_BASE_PATH}
  cd ${BOOST_BASE_PATH}
  download_file__ ${BOOST_DOWNLOAD_URL}
fi

if [ ! -d "${BOOST_SOURCE_PATH}" ] || [ ${BOOST_REBUILD} = true ] ; then
  BOOST_REBUILD=true
  cd ${BOOST_BASE_PATH}
  tar -xvf ${BOOST_TARFILE_FULL}
fi

if [ ! -d "${TOOLS_INCLUDE_PATH}/boost" ] || [ ${BOOST_REBUILD} = true ] ; then
  CLHEP_REBUILD=true
  echo "Building CLHEP from ${BOOST_SOURCE_PATH} ..."
  cd ${BOOST_SOURCE_PATH}
  sh bootstrap.sh
  ./b2 --build-dir=${BOOST_BUILD_PATH} --build-type=minimal --prefix=${TOOLS_INSTALL_PATH} --layout=system install

  echo "Boost is installed!"
  echo "Boost build directory: ${BOOST_SOURCE_PATH}"
  echo "Boost include directory: ${TOOLS_INCLUDE_PATH}/boost/"
else
  echo "Boost is already installed!"
  echo "Boost build directory: ${BOOST_SOURCE_PATH}"
  echo "Boost include directory: ${TOOLS_INCLUDE_PATH}/boost/"
fi


##########################################################
# CLHEP
##########################################################
CLHEP_BASE_PATH="${TOOLS_PATH}/CLHEP"
CLHEP_TARFILE_FULL="${CLHEP_BASE_PATH}/clhep-${CLHEP_VERSION}.tgz"
CLHEP_SOURCE_PATH="${CLHEP_BASE_PATH}/${CLHEP_VERSION}/CLHEP/"
CLHEP_BUILD_PATH="${CLHEP_BASE_PATH}/${CLHEP_VERSION}/build/"
CLHEP_LIB_FULL="${TOOLS_LIBRARY_PATH}/libCLHEP.${DYLIBEXTENSION}"

echo ""
if ! test -f "${CLHEP_TARFILE_FULL}"; then
  CLHEP_REBUILD=true
  mkdir -p ${CLHEP_BASE_PATH}
  cd ${CLHEP_BASE_PATH}
  echo "Downloading CLHEP from ${CLHEP_DOWNLOAD_URL} ..."
  download_file__ ${CLHEP_DOWNLOAD_URL}
fi

if [ ! -d "${CLHEP_SOURCE_PATH}" ] || [ ${CLHEP_REBUILD} = true ] ; then
  CLHEP_REBUILD=true
  tar -xvf ${CLHEP_TARFILE_FULL}
fi

if [ ! -f "${CLHEP_LIB_FULL}" ] || [ ${CLHEP_REBUILD} = true ] ; then
  CLHEP_REBUILD=true
  echo "Building CLHEP from ${CLHEP_BUILD_PATH} ..."
  mkdir -p ${CLHEP_BUILD_PATH}
  cd  ${CLHEP_BUILD_PATH}
  make clean
  cmake ${CLHEP_SOURCE_PATH} -DCMAKE_INSTALL_PREFIX=${TOOLS_INSTALL_PATH}/ -DCLHEP_SINGLE_THREAD=ON
  make -j$1
  make install

  echo "CLHEP is installed!"
  echo "CLHEP build directory: ${CLHEP_SOURCE_PATH}"
  echo "CLHEP library file: ${CLHEP_LIB_FULL}"
else
  echo "CLHEP is already installed!"
  echo "CLHEP build directory: ${CLHEP_SOURCE_PATH}"
  echo "CLHEP library file: ${CLHEP_LIB_FULL}"
fi


##########################################################
# RAVE
##########################################################
RAVE_SOURCE_PATH="${TOOLS_PATH}/Rave.${RAVE_BRANCH_NAME}"
RAVE_BUILD_PATH="${RAVE_SOURCE_PATH}"
RAVE_VERSION="${RAVE_BRANCH_NAME}"
RAVE_LIB_FULL="${TOOLS_LIBRARY_PATH}/libRaveBase.${DYLIBEXTENSION}"
RAVE_SHARE_PATH="${TOOLS_SHARE_PATH}/rave"

echo ""
if [ ${CLHEP_REBUILD} = true ] ; then
  RAVE_REBUILD=true
fi

if [ ! -d "${RAVE_SOURCE_PATH}" ] || [ ${RAVE_REBUILD} = true ] ; then
  RAVE_REBUILD=true
  echo "Downloading RAVE from ${RAVE_DOWNLOAD_URL} branch:${RAVE_BRANCH_NAME} ..."
  git clone ${RAVE_DOWNLOAD_URL} ${RAVE_SOURCE_PATH}
fi

if [ ! -d "${RAVE_BUILD_PATH}" ] || [ ${RAVE_REBUILD} = true ] ; then
  RAVE_REBUILD=true
  cd ${RAVE_SOURCE_PATH}
  git fetch
  git checkout ${RAVE_BRANCH_NAME}
fi

if [ ! -f "${RAVE_LIB_FULL}" ] || [ ${RAVE_REBUILD} = true ] ; then
  RAVE_REBUILD=true
  echo "Building RAVE from ${RAVE_BUILD_PATH} ..."
  cd ${RAVE_SOURCE_PATH}
  ./configure --prefix=${TOOLS_INSTALL_PATH}/ --disable-java -with-clhep=${TOOLS_INSTALL_PATH} --with-boost=${TOOLS_INSTALL_PATH} --with-boost-libdir=${TOOLS_LIBRARY_PATH}
  make -j$1
  make install

  echo "RAVE is installed!"
  echo "RAVE build directory: ${RAVE_SOURCE_PATH}"
  echo "RAVE library file: ${RAVE_LIB_FULL}"
else
  echo "RAVE is already installed!"
  echo "RAVE build directory: ${RAVE_SOURCE_PATH}"
  echo "RAVE library file: ${RAVE_LIB_FULL}"
fi


##########################################################
# GENFIT
##########################################################
GENFIT_SOURCE_PATH="${TOOLS_PATH}/GenFit.${GENFIT_BRANCH_NAME}"
GENFIT_BUILD_PATH="${GENFIT_SOURCE_PATH}/build"
GENFIT_LIB_FULL="${TOOLS_LIBRARY_PATH}/libgenfit2.${DYLIBEXTENSION}"

echo ""

export RAVEPATH=${RAVE_SHARE_PATH}
export BOOST_ROOT=${TOOLS_INSTALL_PATH}

if [ ! -d "${GENFIT_SOURCE_PATH}" ] || [ ${GENFIT_REBUILD} = true ] ; then
  if [ -d "${GENFIT_SOURCE_PATH}" ] && [ ${GENFIT_REBUILD} = true ]; then
    echo "Removing ${GENFIT_SOURCE_PATH} ..."
    rm -rf ${GENFIT_SOURCE_PATH}
  fi
  GENFIT_REBUILD=true
  echo "Downloading GENFIT from ${GENFIT_DOWNLOAD_URL} branch:${GENFIT_BRANCH_NAME} ..."
  git clone ${GENFIT_DOWNLOAD_URL} ${GENFIT_SOURCE_PATH}
fi

if [ ! -d "${GENFIT_BUILD_PATH}" ] || [ ${GENFIT_REBUILD} = true ] ; then
  GENFIT_REBUILD=true
  cd ${GENFIT_SOURCE_PATH}
  git fetch
  git checkout ${GENFIT_BRANCH_NAME}
fi

if [ ${RAVE_REBUILD} = true ] ; then
  GENFIT_REBUILD=true
fi

if [ ! -f "${GENFIT_LIB_FULL}" ] || [ ${GENFIT_REBUILD} = true ] ; then
  GENFIT_REBUILD=true
  echo "Building GENFIT from ${GENFIT_BUILD_PATH} ..."
  mkdir -p ${GENFIT_BUILD_PATH}
  cd  ${GENFIT_BUILD_PATH}
  cmake ${GENFIT_SOURCE_PATH} -DCMAKE_INSTALL_PREFIX=${TOOLS_INSTALL_PATH} -DBoost_NO_SYSTEM_PATHS=ON -DBoost_NO_BOOST_CMAKE=ON
  make -j$1
  make install

  echo "GENFIT is installed!"
  echo "GENFIT build directory: ${GENFIT_SOURCE_PATH}"
  echo "GENFIT library file: ${GENFIT_LIB_FULL}"
else
  echo "GENFIT is already installed!"
  echo "GENFIT build directory: ${GENFIT_SOURCE_PATH}"
  echo "GENFIT library file: ${GENFIT_LIB_FULL}"
fi


