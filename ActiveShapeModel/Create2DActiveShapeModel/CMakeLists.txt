PROJECT( 2dasm )

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

FIND_PACKAGE ( ITK )
IF (  ITK_FOUND  )
         INCLUDE(  ${USE_ITK_FILE}  )
ENDIF(   ITK_FOUND   )


INCLUDE_DIRECTORIES(${imfilter_SOURCE_DIR})

ADD_EXECUTABLE( 2dasm   2dasm.cxx )

TARGET_LINK_LIBRARIES ( 
  2dasm
  ITKAlgorithms  
  ITKBasicFilters 
  ITKCommon 
  ITKIO
  ITKNumerics
  ITKSpatialObject)
