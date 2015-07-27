# Copyright (C) 2010-2013  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

PROJECT( OCCReader )

  CMAKE_MINIMUM_REQUIRED( VERSION 2.8.7 )



  SET(CMAKE_BUILD_TYPE Release)
  MARK_AS_ADVANCED(
    LIBRARY_OUTPUT_PATH
    EXECUTABLE_OUTPUT_PATH
  )
#Package ParaView is under build folder.
  SET(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}) 

  IF(WIN32)
  MESSAGE( Windows system )
  SET(WINDOWS TRUE)
  ADD_DEFINITIONS(-DWNT)
  ENDIF(WIN32)

  FIND_PACKAGE( ParaView COMPONENTS pqCore NO_MODULE)

  IF( ParaView_FOUND )

    INCLUDE(${OCCReader_SOURCE_DIR}/FindCAS.cmake)
    INCLUDE_DIRECTORIES( ${CAS_INCLUDE_DIRS})

    INCLUDE( ${PARAVIEW_USE_FILE} )



    FIND_PACKAGE( VTK  COMPONENTS vtkClientServer NO_MODULE)
    FIND_PACKAGE( VTK  COMPONENTS vtkCommonDataModel NO_MODULE)
    FIND_PACKAGE( VTK  COMPONENTS vtkCommonExecutionModel NO_MODULE)
    INCLUDE( ${VTK_USE_FILE} )

    SET( OCCREADER_SRCS
      ${OCCReader_SOURCE_DIR}/vtkOCCReader.cxx
      ${OCCReader_SOURCE_DIR}/GEOM_EdgeSource.cxx  
      ${OCCReader_SOURCE_DIR}/GEOM_FaceSource.cxx  
      ${OCCReader_SOURCE_DIR}/GEOM_ShadingFace.cxx  
      ${OCCReader_SOURCE_DIR}/GEOM_VertexSource.cxx  
      ${OCCReader_SOURCE_DIR}/GEOM_WireframeFace.cxx
      )


    
    ADD_PARAVIEW_PLUGIN( OCCReader "1.0"
      SERVER_MANAGER_XML OCCReaderServerManager.xml
      SERVER_MANAGER_SOURCES ${OCCREADER_SRCS} 
      REQUIRED_ON_SERVER)

    TARGET_LINK_LIBRARIES(OCCReader
      vtkCommonDataModel
      vtkCommonExecutionModel
      vtkClientServer
      pqCore
      ${TKG3d}
      ${TKGeomBase}
      ${TKGeomAlgo}
      ${TKBRep}
      ${TKTopAlgo}
      ${TKG2d}
      ${TKMesh}
      ${TKSTEP}
      ${TKCAF}
      ${TKLCAF}
      ${TKSTEPBase}
      ${TKIGES}
      )
  
  
  ENDIF( ParaView_FOUND )

