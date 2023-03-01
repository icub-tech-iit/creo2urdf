/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

#ifndef OTKXSAVEINTFDATACXX_H
#define OTKXSAVEINTFDATACXX_H

#include <wfcGeometry.h>
#include <wfcModel.h>


class CreateIntfData
{
public:
	void CreateIntfDataFile (wfcInterfaceData_ptr IntfData, xstring modelName, ofstream& infoFile);

	void CreateEdgeSurfaceData (wfcEdgeSurfaceData_ptr EdgeSurfData, int count, int i, xstring className, ofstream& edgeInfoFile);

	void CreateSurfaceDescriptor(wfcWSurfaceDescriptor_ptr SurfDesc, int i, xstring string, xstring className, ofstream& surfInfoFile);

	void CreateCurveDescriptor (pfcCurveDescriptor_ptr CurveDesc, int i, xstring string, xstring className, ofstream& curveInfoFile);

	void CreateSurfaceExtents(pfcSurfaceExtents_ptr SurfExtents, int i, xstring className, xstring string, ofstream& surfExtInfoFile);

	void CreateTransformedSurface(pfcTransformedSurfaceDescriptor_ptr TransSurfDesc, int i, xstring className, xstring string, ofstream& transInfoFile);

	void CreateCompCurveDescriptor (pfcCurveDescriptor_ptr CurveDesc, int i, xstring string, xstring className, ofstream& ccInfoFile);

	void CreateSplinePoints (pfcSplinePoints_ptr SplPoints, int i, xstring string, xstring className, ofstream& splineInfoFile);

	void CreatePfcSplineSurfaceDescriptor(pfcSplineSurfaceDescriptor_ptr SplineSurf, int i, xstring string, xstring className, ofstream& splSurfInfoFile);

	pfcMatrix3D_ptr CreatePfcMatrix3D(xreal a1, xreal a2, xreal a3, xreal a4,
									  xreal b1, xreal b2, xreal b3, xreal b4,
									  xreal c1, xreal c2, xreal c3, xreal c4,
									  xreal d1, xreal d2, xreal d3, xreal d4);

	pfcUVOutline_ptr CreatePfcUVOutline(pfcUVParams_ptr param1, pfcUVParams_ptr param2);

	pfcPoint3D_ptr CreatePfcPoint3D (xreal X, xreal Y, xreal Z);

	pfcVector3D_ptr CreatePfcVector3D (xreal X, xreal Y, xreal Z);

	void CreateWfcSurfaceDescriptor(wfcWSurfaceDescriptors_ptr SurfaceDescs, xstring className, ofstream& infoFile);
	
	void CreateWfcQuiltDatas(wfcQuiltDatas_ptr Quiltdatas, xstring className, ofstream& infoFile);

	void CreateWfcEdgeDescriptor(wfcEdgeDescriptors_ptr EdgeDescs, xstring className, ofstream& infoFile);

	pfcUVParams_ptr CreatePfcUVParams(xreal param1, xreal param2);
};

class otkxTypeEnums
{
public:
	char* wfcAccuracyTypeGet (int Id);
	char* wfcEdgeDirectionTypeGet (int Id);
	char* wfcContourTraversalGet (int Id);
	char* wfcSurfaceOrientationGet (int Id);
	char* wfcCurveDirectionGet (int Id);
};

void CreatePfcBSplinePoints(pfcPoint3D_ptr Point, xreal weight, pfcBSplinePoints_ptr BPoints);
void CreatePfcSplinePoints(xreal param, pfcPoint3D_ptr Point, pfcVector3D_ptr Tangent, pfcSplinePoints_ptr Points);
void CreatePfcSplineSurfacePoint(pfcUVParams_ptr UVParam, pfcPoint3D_ptr PointD, pfcVector3D_ptr UTangent, pfcVector3D_ptr VTangent, pfcVector3D_ptr UVDeriv, pfcSplineSurfacePoints_ptr splPoint);
void CreatePfcTransform3D(pfcVector3D_ptr XAxis, pfcVector3D_ptr YAxis, pfcVector3D_ptr ZAxis, pfcPoint3D_ptr Origin, pfcTransform3D_ptr Trasform3D);
void CreatePfcUVParamsSeq(xreal param1, xreal param2, pfcUVParamsSequence_ptr UVParamSeq);

void CreatePfcUVParamsSeqArr(xint count, xreal param1, xreal param2, pfcUVParamsSequence_ptr UVParamSeq);
void PrintEdgeIdArray(xintsequence_ptr EdgeIds, xstring string, int i, ofstream& sinfoFile);
void CreateEdgeIdArray(xintsequence_ptr EdgeId, int size, ...);

wfcInterfaceData_ptr OtkUtilInterfaceData();
wfcQuiltDatas_ptr OtkUtilQuiltDatasCreate();
wfcEdgeDescriptors_ptr OtkUtilEdgeDescriptorCreate();
wfcWSurfaceDescriptors_ptr OtkUtilSurfaceDescriptorCreate();

wfcInterfaceData_ptr OtkUtilInterfaceData_Torus() ;
wfcWSurfaceDescriptors_ptr OtkUtilSurfaceDescriptorCreate_Torus();
wfcEdgeDescriptors_ptr OtkUtilEdgeDescriptorCreate_Torus();
wfcQuiltDatas_ptr OtkUtilQuiltDatasCreate_Torus();

wfcInterfaceData_ptr OtkUtilInterfaceData_Tab() ;
wfcWSurfaceDescriptors_ptr OtkUtilSurfaceDescriptorCreate_Tab();
wfcEdgeDescriptors_ptr OtkUtilEdgeDescriptorCreate_Tab();
wfcQuiltDatas_ptr OtkUtilQuiltDatasCreate_Tab();

wfcInterfaceData_ptr OtkUtilInterfaceData_Rev() ;
wfcWSurfaceDescriptors_ptr OtkUtilSurfaceDescriptorCreate_Rev();
wfcQuiltDatas_ptr OtkUtilQuiltDatasCreate_Rev();
wfcEdgeDescriptors_ptr OtkUtilEdgeDescriptorCreate_Rev();

#endif /* OTKXSAVEINTFDATACXX_H */
