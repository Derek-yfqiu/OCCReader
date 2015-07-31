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


#ifndef __vtkOCCReader_h
#define __vtkOCCReader_h

#define OCCReader_EXPORTS

#include "OCC2VTK.h"
#include "vtkPolyDataAlgorithm.h"
#include <vtksys/stl/vector>
//#include <GEOM_Gen.hh>

#include "GEOM_VertexSource.h"
#include "GEOM_EdgeSource.h"
#include "GEOM_WireframeFace.h"
#include "GEOM_ShadingFace.h"
#include <string>

//#include <config.h>
class vtkDoubleArray;
class vtkUnstructuredGrid;


class vtkOCCReader : public vtkAlgorithm //
{
public:
  vtkTypeMacro(vtkOCCReader, vtkAlgorithm)
  OCC2VTK_EXPORT static vtkOCCReader* New();
  virtual void PrintSelf( ostream& os, vtkIndent indent );
//  virtual const char *GetFileName();
//  virtual void SetFileName(const char *FileName);
  // Specifies the name of the file
  vtkGetStringMacro(FileName);
  vtkSetStringMacro(FileName);
//  void SetBufferingPolicy(int pol);
//  int GetBufferingPolicy();
  void SetMode(int dispMode);
  int getMode();
  void SetDeflection(double aDeflection);
  double getDeflection();
protected:
  vtkOCCReader();
  virtual ~vtkOCCReader();
  int FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info);
  int ProcessRequest(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
  virtual int RequestData( vtkInformation* request, vtkInformationVector** inInfo, vtkInformationVector* outInfo );
  int DispMode; //1-wireframe 2-shading
  float myDeflection; //for adjusting the smoothing of the shading

  vtkPointData *MyDataSet;
//  std::string myFileName;
  // name of the file to read
  char* FileName;
  //not clean but to avoid to include CORBA.h in this *.h
  static void *Orb;

  vtkOCCReader( const vtkOCCReader& ); // Not implemented.
  void operator = ( const vtkOCCReader& ); // Not implemented.
};

#endif // __vtkOCCReader_h

