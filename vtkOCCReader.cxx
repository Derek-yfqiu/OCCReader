// Copyright (C) 2010-2013  CEA/DEN, EDF R&D
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "vtkOCCReader.h"

#include "vtkPoints.h"
#include "vtkIntArray.h"
#include "vtkCellData.h"
#include "vtkCellTypes.h"
#include "vtkCharArray.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkUnstructuredGrid.h"
#include "vtkAppendPolyData.h"
#include "vtkPolyDataWriter.h"
//
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkGenericAttributeCollection.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
//
#include "vtksys/stl/string"
#include "vtksys/ios/fstream"
#include "vtksys/stl/algorithm"



//Work with IOR.
//#include <SALOMEDS.hh>
//#include <GEOM_Client.hxx>
//#include <GeometryGUI.h>

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopTools_ListOfShape.hxx>

#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>

#include <BRepBndLib.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include "BRep_Builder.hxx"
#include "IGESControl_Reader.hxx"
#include "STEPControl_Reader.hxx"

#define MAX2(X, Y)    (Abs(X) > Abs(Y) ? Abs(X) : Abs(Y))
#define MAX3(X, Y, Z) (MAX2(MAX2(X,Y), Z))

//

vtkStandardNewMacro(vtkOCCReader);
//vtkCxxRevisionMacro(vtkOCCReader,"$Revision: 1.2.2.2 $");


vtkOCCReader::vtkOCCReader()
{
//    this->myFileName = "";
    this->FileName = NULL;
    this->MyDataSet=0;
    this->DispMode = 2;  //default shading
    this->myDeflection = 0.001;

    this->SetNumberOfInputPorts(0);
    this->SetNumberOfOutputPorts(1);

}

vtkOCCReader::~vtkOCCReader()
{
      this->SetFileName(0);
}

void vtkOCCReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf( os, indent );
  os << "Data: " << this->MyDataSet << "\n";
}

//const char *vtkOCCReader::GetFileName()
//{
//  return myFileName.c_str();
//}

//void vtkOCCReader::SetFileName(const char *FileName)
//{
//  vtkWarningMacro("File name seting!")
//  if(!FileName)
//    return;
//  if(FileName[0]=='\0')
//    return;
//  int length=strlen(FileName);
//  myFileName.copy((char *)FileName,length);
//  this->Modified();
//}

//void vtkOCCReader::SetBufferingPolicy(int pol)
//{
//  BufferingPolicy=pol;
//}

//int vtkOCCReader::GetBufferingPolicy()
//{
//  return BufferingPolicy;
//}

void vtkOCCReader::SetMode(int dispMode)
{
    //display mode: 1- wireframe; 2-shading 3-wireframe+shading
    DispMode = (dispMode > 0 && dispMode < 4) ? dispMode : 2;
    this->Modified();  //invoke update
}

void vtkOCCReader::SetDeflection(double aDeflection)
{
    //smoothing
    myDeflection = ( aDeflection>= 0.00001 && aDeflection <= 0.001) ? aDeflection : 0.001;
    this->Modified();  //invoke update
}

int vtkOCCReader::getMode()
{
  return DispMode;
}
double vtkOCCReader::getDeflection()
{
  return myDeflection;
}
 //int vtkOCCReader::RequestUpdateExtent( vtkInformation* request, vtkInformationVector** inInfo, vtkInformationVector* outInfo )
 //{
 //return this->Superclass::RequestUpdateExtent(request,inInfo,outInfo);

  /*vtkOCC2VTKCorbaDataSet* output = vtkOCC2VTKCorbaDataSet::SafeDownCast( info->Get( vtkDataObject::DATA_OBJECT() ) );
  if ( ! output )
    {
    output = vtkOCC2VTKCorbaDataSet::New();
    output->SetPipelineInformation( info );
    output->Delete();
    this->GetOutputPortInformation( 0 )->Set( vtkDataObject::DATA_EXTENT_TYPE(), output->GetExtentType() );
    }*/

 // return 1;
 //}

int vtkOCCReader::ProcessRequest(vtkInformation* request,
                                         vtkInformationVector** inputVector,
                                         vtkInformationVector* outputVector)
{
  // generate the data
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
    {
    return this->RequestData(request, inputVector, outputVector);
    }
  return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}

int vtkOCCReader::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
    return 1;
}

