/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

#include <fstream>
#include <pfcExceptions.h>
#include <pfcGlobal.h>
#include <wfcGeometry.h>
#include <OTKXUtils.h> 


// *************************************************************************************************************** /
void OtkUtilInterfaceDataInfo(wfcInterfaceData_ptr wInterfaceData, ofstream& InterfInfoFile)
{

	// ************* surface data info
	wfcWSurfaceDescriptors_ptr SurfDescs = wInterfaceData->GetSurfaceData ();

	InterfInfoFile << "******************* Interface Data : Surface Info ********************* " << endl;
	if(SurfDescs != 0)
	{
		InterfInfoFile << "\t Total of Surface Descriptor : " << SurfDescs->getarraysize() << endl;
		for(int i = 0; i < SurfDescs->getarraysize(); i++)
		{		
			InterfInfoFile << "\t--------- Surface Descriptor : " << i+1 << " -------- " << endl;
			OtkUtilSurfaceDescriptorInfo(SurfDescs->get(i), InterfInfoFile);
		}
	}
	else
		InterfInfoFile << "\t Total of Surface Descriptor : 0" << endl;
	// ************* Quilt data info 
	wfcQuiltDatas_ptr QuiltDatas = wInterfaceData->GetQuiltData ();

	InterfInfoFile << "\n******************* Interface Data : Quilt Info ********************* " << endl;
	if(QuiltDatas != 0)
	{
		InterfInfoFile << "\t Total of Quilt Data : " << QuiltDatas->getarraysize() << endl;
		for(int j = 0; j < QuiltDatas->getarraysize(); j++)
		{		
			wfcQuiltData_ptr QuiltData = QuiltDatas->get(j);
		
			InterfInfoFile << "\t--------- Quilt Data : " << j+1 << " -------- " << endl;
			InterfInfoFile << "\t Quilt Id : " << QuiltData->GetQuiltId() << endl;
			
			wfcWSurfaceDescriptors_ptr wQuiltSurfDescs = QuiltData->GetSurfaceDescriptors ();

			InterfInfoFile << "\t Total of Quilt Surface Descriptor : " << wQuiltSurfDescs->getarraysize() << endl;
			for(int k = 0; k < wQuiltSurfDescs->getarraysize(); k++)
			{
				InterfInfoFile << "\t--------- Quilt Surface Descriptor : " << k+1 << " -------- " << endl;
				OtkUtilSurfaceDescriptorInfo(wQuiltSurfDescs->get(k), InterfInfoFile);
			}
		} // for(j)
	} // if(QuiltDatas)
	else
		InterfInfoFile << "\t Total of Quilt Data : 0" << endl;
			
	// ************* Datum data info 
	wfcDatumDatas_ptr DatumDatas = wInterfaceData->GetDatumData();

	InterfInfoFile << "\n******************* Interface Data : Datum Info ********************* " << endl;
	if(DatumDatas != 0)
	{
		InterfInfoFile << "\t Total of Datum Data : " << DatumDatas->getarraysize() << endl;
		for(int x = 0; x < DatumDatas->getarraysize(); x++)
		{
			wfcDatumData_ptr DatumData = DatumDatas->get(x);

			InterfInfoFile << "\t--------- Datum Data : " << x+1 << " -------- " << endl;
			InterfInfoFile << "\t Datum Id : " << DatumData->GetId() << endl;
			InterfInfoFile << "\t Datum Name : " << DatumData->GetName() << endl;

			wfcDatumObject_ptr DatumObject = DatumData->GetDatumObject();

			if(DatumObject->GetDatumObjectType() == wfcDATUM_CURVE)
			{
				InterfInfoFile << "\t Datum Object Type : CURVE" << endl;
				wfcCurveDatumObject_ptr CurveDatumObject = wfcCurveDatumObject::cast(DatumObject);

				pfcCurveDescriptor_ptr CurveDesc = CurveDatumObject->GetCurve();

				OtkUtilCurveDescriptorInfo (CurveDesc, InterfInfoFile);				
			}
			else if(DatumObject->GetDatumObjectType() == wfcDATUM_PLANE)
			{
				InterfInfoFile << "\t Datum Object Type : PLANE" << endl;
				wfcPlaneDatumObject_ptr PlaneDatumObject = wfcPlaneDatumObject::cast(DatumObject);

				wfcWPlaneData_ptr wPlaneData = PlaneDatumObject->GetPlaneData();
				
				pfcVector3D_ptr XVector = wPlaneData->GetXAxis ();
				InterfInfoFile << "\t X- Axis : [ " << XVector->get(0) << " " << XVector->get(1) << " " << XVector->get(2) << " ] " << endl;

				pfcVector3D_ptr YVector = wPlaneData->GetYAxis ();
				InterfInfoFile << "\t Y- Axis : [ " << YVector->get(0) << " " << YVector->get(1) << " " << YVector->get(2) << " ] " << endl;

				pfcVector3D_ptr ZVector = wPlaneData->GetZAxis ();
				InterfInfoFile << "\t Z- Axis : [ " << ZVector->get(0) << " " << ZVector->get(1) << " " << ZVector->get(2) << " ] " << endl;

				pfcPoint3D_ptr Origin = wPlaneData->GetOrigin ();
				InterfInfoFile << "\t Origin : [ " << Origin->get(0) << " " << Origin->get(1) << " " << Origin->get(2) << " ] " << endl;
			}
			else if(DatumObject->GetDatumObjectType() == wfcDATUM_CSYS)
			{
				InterfInfoFile << "\t Datum Object Type : CSYS" << endl;
				wfcCsysDatumObject_ptr CsysDatumObject = wfcCsysDatumObject::cast(DatumObject);

				wfcWCsysData_ptr wCsysData = CsysDatumObject->GetCsysData();

				pfcVector3D_ptr XVector = wCsysData->GetXAxis ();
				InterfInfoFile << "\t X- Axis : [ " << XVector->get(0) << " " << XVector->get(1) << " " << XVector->get(2) << " ] " << endl;

				pfcVector3D_ptr YVector = wCsysData->GetYAxis ();
				InterfInfoFile << "\t Y- Axis : [ " << YVector->get(0) << " " << YVector->get(1) << " " << YVector->get(2) << " ] " << endl;

				pfcVector3D_ptr ZVector = wCsysData->GetZAxis ();
				InterfInfoFile << "\t Z- Axis : [ " << ZVector->get(0) << " " << ZVector->get(1) << " " << ZVector->get(2) << " ] " << endl;

				pfcVector3D_ptr Origin = wCsysData->GetOrigin ();
				InterfInfoFile << "\t Origin : [ " << Origin->get(0) << " " << Origin->get(1) << " " << Origin->get(2) << " ] " << endl;
			}
		} // for(x)
	} // if(DatumDatas)
	else
		InterfInfoFile << "\t Total of Datum Data : 0" << endl;
	// ************* Edge data info 

	wfcEdgeDescriptors_ptr EdgeDescs = wInterfaceData->GetEdgeDescriptor();

	InterfInfoFile << "\n******************* Interface Data : Edge Info ********************* " << endl;
	if(EdgeDescs != 0) 
	{
		InterfInfoFile << "\t Total of Edge Data : " << EdgeDescs->getarraysize() << endl;
		for(int y = 0; y < EdgeDescs->getarraysize(); y++)
		{		
			wfcEdgeDescriptor_ptr EdgeDesc = EdgeDescs->get(y);
			InterfInfoFile << "\t--------- Edge Descriptor : " << y+1 << " -------- " << endl;
		
			wfcEdgeSurfaceData_ptr EdgeSurfData1 = EdgeDesc->GetEdgeSurface1 ();

			InterfInfoFile << "\t -- Edge Surface 1 -- " << endl;
			OtkUtilEdgeSurfaceDataInfo (EdgeSurfData1, InterfInfoFile);

			wfcEdgeSurfaceData_ptr EdgeSurfData2 = EdgeDesc->GetEdgeSurface2 ();

			InterfInfoFile << "\t -- Edge Surface 2 -- " << endl;
			OtkUtilEdgeSurfaceDataInfo (EdgeSurfData2, InterfInfoFile);

			pfcCurveDescriptor_ptr CurveDesc = EdgeDesc->GetXYZCurveData ();

			OtkUtilCurveDescriptorInfo (CurveDesc, InterfInfoFile);	
		} // for(y)
	}
	else
		InterfInfoFile << "\t Total of Edge Data : 0" << endl;
	// ************* Accuracy and Outline info 

	InterfInfoFile << "\n******************* Interface Data : Accuracy Info ********************* " << endl;
	wfcAccuracytype AccType = wInterfaceData->GetAccuracytype ();

	InterfInfoFile << "\t Accuracy Type : " << OtkUtilAccuracyTypeEnumInfo(AccType) << endl;

	InterfInfoFile << "\t Accuracy Value : " << wInterfaceData->GetAccuracy() << endl;

	pfcOutline3D_ptr wOutline = wInterfaceData->GetOutline();
		
	pfcPoint3D_ptr Point1 = wOutline->get(0);

	pfcPoint3D_ptr Point2 = wOutline->get(1);

	InterfInfoFile << "\t Outline Point 1 : [ " << Point1->get(0) << " " << Point1->get(1) << " " << Point1->get(2) << " ]" << endl;
		InterfInfoFile << "\t Outline Point 2 : [ " << Point2->get(0) << " " << Point2->get(1) << " " << Point2->get(2) << " ]" << endl;

}

