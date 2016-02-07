#########################################################################################################
# 1) ParadisEO install: SIMPLE Configuration
#########################################################################################################

#  Here, just specify PARADISEO_DIR : the directory where ParadisEO has been installed
SET(PARADISEO_DIR "/vagrant" CACHE PATH "ParadisEO directory" FORCE)

#########################################################################################################


#########################################################################################################
# 2) ParadisEO install: ADVANCED Configuration
#########################################################################################################

SET(PARADISEO_EO_SRC_DIR "${PARADISEO_DIR}/eo" CACHE PATH "ParadisEO-EO source directory" FORCE)
SET(PARADISEO_EO_BIN_DIR "${PARADISEO_DIR}/eo/build" CACHE PATH "ParadisEO-EO binary directory" FORCE)

SET(PARADISEO_MO_SRC_DIR "${PARADISEO_DIR}/mo" CACHE PATH "ParadisEO-MO source directory" FORCE)
SET(PARADISEO_MO_BIN_DIR "${PARADISEO_DIR}/mo/build" CACHE PATH "ParadisEO-MO binary directory" FORCE)

SET(PARADISEO_MOEO_SRC_DIR "${PARADISEO_DIR}/moeo" CACHE PATH "ParadisEO-MOEO source directory" FORCE)
SET(PARADISEO_MOEO_BIN_DIR "${PARADISEO_DIR}moeo/build" CACHE PATH "ParadisEO-MOEO binary directory" FORCE)

SET(PARADISEO_PEO_SRC_DIR "${PARADISEO_DIR}/peo" CACHE PATH "ParadisEO-PEO source directory" FORCE)
SET(PARADISEO_PEO_BIN_DIR "${PARADISEO_DIR}/peo/build" CACHE PATH "ParadisEO-PEO binary directory" FORCE)

#########################################################################################################


######################################################################################
### 3) OPTIONNAL: Windows advanced config - especially for Microsoft Visual Studio 8
######################################################################################

  IF(CMAKE_CXX_COMPILER MATCHES cl)
   IF(NOT WITH_SHARED_LIBS)
     IF(CMAKE_GENERATOR STREQUAL "Visual Studio 8 2005")
       SET(CMAKE_CXX_FLAGS "/nologo /W3 /Gy")
       SET(CMAKE_CXX_FLAGS_DEBUG "/MTd /Z7 /Od")
       SET(CMAKE_CXX_FLAGS_RELEASE "/MT /O2")
       SET(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O2")
       SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MTd /Z7 /Od")
       SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
     ENDIF(CMAKE_GENERATOR STREQUAL "Visual Studio 8 2005")
   ENDIF(NOT WITH_SHARED_LIBS)
  ENDIF(CMAKE_CXX_COMPILER MATCHES cl)

######################################################################################