void MeshShape(const TopoDS_Shape theShape,
               float& theDeflection,
               bool theForced )
{

  Standard_Real aDeflection = theDeflection <= 0 ? 0.0001 : theDeflection;

  //If deflection <= 0, than return default deflection
  if(theDeflection <= 0)
    theDeflection = aDeflection;

  // Is shape triangulated?
  Standard_Boolean alreadymeshed = Standard_True;
  TopExp_Explorer ex;
  TopLoc_Location aLoc;
  for (ex.Init(theShape, TopAbs_FACE); ex.More(); ex.Next()) {
    const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
    Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
    if(aPoly.IsNull()) {
  alreadymeshed = Standard_False;
  break;
    }
  }

  if(!alreadymeshed || theForced) {
    Bnd_Box B;
    BRepBndLib::Add(theShape, B);
    if ( B.IsVoid() )
  return; // NPAL15983 (Bug when displaying empty groups)
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

    // This magic line comes from Prs3d_ShadedShape.gxx in OCCT
    aDeflection = MAX3(aXmax-aXmin, aYmax-aYmin, aZmax-aZmin) * aDeflection * 4;

    //Clean triangulation before compute incremental mesh
    BRepTools::Clean(theShape);

    //Compute triangulation
    BRepMesh_IncrementalMesh MESH(theShape,aDeflection);
  }
}

void SetShape(const TopoDS_Shape& theShape,
              const TopTools_IndexedDataMapOfShapeListOfShape& theEdgeMap,
              bool theIsVector,
              GEOM_EdgeSource* theIsolatedEdgeSource,
              GEOM_EdgeSource* theOneFaceEdgeSource,
              GEOM_EdgeSource* theSharedEdgeSource,
              GEOM_WireframeFace* theWireframeFaceSource,
              GEOM_ShadingFace* theShadingFaceSource)
{
  if (theShape.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator anItr(theShape);
    for (; anItr.More(); anItr.Next()) {
      SetShape(anItr.Value(),theEdgeMap,theIsVector,
               theIsolatedEdgeSource,
               theOneFaceEdgeSource,
               theSharedEdgeSource,
               theWireframeFaceSource,
               theShadingFaceSource);
    }
  }

  switch (theShape.ShapeType()) {
    case TopAbs_WIRE: {
      TopExp_Explorer anEdgeExp(theShape,TopAbs_EDGE);
      for (; anEdgeExp.More(); anEdgeExp.Next()){
        const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
        if (!BRep_Tool::Degenerated(anEdge))
          theIsolatedEdgeSource->AddEdge(anEdge,theIsVector);
      }
      break;
    }
    case TopAbs_EDGE: {
      const TopoDS_Edge& anEdge = TopoDS::Edge(theShape);
      if (!BRep_Tool::Degenerated(anEdge))
        theIsolatedEdgeSource->AddEdge(anEdge,theIsVector);
      break;
    }
    case TopAbs_VERTEX: {
      break;
    }
    default: {
      TopExp_Explorer aFaceExp (theShape,TopAbs_FACE);
      for(; aFaceExp.More(); aFaceExp.Next()) {
        const TopoDS_Face& aFace = TopoDS::Face(aFaceExp.Current());
        theWireframeFaceSource->AddFace(aFace);
        theShadingFaceSource->AddFace(aFace);
        TopExp_Explorer anEdgeExp(aFaceExp.Current(), TopAbs_EDGE);
        for(; anEdgeExp.More(); anEdgeExp.Next()) {
          const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
          if(!BRep_Tool::Degenerated(anEdge)){
            // compute the number of faces
            int aNbOfFaces = theEdgeMap.FindFromKey(anEdge).Extent();
            switch(aNbOfFaces){
            case 0:  // isolated edge
              theIsolatedEdgeSource->AddEdge(anEdge,theIsVector);
              break;
            case 1:  // edge in only one face
              theOneFaceEdgeSource->AddEdge(anEdge,theIsVector);
              break;
            default: // edge shared by at least two faces
              theSharedEdgeSource->AddEdge(anEdge,theIsVector);
            }
          }
        }
      }
    }
  }
}


int vtkOCCReader::RequestData(vtkInformation* request, vtkInformationVector** inInfo, vtkInformationVector* outputVector)
{
    vtkInformation *outInfo=outputVector->GetInformationObject(0);
    //
    vtkPolyData *ret0=vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT())); //pointer for output data

    //read the GEOM file
    TopoDS_Shape aShape ;


    if (!this->FileName)
    {
        vtkWarningMacro("File name Empty!");
        return 0;

    }
//    vtkWarningMacro("File name :" <<FileName);

    int len = strlen( FileName );
        char exten[4];
//    std::string exten ; //file extension
    //        exten = myFileName.substr(myFileName.size()-5,myFileName.size()-1);