// *************************************************************************************************************** /
void OtkUtilSurfaceDescriptorInfo(wfcWSurfaceDescriptor_ptr SurfDesc, ofstream& SurfInfoFile)
{
	SurfInfoFile << "\t Surface Id : " << SurfDesc->GetSurfaceId() << endl;
	wfcContourDescriptors_ptr ContourDescs = SurfDesc->GetContourData();
	
	if(ContourDescs != 0)
	{	
	 for(int j = 0; j < ContourDescs->getarraysize(); j++)
	 {
		wfcContourDescriptor_ptr ContourDesc = ContourDescs->get(j);

		SurfInfoFile << "\t Contour Traversal : " << OtkUtilContourTraversalEnumInfo(ContourDesc->GetContourTraversal()) << endl;
		
		xintsequence_ptr EdgeIds = ContourDesc->GetEdgeIds();

		for(int k = 0; k < EdgeIds->getarraysize(); k++)
		{
			SurfInfoFile << "\t Edge Id : " << EdgeIds->get(k)<< endl;
		}
	 } // for(int j)
	} // if(ContourDescs )
	
	switch(SurfDesc->GetSurfaceType())
	{
		case pfcSURFACE_PLANE:
		{
			SurfInfoFile << "\t Surface Descriptor type : pfcSURFACE_PLANE " << endl;
			pfcSurfaceDescriptor_ptr pSurfDesc = pfcSurfaceDescriptor::cast(SurfDesc);
				
			pfcPlaneDescriptor_ptr Plane = pfcPlaneDescriptor::cast(pSurfDesc);
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(Plane);

			OtkUtilTransSurfaceDescriptorInfo (TransSurfDesc, SurfInfoFile);
			break;
		}
		case pfcSURFACE_CYLINDER:
		{
			SurfInfoFile << "\t Surface Descriptor type : pfcSURFACE_CYLINDER " << endl;
			pfcSurfaceDescriptor_ptr pSurfDesc = pfcSurfaceDescriptor::cast(SurfDesc);
				
			pfcCylinderDescriptor_ptr Cylinder = pfcCylinderDescriptor::cast(pSurfDesc);
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(Cylinder);

			SurfInfoFile << "\t Cylinder Radius : " << Cylinder->GetRadius() << endl;
			OtkUtilTransSurfaceDescriptorInfo (TransSurfDesc, SurfInfoFile);
			break;
		}
		case pfcSURFACE_CONE:
		{
			SurfInfoFile << "\t Surface Descriptor type : pfcSURFACE_CONE " << endl;
			pfcSurfaceDescriptor_ptr pSurfDesc = pfcSurfaceDescriptor::cast(SurfDesc);	
				
			pfcConeDescriptor_ptr Cone = pfcConeDescriptor::cast(pSurfDesc);
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(Cone);

			SurfInfoFile << "\t Cone Alpha : " << Cone->GetAlpha() << endl;
			OtkUtilTransSurfaceDescriptorInfo (TransSurfDesc, SurfInfoFile);
			break;
		}
		case pfcSURFACE_TORUS:
		{
			SurfInfoFile << "\t Surface Descriptor type : pfcSURFACE_TORUS " << endl;
			pfcSurfaceDescriptor_ptr pSurfDesc = pfcSurfaceDescriptor::cast(SurfDesc);	
				
			pfcTorusDescriptor_ptr Torus = pfcTorusDescriptor::cast(pSurfDesc);
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(Torus);

			SurfInfoFile << "\t Torus Radius 1 : " << Torus->GetRadius1() << endl;
			SurfInfoFile << "\t Torus Radius 2 : " << Torus->GetRadius2() << endl;
			OtkUtilTransSurfaceDescriptorInfo (TransSurfDesc, SurfInfoFile);
			break;
		}
		case pfcSURFACE_RULED:		
		{
			SurfInfoFile << "\t Surface Descriptor type : pfcSURFACE_RULED " << endl;
			pfcSurfaceDescriptor_ptr pSurfDesc = pfcSurfaceDescriptor::cast(SurfDesc);
				
			pfcRuledSurfaceDescriptor_ptr Rule = pfcRuledSurfaceDescriptor::cast(pSurfDesc);
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(Rule);

			pfcCurveDescriptor_ptr Curve1 = Rule->GetProfile1();
			OtkUtilCurveDescriptorInfo(Curve1, SurfInfoFile);

			pfcCurveDescriptor_ptr Curve2 = Rule->GetProfile2();
			OtkUtilCurveDescriptorInfo(Curve2, SurfInfoFile);

			OtkUtilTransSurfaceDescriptorInfo (TransSurfDesc, SurfInfoFile);
			break;
		}
		case pfcSURFACE_REVOLVED:
		{
			SurfInfoFile << "\t Surface Descriptor type : pfcSURFACE_REVOLVED " << endl;
			pfcSurfaceDescriptor_ptr pSurfDesc = pfcSurfaceDescriptor::cast(SurfDesc);	
				
			pfcRevolvedSurfaceDescriptor_ptr Revolve = pfcRevolvedSurfaceDescriptor::cast(pSurfDesc);
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(Revolve);

			pfcCurveDescriptor_ptr Curve = Revolve->GetProfile();
			OtkUtilCurveDescriptorInfo(Curve, SurfInfoFile);

			OtkUtilTransSurfaceDescriptorInfo (TransSurfDesc, SurfInfoFile);
			break;
		}
		case pfcSURFACE_TABULATED_CYLINDER:
		{
			SurfInfoFile << "\t Surface Descriptor type : pfcSURFACE_TABULATED_CYLINDER " << endl;
			pfcSurfaceDescriptor_ptr pSurfDesc = pfcSurfaceDescriptor::cast(SurfDesc);
				
			pfcTabulatedCylinderDescriptor_ptr Cylinder = pfcTabulatedCylinderDescriptor::cast(pSurfDesc);
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(Cylinder);

			pfcCurveDescriptor_ptr Curve = Cylinder->GetProfile();
			OtkUtilCurveDescriptorInfo(Curve, SurfInfoFile);

			OtkUtilTransSurfaceDescriptorInfo (TransSurfDesc, SurfInfoFile);
			break;
		}
		case pfcSURFACE_FILLET:
		{
			SurfInfoFile << "\t Surface Descriptor type : pfcSURFACE_FILLET " << endl;
			pfcSurfaceDescriptor_ptr pSurfDesc = pfcSurfaceDescriptor::cast(SurfDesc);	
				
			pfcFilletSurfaceDescriptor_ptr Fillet = pfcFilletSurfaceDescriptor::cast(pSurfDesc);

			pfcSplineDescriptor_ptr U0Profile = Fillet->GetU0Profile();
			OtkUtilSplineDescriptorInfo(U0Profile, SurfInfoFile);

			pfcSplineDescriptor_ptr CenterProfile = Fillet->GetCenterProfile();
			OtkUtilSplineDescriptorInfo(CenterProfile, SurfInfoFile);

			pfcSplineDescriptor_ptr TanProfile = Fillet->GetTangentProfile();			
			OtkUtilSplineDescriptorInfo(TanProfile, SurfInfoFile);
			break;
		}
		case pfcSURFACE_COONS_PATCH:
		{					
			SurfInfoFile << "\t Surface Descriptor type : pfcSURFACE_COONS_PATCH " << endl;
			pfcSurfaceDescriptor_ptr pSurfDesc = pfcSurfaceDescriptor::cast(SurfDesc);	

			pfcCoonsPatchDescriptor_ptr Patch = pfcCoonsPatchDescriptor::cast(pSurfDesc);

			pfcCurveDescriptor_ptr U0Profile = Patch->GetU0Profile();
			OtkUtilCurveDescriptorInfo(U0Profile, SurfInfoFile);

			pfcCurveDescriptor_ptr U1Profile = Patch->GetU1Profile();
			OtkUtilCurveDescriptorInfo(U1Profile, SurfInfoFile);

			pfcCurveDescriptor_ptr V0Profile = Patch->GetV0Profile();
			OtkUtilCurveDescriptorInfo(V0Profile, SurfInfoFile);

			pfcCurveDescriptor_ptr V1Profile = Patch->GetV1Profile();
			OtkUtilCurveDescriptorInfo(V1Profile, SurfInfoFile);

			pfcCoonsCornerPoints_ptr Corner = Patch->GetCornerPoints ();
			pfcCoonsUVDerivatives_ptr Deriv = Patch->GetUVDerivatives ();
			break;
		}
		case pfcSURFACE_SPLINE:
		{
			SurfInfoFile << "\t Surface Descriptor type : pfcSURFACE_SPLINE " << endl;
			pfcSurfaceDescriptor_ptr pSurfDesc = pfcSurfaceDescriptor::cast(SurfDesc);	

			pfcSplineSurfaceDescriptor_ptr Spline = pfcSplineSurfaceDescriptor::cast(pSurfDesc);

			OtkUtilSplineSurfDescriptorInfo(Spline, SurfInfoFile);
			break;
		}
		case pfcSURFACE_NURBS:
		{
			SurfInfoFile << "\t Surface Descriptor type : pfcSURFACE_NURBS " << endl;
			pfcSurfaceDescriptor_ptr pSurfDesc = pfcSurfaceDescriptor::cast(SurfDesc);
				
			pfcNURBSSurfaceDescriptor_ptr Nurbs = pfcNURBSSurfaceDescriptor::cast(pSurfDesc);

			SurfInfoFile << "\t U Degree : " << Nurbs->GetUDegree() << endl;
			SurfInfoFile << "\t V Degree : " << Nurbs->GetVDegree() << endl;
			
			pfcBSplinePoints_ptr SplinePoints = Nurbs->GetPoints();
			for(int x = 0; x < SplinePoints->getarraysize(); x++)
			{
				pfcBSplinePoint_ptr SplinePoint = SplinePoints->get(x);
				xreal weight = SplinePoint->GetWeight();
				if(weight != 0)
				{
					SurfInfoFile << "\t Spline Point weight : " << weight << endl;
				}
				pfcPoint3D_ptr Point1 = SplinePoint->GetPoint ();
				SurfInfoFile << "\t Point : [ " << Point1->get(0) << " " << Point1->get(1) << " " << Point1->get(2) << " ]" << endl;
			}

			xrealsequence_ptr UKnots = Nurbs->GetUKnots();
			for(int i = 0; i < UKnots->getarraysize(); i++)
			{
				SurfInfoFile << "\t U Knot " << i+1 << " : " << UKnots->get(i) << endl;
			}
			xrealsequence_ptr VKnots = Nurbs->GetUKnots();
			for(int j = 0; j < VKnots->getarraysize(); j++)
			{
				SurfInfoFile << "\t V Knot " << j+1 << " : " << VKnots->get(j) << endl;
			}			
			break;
		}
		case pfcSURFACE_CYLINDRICAL_SPLINE:
		{
			SurfInfoFile << "\t Surface Descriptor type : pfcSURFACE_CYLINDRICAL_SPLINE " << endl;
			pfcSurfaceDescriptor_ptr pSurfDesc = pfcSurfaceDescriptor::cast(SurfDesc);			
				
			pfcCylindricalSplineSurfaceDescriptor_ptr cSpline = pfcCylindricalSplineSurfaceDescriptor::cast(pSurfDesc);
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(cSpline);
	
			pfcSplineSurfaceDescriptor_ptr SplineSurf = cSpline->GetSplineSurfaceData ();
			
			OtkUtilSplineSurfDescriptorInfo(SplineSurf, SurfInfoFile);

			OtkUtilTransSurfaceDescriptorInfo (TransSurfDesc, SurfInfoFile);
			break;
		}
		case pfcSURFACE_SPHERICAL_SPLINE:
		{
			SurfInfoFile << "\t Surface Descriptor type : pfcSURFACE_SPHERICAL_SPLINE " << endl;
			pfcSurfaceDescriptor_ptr pSurfDesc = pfcSurfaceDescriptor::cast(SurfDesc);			
				
			pfcSphericalSplineSurfaceDescriptor_ptr sSpline = pfcSphericalSplineSurfaceDescriptor::cast(pSurfDesc);
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(sSpline);
	
			pfcSplineSurfaceDescriptor_ptr SplineSurf = sSpline->GetSplineSurfaceData ();
			
			OtkUtilSplineSurfDescriptorInfo(SplineSurf, SurfInfoFile);

			OtkUtilTransSurfaceDescriptorInfo (TransSurfDesc, SurfInfoFile);
			break;
		}
		case pfcSURFACE_FOREIGN:
		{
			SurfInfoFile << "\t Surface Descriptor type : pfcSURFACE_FOREIGN " << endl;
			pfcSurfaceDescriptor_ptr pSurfDesc = pfcSurfaceDescriptor::cast(SurfDesc);			
				
			pfcForeignSurfaceDescriptor_ptr Foreign = pfcForeignSurfaceDescriptor::cast(pSurfDesc);
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(Foreign);

			SurfInfoFile << "\t Foreign Id : " << Foreign->GetForeignID() << endl;
			OtkUtilTransSurfaceDescriptorInfo (TransSurfDesc, SurfInfoFile);
			break;
		}
	} // switch()
	
	SurfInfoFile << "\t Surface Orientation : " << OtkUtilSurfaceOrientationEnumInfo(SurfDesc->GetOrientation()) << endl;
	
	pfcSurfaceExtents_ptr SurfExtents = SurfDesc->GetExtents();

	if(SurfExtents != 0)
	{
		pfcUVOutline_ptr UVOutline = SurfExtents->GetUVExtents();

		pfcUVParams_ptr UVParams1 = UVOutline->get(0);

		pfcUVParams_ptr UVParams2 = UVOutline->get(1);

		SurfInfoFile << "\t UV Param 1 : [ " << UVParams1->get(0) << " " << UVParams1->get(1) << " ]" << endl;
		SurfInfoFile << "\t UV Param 2 : [ " << UVParams2->get(0) << " " << UVParams2->get(1) << " ]" << endl;

		pfcOutline3D_ptr XYZOutline = SurfExtents->GetXYZExtents();

		pfcPoint3D_ptr Point1 = XYZOutline->get(0);

		pfcPoint3D_ptr Point2 = XYZOutline->get(1);

		SurfInfoFile << "\t XYZ Point 1 : [ " << Point1->get(0) << " " << Point1->get(1) << " " << Point1->get(2) << " ]" << endl;
		SurfInfoFile << "\t XYZ Point 2 : [ " << Point2->get(0) << " " << Point2->get(1) << " " << Point2->get(2) << " ]" << endl;
	}
}

