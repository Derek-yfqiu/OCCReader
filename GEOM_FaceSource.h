// Copyright (C) 2014-2015  KIT-INR/NK 
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//

#ifndef GEOM_FACESOURCE_H 
#define GEOM_FACESOURCE_H 
#ifdef WNT
#define OCCReader_EXPORTS
#endif
#include "OCC2VTK.h" 

#include <config.h>
#include <gp_Pnt.hxx>
#include <TopoDS_Face.hxx> 
#include <NCollection_Set.hxx> 
 
typedef NCollection_Set<TopoDS_Face> TFaceSet; 
 
#include <vtkPoints.h> 
#include <vtkPolyDataAlgorithm.h> 

class vtkPolyData;

class OCC2VTK_EXPORT GEOM_FaceSource: public vtkPolyDataAlgorithm 
{ 
public: 
  vtkTypeMacro(GEOM_FaceSource,vtkPolyDataAlgorithm); 
 
  void AddFace(const TopoDS_Face& theFace); 
  void Clear(){ myFaceSet.Clear();} 
  bool IsEmpty(){return myFaceSet.IsEmpty();}
 
protected: 
  TFaceSet myFaceSet; 
 
  static 
  void MoveTo(gp_Pnt thePnt, 
              vtkPoints* thePts);
  static 
  void DrawTo(gp_Pnt thePnt,
              vtkPolyData* thePolyData,
              vtkPoints* thePts); 
 
  GEOM_FaceSource(); 
  ~GEOM_FaceSource(); 
 
private: 
  // Not implememnted 
  GEOM_FaceSource(const GEOM_FaceSource&); 
  void operator=(const GEOM_FaceSource&); 
}; 
 
 
#endif //GEOM_FACESOURCE_H 