//    exten.copy(FileName,4,len-5);
    strcpy( exten, FileName + (len - 4));
//    vtkWarningMacro("File extension: "<<exten);

    if( !strcmp( exten, "brep") ){
//    if( exten.compare("brep") ==0){
      // read brep file
      BRep_Builder aBuilder;
      BRepTools::Read(aShape,FileName,aBuilder);
    }
    else if( !strcmp( exten, "iges") || !strcmp( exten, ".igs")){
//    else if( exten.compare("iges") ==0 || exten.compare(".igs") ==0){
      //read IGES file
      IGESControl_Reader Reader;
      Standard_Integer status = Reader.ReadFile( FileName );

      Reader.TransferRoots();
      aShape = Reader.OneShape();
    }
    else if( !strcmp( exten, "step") || !strcmp( exten, ".stp")){
//    else if( exten.compare("step") ==0 || exten.compare(".stp") ==0){
      //read IGES file
      STEPControl_Reader Reader;
      Standard_Integer status = Reader.ReadFile( FileName );

      Reader.TransferRoots();
      aShape = Reader.OneShape();
//      vtkWarningMacro("Step file read!")

    }
    else
    {
      //unrecognize file extension
        vtkErrorMacro("Unrecognized file!")
      return 0;
    }

    if (aShape.IsNull())
    {
        vtkErrorMacro("The Shape is Nil!");
        return 0;
    }

    GEOM_EdgeSource* theIsolatedEdgeSource = GEOM_EdgeSource::New();
    GEOM_EdgeSource* theOneFaceEdgeSource = GEOM_EdgeSource::New();
    GEOM_EdgeSource* theSharedEdgeSource = GEOM_EdgeSource::New();
    GEOM_WireframeFace* theWireframeFaceSource = GEOM_WireframeFace::New();
    GEOM_ShadingFace* theShadingFaceSource = GEOM_ShadingFace::New();
//    float aDeflection = 0.001; //setting the face meshing effect

    // Is shape triangulated?
    bool wasMeshed = true;
    TopExp_Explorer ex;
    TopLoc_Location aLoc;
    for (ex.Init(aShape, TopAbs_FACE); ex.More(); ex.Next()) {
        const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
        Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
        if(aPoly.IsNull()) {
            wasMeshed = false;
            break;
        }
    }

    MeshShape( aShape, myDeflection, true ); //meshing the face with U V lines, theForce is set default true in GEOM

    // look if edges are free or shared
    TopTools_IndexedDataMapOfShapeListOfShape anEdgeMap;
    TopExp::MapShapesAndAncestors(aShape,TopAbs_EDGE,TopAbs_FACE,anEdgeMap);
    //call method to set the shape
    SetShape(aShape,anEdgeMap,false,
             theIsolatedEdgeSource,
             theOneFaceEdgeSource,
             theSharedEdgeSource,
             theWireframeFaceSource,
             theShadingFaceSource);

    vtkAppendPolyData *appendPolyData = vtkAppendPolyData::New();
    if (DispMode == 1) //wireframe mode
    {
        appendPolyData->AddInputConnection(theIsolatedEdgeSource->GetOutputPort()); //for display bounday frame
        appendPolyData->AddInputConnection(theOneFaceEdgeSource->GetOutputPort());//for display bounday frame
        appendPolyData->AddInputConnection(theSharedEdgeSource->GetOutputPort());//for display bounday frame
        //    appendPolyData->AddInputConnection(theWireframeFaceSource->GetOutputPort()); //no need to display
    }
    else if (DispMode == 2)
    {
        appendPolyData->AddInputConnection(theShadingFaceSource->GetOutputPort());//for display faces
    }
    else if (DispMode == 3)
    {
        appendPolyData->AddInputConnection(theIsolatedEdgeSource->GetOutputPort()); //for display bounday frame
        appendPolyData->AddInputConnection(theOneFaceEdgeSource->GetOutputPort());//for display bounday frame
        appendPolyData->AddInputConnection(theSharedEdgeSource->GetOutputPort());//for display bounday frame
        appendPolyData->AddInputConnection(theShadingFaceSource->GetOutputPort());//for display faces
    }
    else
        return 0;

    appendPolyData->Update();
    ret0->ShallowCopy(appendPolyData->GetOutput());

    theIsolatedEdgeSource->Delete();
    theOneFaceEdgeSource->Delete();
    theSharedEdgeSource->Delete();
    theWireframeFaceSource->Delete();
    theShadingFaceSource->Delete();
    return 1;
}