// *************************************************************************************************************** /
void OtkUtilEdgeSurfaceDataInfo(wfcEdgeSurfaceData_ptr EdgeSurfData, ofstream& SurfInfoFile)
{
	SurfInfoFile << "\t Surface Id : " << EdgeSurfData->GetEdgeSurfaceId() << endl;
	SurfInfoFile << "\t Surface Direction : " << EdgeSurfData->GetDirection() << endl;
	
	pfcUVParamsSequence_ptr UVParams = EdgeSurfData->GetUVParamsSequence ();

	for(int i = 0; i < UVParams->getarraysize(); i++)
	{
		pfcUVParams_ptr UVParam = UVParams->get(i);
		SurfInfoFile << "\t UV Param " << i+1 << " : [ " <<  UVParam->get(0) << " " << UVParam->get(1) << " ]" << endl;
	}
	pfcCurveDescriptor_ptr CurveDesc = EdgeSurfData->GetUVCurveData ();

	if(CurveDesc != 0)
		OtkUtilCurveDescriptorInfo(CurveDesc, SurfInfoFile);	
}
// *************************************************************************************************************** /
void OtkUtilTransSurfaceDescriptorInfo (pfcTransformedSurfaceDescriptor_ptr TransDesc, ofstream& SurfInfoFile)
{
		 pfcTransform3D_ptr Trans3D = TransDesc->GetCoordSys ();

		 pfcMatrix3D_ptr Matrix = Trans3D->GetMatrix ();

		 pfcVector3D_ptr VecX = Trans3D->GetXAxis ();

		 pfcVector3D_ptr VecY = Trans3D->GetYAxis ();

		 pfcVector3D_ptr VecZ = Trans3D->GetZAxis ();

		 pfcPoint3D_ptr  Origin = Trans3D->GetOrigin ();

		 SurfInfoFile << "\t Matrix : " <<endl;
		 for(int a = 0; a < 4; a++)
		 {
			SurfInfoFile << "\t [ " << Matrix->get(a, 0) << " " <<  Matrix->get(a, 1)
				<< " " << Matrix->get(a, 2) << " " << Matrix->get(a, 3) << " ] " << endl;	
		 }

		 SurfInfoFile << "\t X- Axis : [ " << VecX->get(0) << " " << VecX->get(1) << " " << VecX->get(2) << " ] " << endl;
		 SurfInfoFile << "\t Y- Axis : [ " << VecY->get(0) << " " << VecY->get(1) << " " << VecY->get(2) << " ] " << endl;
		 SurfInfoFile << "\t Z- Axis : [ " << VecZ->get(0) << " " << VecZ->get(1) << " " << VecZ->get(2) << " ] " << endl;

		 SurfInfoFile << "\t Point : [ " << Origin->get(0) << " " << Origin->get(1) << " " << Origin->get(2) << " ]" << endl;
}
// *************************************************************************************************************** /
void OtkUtilCurveDescriptorInfo (pfcCurveDescriptor_ptr CurveDesc, ofstream& CurveInfoFile)
{
	pfcCurveType CurveType = CurveDesc->GetCurveType();
	switch(CurveType)
	{
		case pfcCURVE_COMPOSITE:
		{
			CurveInfoFile << "\t CurveDescriptor type : pfcCURVE_COMPOSITE" << endl;
			wfcWCompositeCurveDescriptor_ptr Composite = wfcWCompositeCurveDescriptor::cast(CurveDesc);

			pfcCurveDescriptors_ptr CurvDescs = Composite->GetElements ();
			int compNumbers = CurvDescs->getarraysize();

			wfcCurveDirections_ptr CompDirs = Composite->GetCompDirections();
			
			CurveInfoFile << "\t ------ Composite Curve Info ------- " << endl;
			CurveInfoFile << "\t Number of components: "<<compNumbers << endl;
							
			for(int i = 0; i < compNumbers; i++)
			{
				CurveInfoFile << "\t Component #"<<(i+1)<<endl;
				wfcCurveDirection CompDir = CompDirs->get(i);
				
				if(CompDir == wfcCURVE_NO_FLIP)
					CurveInfoFile << "\t Direction: wfcCURVE_NO_FLIP"<< endl;	
				else if(CompDir == wfcCURVE_FLIP)
					CurveInfoFile << "\t Direction: wfcCURVE_FLIP"<< endl;					

				OtkUtilCurveDescriptorInfo(CurvDescs->get(i), CurveInfoFile);			
			}
			break;
		}
		case pfcCURVE_POINT:
		{
			CurveInfoFile << "\t CurveDescriptor type : pfcCURVE_POINT" << endl;

			pfcPointDescriptor_ptr Point = pfcPointDescriptor::cast(CurveDesc);
			pfcPoint3D_ptr Point3D = Point->GetPoint ();

			CurveInfoFile << "\t Point : [ " << Point3D->get(0) << " " << Point3D->get(1) << " " << Point3D->get(2) << " ]" << endl;
			break;
		}
		case pfcCURVE_LINE:
		{
			CurveInfoFile << "\t CurveDescriptor type : pfcCURVE_LINE" << endl;

			pfcLineDescriptor_ptr Line = pfcLineDescriptor::cast(CurveDesc);
			pfcPoint3D_ptr Point1 = Line->GetEnd1 ();
			pfcPoint3D_ptr Point2 = Line->GetEnd2 ();

			CurveInfoFile << "\t End 1 : [ " << Point1->get(0) << " " << Point1->get(1) << " " << Point1->get(2) << " ]" << endl;
			CurveInfoFile << "\t End 2 : [ " << Point2->get(0) << " " << Point2->get(1) << " " << Point2->get(2) << " ]" << endl;
			break;
		}
		case pfcCURVE_ARROW:
		{
			CurveInfoFile << "\t CurveDescriptor type : pfcCURVE_ARROW" << endl;

			pfcArrowDescriptor_ptr Arrow = pfcArrowDescriptor::cast(CurveDesc);
			pfcPoint3D_ptr Point1 = Arrow->GetEnd1 ();
			pfcPoint3D_ptr Point2 = Arrow->GetEnd2 ();

			CurveInfoFile << "\t End 1 : [ " << Point1->get(0) << " " << Point1->get(1) << " " << Point1->get(2) << " ]" << endl;
			CurveInfoFile << "\t End 2 : [ " << Point2->get(0) << " " << Point2->get(1) << " " << Point2->get(2) << " ]" << endl;
			break;
		}
		case pfcCURVE_ARC:
		{
			CurveInfoFile << "\t CurveDescriptor type : pfcCURVE_ARC" << endl;

			pfcArcDescriptor_ptr Arc = pfcArcDescriptor::cast(CurveDesc);
			pfcVector3D_ptr Point1 = Arc->GetVector1 ();

			pfcVector3D_ptr Point2 = Arc->GetVector2 ();

			pfcPoint3D_ptr Center = Arc->GetCenter ();

			CurveInfoFile << "\t Vector 1 : [ " << Point1->get(0) << " " << Point1->get(1) << " " << Point1->get(2) << " ]" << endl;
			CurveInfoFile << "\t Vector 2 : [ " << Point2->get(0) << " " << Point2->get(1) << " " << Point2->get(2) << " ]" << endl;
			CurveInfoFile << "\t Center  : [ " << Center->get(0) << " " << Center->get(1) << " " << Center->get(2) << " ]" << endl;
			CurveInfoFile << "\t Start Angle : " << Arc->GetStartAngle () << endl;
			CurveInfoFile << "\t End Angle : " << Arc->GetEndAngle () << endl;
			CurveInfoFile << "\t Arc Radius : " << Arc->GetRadius () << endl;
			break;
		}
		case pfcCURVE_SPLINE:
		{
			CurveInfoFile << "\t CurveDescriptor type : pfcCURVE_SPLINE" << endl;

			pfcSplineDescriptor_ptr SplineDesc = pfcSplineDescriptor::cast(CurveDesc);
			OtkUtilSplineDescriptorInfo(SplineDesc, CurveInfoFile);
			
			break;
		}
		case pfcCURVE_BSPLINE:
		{
			CurveInfoFile << "\t CurveDescriptor type : pfcCURVE_BSPLINE" << endl;

			pfcBSplineDescriptor_ptr BSplineDesc = pfcBSplineDescriptor::cast(CurveDesc);
			pfcBSplinePoints_ptr BSplinePoint = BSplineDesc->GetPoints ();

			for(int i = 0; i < BSplinePoint->getarraysize(); i++)
			{
				pfcBSplinePoint_ptr BSpline = BSplinePoint->get(i);
				xreal weight = BSpline->GetWeight ();
				if(weight != 0)
					CurveInfoFile << "\t Weight : " << weight << endl;

				pfcPoint3D_ptr Point3D = BSpline->GetPoint();								
				CurveInfoFile << "\t Point : [ " << Point3D->get(0) << " " << Point3D->get(1) << " " << Point3D->get(2) << " ]" << endl;
			}
			CurveInfoFile << "\t Degree : " << BSplineDesc->GetDegree () << endl;
			xrealsequence_ptr knots =  BSplineDesc->GetKnots ();

			for(int j = 0; j < knots->getarraysize(); j++)
				CurveInfoFile << "\t Knot " << j+1 << " : " << knots->get(j) << endl;
			break;
		}
		case pfcCURVE_CIRCLE:
		{
			CurveInfoFile << "\t CurveDescriptor type : pfcCURVE_CIRCLE" << endl;			
			pfcCircleDescriptor_ptr Circle = pfcCircleDescriptor::cast(CurveDesc);
			pfcPoint3D_ptr Point = Circle->GetCenter ();

			pfcVector3D_ptr Vector = Circle->GetUnitNormal ();

			CurveInfoFile << "\t Point : [ " << Point->get(0) << " " << Point->get(1) << " " << Point->get(2) << " ]" << endl;
			CurveInfoFile << "\t Unit Normal : [ " << Vector->get(0) << " " << Vector->get(1) << " " << Vector->get(2) << " ]" << endl;
			CurveInfoFile << "\t Radius : " << Circle->GetRadius () << endl;			
			break;
		}
		case pfcCURVE_ELLIPSE:
		{
			CurveInfoFile << "\t CurveDescriptor type : pfcCURVE_ELLIPSE" << endl;			
			pfcEllipseDescriptor_ptr Ellipse = pfcEllipseDescriptor::cast(CurveDesc);
			pfcPoint3D_ptr Point = Ellipse->GetCenter ();

			pfcVector3D_ptr Vector = Ellipse->GetUnitNormal ();

			pfcVector3D_ptr Axis = Ellipse->GetUnitMajorAxis ();

			CurveInfoFile << "\t Point : [ " << Point->get(0) << " " << Point->get(1) << " " << Point->get(2) << " ]" << endl;
			CurveInfoFile << "\t Unit Normal : [ " << Vector->get(0) << " " << Vector->get(1) << " " << Vector->get(2) << " ]" << endl;
			CurveInfoFile << "\t Unit Major Axis : [ " << Axis->get(0) << " " << Axis->get(1) << " " << Axis->get(2) << " ]" << endl;
			CurveInfoFile << "\t Major Length : " << Ellipse->GetMajorLength () << endl;
			CurveInfoFile << "\t Minor Length : " << Ellipse->GetMinorLength () << endl;
			CurveInfoFile << "\t Start Angle : " << Ellipse->GetStartAngle () << endl;
			CurveInfoFile << "\t End Angle : " << Ellipse->GetEndAngle () << endl;
			break;
		}
		case pfcCURVE_POLYGON:
		{
			CurveInfoFile << "\t CurveDescriptor type : pfcCURVE_POLYGON" << endl;			
			pfcPolygonDescriptor_ptr Polygon = pfcPolygonDescriptor::cast(CurveDesc);
			pfcPoint3Ds_ptr Points = Polygon->GetVertices ();

			for(int i = 0; i < Points->getarraysize(); i++)
			{
				pfcPoint3D_ptr Point = Points->get(i);
				CurveInfoFile << "\t Point : [ " << Point->get(0) << " " << Point->get(1) << " " << Point->get(2) << " ]" << endl;
			}			
			break;
		}
		case pfcCURVE_TEXT:
		{
			CurveInfoFile << "\t CurveDescriptor type : pfcCURVE_TEXT" << endl;			
			pfcTextDescriptor_ptr Text = pfcTextDescriptor::cast(CurveDesc);
			pfcPoint3D_ptr Point = Text->GetPoint ();

			pfcTextStyle_ptr TextStyle = Text->GetStyle ();

			CurveInfoFile << "\t Point : [ " << Point->get(0) << " " << Point->get(1) << " " << Point->get(2) << " ]" << endl;
			CurveInfoFile << "\t Text : " << Text->GetTextString () << endl; 

			CurveInfoFile << "\t ---- TextStyle info ---- " << endl; 
			CurveInfoFile << "\t\t Angle : " << TextStyle->GetAngle ()<< endl; 
			CurveInfoFile << "\t\t Font Name : " << TextStyle->GetFontName ()<< endl; 
			CurveInfoFile << "\t\t Height : " << TextStyle->GetHeight ()<< endl; 
			CurveInfoFile << "\t\t Width Factor : " << TextStyle->GetWidthFactor ()<< endl; 
			CurveInfoFile << "\t\t Slant Angle : " << TextStyle->GetSlantAngle ()<< endl; 
			CurveInfoFile << "\t\t Thickness : " << TextStyle->GetThickness ()<< endl; 
			CurveInfoFile << "\t\t IsUnderlined : " << TextStyle->GetIsUnderlined ()<< endl; 
			CurveInfoFile << "\t\t IsMirrored : " << TextStyle->GetIsMirrored ()<< endl; 
			break;
		}
	}
}
// *************************************************************************************************************** /
void OtkUtilCompCurveDescriptorInfo (pfcCurveDescriptor_ptr CurveDesc, ofstream& CCurveInfoFile)
{
	CCurveInfoFile << "\t ------ Composite Curve Info ------- " << endl;
	OtkUtilCurveDescriptorInfo(CurveDesc, CCurveInfoFile);
}
// *************************************************************************************************************** /
void OtkUtilSplineDescriptorInfo(pfcSplineDescriptor_ptr SplineDesc, ofstream& SplineInfoFile)
{
	pfcSplinePoints_ptr  SplinePoints = SplineDesc->GetPoints ();

	for(int i = 0; i < SplinePoints->getarraysize(); i++)
	{
		pfcSplinePoint_ptr Spline = SplinePoints->get(i);
		pfcPoint3D_ptr Point3D = Spline->GetPoint();

		pfcVector3D_ptr Vector = Spline->GetTangent();

		SplineInfoFile << "\t Parameter : " << Spline->GetParameter () << endl;
		SplineInfoFile << "\t Tangent : [ " << Vector->get(0) << " " << Vector->get(1) << " " << Vector->get(2) << " ]" << endl;
		SplineInfoFile << "\t Point : [ " << Point3D->get(0) << " " << Point3D->get(1) << " " << Point3D->get(2) << " ]" << endl;
	}
}
// *************************************************************************************************************** /
void OtkUtilSplineSurfDescriptorInfo(pfcSplineSurfaceDescriptor_ptr cSpline, ofstream& SurfInfoFile)
{
	pfcSplineSurfacePoints_ptr Points = cSpline->GetPoints ();
	for(int i = 0; i < Points->getarraysize(); i++)
	{
		pfcSplineSurfacePoint_ptr Point = Points->get(i);
		pfcUVParams_ptr	Params = Point->GetParameter ();

		SurfInfoFile << "\t UV Param  : [ " << Params->get(0) << " " << Params->get(1) << " ]" << endl;

		pfcPoint3D_ptr Point3D = Point->GetPoint ();
		SurfInfoFile << "\t Point : [ " << Point3D->get(0) << " " << Point3D->get(1) << " " << Point3D->get(2) << " ]" << endl;

		pfcVector3D_ptr UTangent = Point->GetUTangent ();
		if(UTangent != 0)
			SurfInfoFile << "\t U Tangent : [ " << UTangent->get(0) << " " << UTangent->get(1) << " " << UTangent->get(2) << " ]" << endl;

		pfcVector3D_ptr VTangent = Point->GetVTangent ();
		if(VTangent != 0)
			SurfInfoFile << "\t V Tangent : [ " << VTangent->get(0) << " " << VTangent->get(1) << " " << VTangent->get(2) << " ]" << endl;

		pfcVector3D_ptr Deriv = Point->GetUVDerivative ();
		if(Deriv != 0)
			SurfInfoFile << "\t Derivative : [ " << Deriv->get(0) << " " << Deriv->get(1) << " " << Deriv->get(2) << " ]" << endl;
	}

}
// *************************************************************************************************************** /
char* OtkUtilAccuracyTypeEnumInfo(xint Id)
{
	switch(Id)
	{
		case wfcACCU_NONE:		return((char*)"ACCU_NONE");
		case wfcACCU_RELATIVE:		return((char*)"ACCU_RELATIVE");
		case wfcACCU_ABSOLUTE:		return((char*)"ACCU_ABSOLUTE");
		case wfcAccuracytype_nil:	return((char*)"Accuracytype_nil");		
		
		default: return (char*) "*** Unknown enum in method OtkAccuracyTypeEnumLookup";
	}
}
// *************************************************************************************************************** /
char* OtkUtilContourTraversalEnumInfo(xint Id)
{
	switch(Id)
	{
		case pfcCONTOUR_TRAV_NONE:		return((char*)"CONTOUR_TRAV_NONE");
		case pfcCONTOUR_TRAV_INTERNAL:		return((char*)"CONTOUR_TRAV_INTERNAL");
		case pfcCONTOUR_TRAV_EXTERNAL:		return((char*)"CONTOUR_TRAV_EXTERNAL");
		case pfcContourTraversal_nil:		return((char*)"ContourTraversal_nil");		
		
		default: return (char*) "*** Unknown enum in method OtkContourTraversalEnumLookup";
	}
}
// *************************************************************************************************************** /
char* OtkUtilSurfaceOrientationEnumInfo(xint Id)
{
	switch(Id)
	{
		case pfcSURFACEORIENT_NONE:		return((char*)"SURFACEORIENT_NONE");
		case pfcSURFACEORIENT_OUTWARD:		return((char*)"SURFACEORIENT_OUTWARD");
		case pfcSURFACEORIENT_INWARD:		return((char*)"SURFACEORIENT_INWARD");
		case pfcSurfaceOrientation_nil:		return((char*)"SurfaceOrientation_nil");		
		
		default: return (char*) "*** Unknown enum in method OtkSurfaceOrientationEnumLookup";
	}
}
// *************************************************************************************************************** /
