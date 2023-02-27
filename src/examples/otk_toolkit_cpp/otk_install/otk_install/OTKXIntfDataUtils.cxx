/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

#include <fstream>
#include <stdarg.h>
#include <pfcExceptions.h>
#include <pfcGlobal.h>
#include <wfcGeometry.h>
#include <wfcModel.h>
#include <OTKXUtils.h> 
#include <OTKXSaveIntfDataCxx.h>

// ************************************************************************************************** /
void CreateIntfData::CreateIntfDataFile (wfcInterfaceData_ptr IntfData, xstring modelName, ofstream& infoFile)
{
	CreateIntfData uIntfData;
	otkxTypeEnums uEnum;

	ofstream infoFileSurf;
	ofstream infoFileQuilt;
	ofstream infoFileEdge;

	infoFile << "#include <OTKXSaveIntfDataCxx.h>\n"<< endl;
	infoFile << "wfcInterfaceData_ptr OtkUtilInterfaceData() \n{" << endl;
	infoFile << "\t CreateIntfData uIntfData;"<< endl;

	// ******************************* SURFACE Data ********************************** /
	wfcWSurfaceDescriptors_ptr SurfDescs = IntfData->GetSurfaceData ();	
	infoFile << "\t wfcWSurfaceDescriptors_ptr wSurfDescs = wfcWSurfaceDescriptors::create(); " << endl;
	
	char surfBuffer[100];
	sprintf (surfBuffer, "SurfaceDescs_%s.cxx", const_cast<char*>((cStringT)modelName));
	infoFileSurf.open(surfBuffer, ios::out);

	infoFileSurf << "#include <OTKXSaveIntfDataCxx.h>\n"<< endl;
	infoFileSurf << "wfcWSurfaceDescriptors_ptr OtkUtilSurfaceDescriptorCreate()\n{\n"<< endl;
	infoFileSurf << "\t CreateIntfData sIntfData;"<< endl;

	infoFile <<"\t wSurfDescs = OtkUtilSurfaceDescriptorCreate();\n"<< endl;

	if(SurfDescs != 0) {	
		uIntfData.CreateWfcSurfaceDescriptor(SurfDescs, "sIntfData", infoFileSurf);
	} else {
		infoFileSurf << "\t return 0;\n}" << endl;
	}
	infoFileSurf.close();

	// ************************* Quilt Data ************************************ /
	wfcQuiltDatas_ptr QuiltDatas = IntfData->GetQuiltData();
	infoFile << " \t wfcQuiltDatas_ptr wQuiltDatas = wfcQuiltDatas::create();" << endl;

	char quiltBuffer[100];
	sprintf (quiltBuffer, "QuiltDatas_%s.cxx", const_cast<char*>((cStringT)modelName));

	infoFileQuilt.open(quiltBuffer, ios::out);

	infoFileQuilt << "#include <OTKXSaveIntfDataCxx.h>\n"<< endl;
	infoFileQuilt << "wfcQuiltDatas_ptr OtkUtilQuiltDatasCreate()\n{\n"<< endl;
	infoFileQuilt << "\t CreateIntfData qIntfData;"<< endl;

	infoFile << "\t wQuiltDatas = OtkUtilQuiltDatasCreate();\n"<< endl;
	if(QuiltDatas != 0) {
		uIntfData.CreateWfcQuiltDatas(QuiltDatas, "qIntfData", infoFileQuilt);
	} else { 
		infoFileQuilt << "\t return 0;\n }"<<endl;
	}
	infoFileQuilt.close();

	// ************************* Edge Descriptor ************************************ /
	wfcEdgeDescriptors_ptr EdgeDescs = IntfData->GetEdgeDescriptor();
	infoFile << "\t wfcEdgeDescriptors_ptr wEdgeDescs = wfcEdgeDescriptors::create();" << endl;

	char edgeBuffer[100];
	sprintf (edgeBuffer, "EdgeDescs_%s.cxx", const_cast<char*>((cStringT)modelName));

	infoFileEdge.open(edgeBuffer, ios::out);

	infoFileEdge << "#include <OTKXSaveIntfDataCxx.h>\n"<< endl;
	infoFileEdge << "wfcEdgeDescriptors_ptr OtkUtilEdgeDescriptorCreate()\n{\n"<< endl;
	infoFileEdge << "\t CreateIntfData eIntfData;"<< endl;
	
	infoFile << "\t wEdgeDescs = OtkUtilEdgeDescriptorCreate();\n" << endl;
	if(EdgeDescs != 0)
	{
		uIntfData.CreateWfcEdgeDescriptor(EdgeDescs, "eIntfData", infoFileEdge);
	} else {
		infoFileEdge << "\t return 0;\n }"<<endl;
	}
	infoFileEdge.close();

	// ************************* Datum Data ************************************ /
	wfcDatumDatas_ptr DatumDatas = IntfData->GetDatumData();
	int j = 1;

	infoFile << " \t wfcDatumDatas_ptr wDatumDatas = wfcDatumDatas::create();" << endl;
	if(DatumDatas != 0)
	{
		for(int i = 0; i < DatumDatas->getarraysize(); i++)
		{			
			wfcDatumData_ptr DatumData = DatumDatas->get(i);
			wfcDatumObject_ptr DatumObject = DatumData->GetDatumObject();
			wfcDatumObjectType DatumObjType = DatumObject->GetDatumObjectType();
			
			switch(DatumObjType)
			{
				case wfcDATUM_CURVE:
				{
					wfcCurveDatumObject_ptr CurveObject = wfcCurveDatumObject::cast(DatumObject);
					pfcCurveDescriptor_ptr CurveDesc = CurveObject->GetCurve();
					uIntfData.CreateCurveDescriptor(CurveDesc, i, "WD", "uIntfData", infoFile);					
					infoFile << "\t wfcCurveDatumObject_ptr WCurveObject" << j << " = wfcCurveDatumObject::Create(WDCurveDesc"<< j << ");" << endl;
					infoFile << "\t wfcDatumObject_ptr WDatumObject" << j << " = wfcDatumObject::cast(WCurveObject"<< j << ");\n" << endl;

					j++;
					break;
				}
				case wfcDATUM_PLANE:
				{					
					wfcPlaneDatumObject_ptr PlaneObject = wfcPlaneDatumObject::cast(DatumObject);
					wfcWPlaneData_ptr PlaneData = PlaneObject->GetPlaneData();
					
					pfcVector3D_ptr XAxis = PlaneData->GetXAxis();
					pfcVector3D_ptr YAxis = PlaneData->GetYAxis();
					pfcVector3D_ptr ZAxis = PlaneData->GetZAxis();
					pfcPoint3D_ptr Origin = PlaneData->GetOrigin();

					infoFile << "\t pfcVector3D_ptr wXAxis"<< j<<" = uIntfData.CreatePfcVector3D ( "<< XAxis->get(0) << " , "<< XAxis->get(1) << " , "<< XAxis->get(2) << " );"<< endl;					
					infoFile << "\t pfcVector3D_ptr wYAxis"<< j<<" = uIntfData.CreatePfcVector3D ( "<< YAxis->get(0) << " , "<< YAxis->get(1) << " , "<< YAxis->get(2) << " );"<< endl;
					infoFile << "\t pfcVector3D_ptr wZAxis"<< j<<" = uIntfData.CreatePfcVector3D ( "<< ZAxis->get(0) << " , "<< ZAxis->get(1) << " , "<< ZAxis->get(2) << " );"<< endl;
					infoFile << "\t pfcPoint3D_ptr wOrigin"<< j<<" = uIntfData.CreatePfcPoint3D( "<< Origin->get(0) << " , " << Origin->get(1) << " , " << Origin->get(2) << " );\n" << endl;

					infoFile << "\t wfcWPlaneData_ptr WPlaneData"<< j <<" = wfcWPlaneData::Create(wXAxis"<< j <<", wYAxis"<< j <<", wZAxis"<< j <<", wOrigin"<< j <<");" << endl;					
					infoFile << "\t wfcPlaneDatumObject_ptr WPlaneObject" << j << " = wfcPlaneDatumObject::Create(WPlaneData"<< j << ");" << endl;
					infoFile << "\t wfcDatumObject_ptr WDatumObject" << j << " = wfcDatumObject::cast(WPlaneObject"<< j << ");\n" << endl;

					j++;	
					break;
				}
				case wfcDATUM_CSYS:
				{
					wfcCsysDatumObject_ptr CsysObject = wfcCsysDatumObject::cast(DatumObject);
					wfcWCsysData_ptr CsysData = CsysObject-> GetCsysData();
					
					pfcVector3D_ptr XAxis = CsysData->GetXAxis();
					pfcVector3D_ptr YAxis = CsysData->GetYAxis();
					pfcVector3D_ptr ZAxis = CsysData->GetZAxis();
					pfcVector3D_ptr Origin = CsysData->GetOrigin();

					infoFile << "\t pfcVector3D_ptr wXAxis"<< j<<" = uIntfData.CreatePfcVector3D ( "<< XAxis->get(0) << " , "<< XAxis->get(1) << " , "<< XAxis->get(2) << " );"<< endl;					
					infoFile << "\t pfcVector3D_ptr wYAxis"<< j<<" = uIntfData.CreatePfcVector3D ( "<< YAxis->get(0) << " , "<< YAxis->get(1) << " , "<< YAxis->get(2) << " );"<< endl;
					infoFile << "\t pfcVector3D_ptr wZAxis"<< j<<" = uIntfData.CreatePfcVector3D ( "<< ZAxis->get(0) << " , "<< ZAxis->get(1) << " , "<< ZAxis->get(2) << " );"<< endl;
					infoFile << "\t pfcVector3D_ptr wOrigin"<< j<<" = uIntfData.CreatePfcVector3D( "<< Origin->get(0) << " , " << Origin->get(1) << " , " << Origin->get(2) << " );\n" << endl;

					infoFile << "\t wfcWCsysData_ptr WCsysData"<< j <<" = wfcWCsysData::Create(wXAxis"<< j <<", wYAxis"<< j <<", wZAxis"<< j <<", wOrigin"<< j <<");" << endl;					
					infoFile << "\t wfcCsysDatumObject_ptr WCsysObject" << j << " = wfcCsysDatumObject::Create(WCsysData"<< j << ");" << endl;
					infoFile << "\t wfcDatumObject_ptr WDatumObject" << j << " = wfcDatumObject::cast(WCsysObject"<< j << ");\n" << endl;
					
					j++;
					break;
				}
			}								
			infoFile << "\t wfcDatumData_ptr WDatumData" << i+1 << " = wfcDatumData::Create("<< DatumData->GetId() <<" , \"" << DatumData->GetName() << "\" , WDatumObject" << i+1 << ");" << endl;			
			infoFile << "\t wDatumDatas->append(WDatumData"<< i+1 <<");\n"<< endl;
		} // for(int i)
	} else {
		infoFile << "\t wDatumDatas = 0;" << endl;
	}

	// ************************* Accuracy and Outline ************************************ /
	pfcOutline3D_ptr Outline = IntfData->GetOutline();
	pfcPoint3D_ptr Point1 = Outline->get(0);
	pfcPoint3D_ptr Point2 = Outline->get(1);

	infoFile << "\t pfcOutline3D_ptr wOutline = pfcOutline3D::create();" << endl;
	infoFile << "\t pfcPoint3D_ptr wPoint1 = uIntfData.CreatePfcPoint3D( "<< Point1->get(0) << ", " << Point1->get(1) << ", " << Point1->get(2) << " );" << endl;
	infoFile << "\t pfcPoint3D_ptr wPoint2 = uIntfData.CreatePfcPoint3D( "<< Point2->get(0) << ", " << Point2->get(1) << ", " << Point2->get(2) << " );" << endl;
	infoFile << "\t wOutline->set(0, wPoint1);" << endl;
	infoFile << "\t wOutline->set(1, wPoint2);\n" << endl;
	
	infoFile << "\t wfcInterfaceData_ptr InterfaceData = wfcInterfaceData::Create(wSurfDescs, wEdgeDescs, wQuiltDatas, wDatumDatas, " << uEnum.wfcAccuracyTypeGet(IntfData->GetAccuracytype())<< ", " << IntfData->GetAccuracy() << ", wOutline);" << endl;
	infoFile << "\t return(InterfaceData);" << endl;
	infoFile << "\n}" << endl;
}

// ************************************************************************************************* /
void CreateIntfData::CreateWfcSurfaceDescriptor(wfcWSurfaceDescriptors_ptr SurfaceDescs, xstring className, ofstream& infoFile){

	CreateIntfData swIntfData;
	infoFile << " \t wfcWSurfaceDescriptors_ptr wSurfDescs = wfcWSurfaceDescriptors::create();"<< endl;
	for(int i =0; i < SurfaceDescs->getarraysize(); i++)
	{
		wfcWSurfaceDescriptor_ptr SurfDesc = SurfaceDescs->get(i);
		swIntfData.CreateSurfaceDescriptor(SurfDesc, i, "WS", className, infoFile);
		infoFile << "\t wSurfDescs->append(WSSurfDesc"<< i+1 << ");\n" << endl;
	}
	infoFile << "\t return(wSurfDescs);\n}"<< endl;
}

// ************************************************************************************************** /
void CreateIntfData::CreateWfcQuiltDatas(wfcQuiltDatas_ptr Quiltdatas, xstring className, ofstream& infoFile){

	CreateIntfData qwIntfData;
	infoFile << " \t wfcQuiltDatas_ptr wQuiltDatas = wfcQuiltDatas::create();"<< endl;
	for(int i = 0; i < Quiltdatas->getarraysize(); i++)
	{
		wfcQuiltData_ptr QuiltData = Quiltdatas->get(i);
		wfcWSurfaceDescriptors_ptr QSurfDescs = QuiltData->GetSurfaceDescriptors();
		infoFile << "\t wfcWSurfaceDescriptors_ptr WQSurfaceDescs"<< i+1<<" =  wfcWSurfaceDescriptors::create();"<< endl;
		if(QSurfDescs != 0)
		{
			char nstring[100];
			sprintf (nstring, "WQS%d_", i+1);
			for(int j =0; j < QSurfDescs->getarraysize(); j++)
			{
				wfcWSurfaceDescriptor_ptr QSurfDesc = QSurfDescs->get(j);
				qwIntfData.CreateSurfaceDescriptor(QSurfDesc, j, nstring, className, infoFile);
				infoFile << "\t WQSurfaceDescs"<<i+1<<"->append("<<nstring<<"SurfDesc"<< j+1 << ");\n" << endl;
			}
		}
		else
			infoFile << "\t WQSurfaceDescs"<<i+1<<" = 0;"<< endl;

		infoFile << "\t wfcQuiltData_ptr WQuiltData"<<i+1<< " = wfcQuiltData::Create(WQSurfaceDescs"<<i+1<<", "<<QuiltData->GetQuiltId()<<");"<< endl;
		infoFile << "\t wQuiltDatas->append(WQuiltData"<<i+1<<");\n"<< endl;
	}

	infoFile << "\t return(wQuiltDatas);\n}"<< endl;	
}

// ************************************************************************************************** /
void CreateIntfData::CreateWfcEdgeDescriptor(wfcEdgeDescriptors_ptr EdgeDescs, xstring className, ofstream& infoFile)
{
	CreateIntfData ewIntfData;
	infoFile << "\t wfcEdgeDescriptors_ptr wEdgeDescs = wfcEdgeDescriptors::create();"<< endl;
	for(int i = 0; i < EdgeDescs->getarraysize(); i++)
	{
		wfcEdgeDescriptor_ptr EdgeDesc = EdgeDescs->get(i);
		wfcEdgeSurfaceData_ptr EdgeSurfData1 = EdgeDesc->GetEdgeSurface1();
		ewIntfData.CreateEdgeSurfaceData(EdgeSurfData1, 1, i, className, infoFile);

		wfcEdgeSurfaceData_ptr EdgeSurfData2 = EdgeDesc->GetEdgeSurface2();
		ewIntfData.CreateEdgeSurfaceData(EdgeSurfData2, 2, i, className, infoFile);
		
		pfcCurveDescriptor_ptr XYZCurveData = EdgeDesc->GetXYZCurveData();
		if(XYZCurveData != 0)
		{
			infoFile << "\t // Curve Descriptor in Edge Descriptor " << endl;
			ewIntfData.CreateCurveDescriptor(XYZCurveData, i, "WEC", className, infoFile);

			infoFile << "\t wfcEdgeDescriptor_ptr wEdgeDesc"<< i+1 <<" = wfcEdgeDescriptor::Create("<<EdgeDesc->GetId()<<", wEdgeSurfaceData1_"<<i+1<<", wEdgeSurfaceData2_"<<i+1<<", WECCurveDesc"<<i+1<<");"<<endl;		
		} else {
			infoFile << "\t wfcEdgeDescriptor_ptr wEdgeDesc"<< i+1 <<" = wfcEdgeDescriptor::Create("<<EdgeDesc->GetId()<<", wEdgeSurfaceData1_"<<i+1<<", wEdgeSurfaceData2_"<<i+1<<", 0);"<<endl;
		}
		infoFile << "\t wEdgeDescs->append(wEdgeDesc" << i+1 << ");\n" << endl;
	}
	infoFile << "\t return(wEdgeDescs);\n}"<< endl;
}

// ************************************************************************************************** /
void CreateIntfData::CreateEdgeSurfaceData (wfcEdgeSurfaceData_ptr EdgeSurfData, int count, int i, xstring className, ofstream& edgeInfoFile)
{
	CreateIntfData curIntfData;
	otkxTypeEnums nEnum;

	char nstring[100];
	sprintf (nstring, "WED%d", count);

	if(EdgeSurfData->GetUVParamsSequence() != 0)
	{
		pfcUVParamsSequence_ptr UVParamSequence = EdgeSurfData->GetUVParamsSequence();
		edgeInfoFile << "\t pfcUVParamsSequence_ptr wUVParamSequence" << count <<"_"<< i+1 <<" = pfcUVParamsSequence::create();" << endl;

		xint ParamCount = 0;
		xreal param1 = xrealnil;
		xreal param2 = xrealnil;
		for(int j = 0; j < UVParamSequence->getarraysize(); j++)
		{
			 param1 = (UVParamSequence->get(0))->get(0);
			 param2 = (UVParamSequence->get(0))->get(1);

			if((UVParamSequence->get(j))->get(0) == param1 && (UVParamSequence->get(j))->get(1) == param2)
			{
				ParamCount++;
			} else {
				edgeInfoFile << "\t CreatePfcUVParamsSeq("<<(UVParamSequence->get(j))->get(0) <<", " << (UVParamSequence->get(j))->get(1)<<", "<<"wUVParamSequence"<< count <<"_"<< i+1<<");" << endl;
			}
		} // for(int j)
		if(param1 != xrealnil && param2 != xrealnil && ParamCount >=2)
		{
			edgeInfoFile << "\t CreatePfcUVParamsSeqArr(" << ParamCount << ", "<<(UVParamSequence->get(0))->get(0) <<", " << (UVParamSequence->get(0))->get(1)<<", "<<"wUVParamSequence"<< count <<"_"<< i+1<<");" << endl;
		}		
	} else {
		edgeInfoFile << "\t pfcUVParamsSequence_ptr wUVParamSequence"<< count <<"_"<< i+1 << " = 0;"<< endl;
	}
	
	if(EdgeSurfData->GetUVCurveData() != 0)
	{
		pfcCurveDescriptor_ptr CurveDesc = EdgeSurfData->GetUVCurveData();
	    curIntfData.CreateCurveDescriptor(CurveDesc, i, nstring, className, edgeInfoFile);
		
		edgeInfoFile << "\t wfcEdgeSurfaceData_ptr wEdgeSurfaceData" << count << "_"<<i+1<<" = wfcEdgeSurfaceData::Create("<< EdgeSurfData->GetEdgeSurfaceId() << ", "<< nEnum.wfcEdgeDirectionTypeGet(EdgeSurfData->GetDirection()) << ", wUVParamSequence"<< count <<"_"<< i+1 << ", " << nstring<<"CurveDesc"<<i+1<<");\n"<< endl;		
	} else {
		edgeInfoFile << "\t wfcEdgeSurfaceData_ptr wEdgeSurfaceData" << count << "_"<<i+1<<" = wfcEdgeSurfaceData::Create("<< EdgeSurfData->GetEdgeSurfaceId() << ", "<< nEnum.wfcEdgeDirectionTypeGet(EdgeSurfData->GetDirection()) << ", wUVParamSequence"<< count <<"_"<< i+1 << ", 0);\n"<< endl;
	}
}

// ************************************************************************************************** /
void CreateIntfData::CreateCurveDescriptor (pfcCurveDescriptor_ptr CurveDesc, int i, xstring string, xstring className, ofstream& curveInfoFile)
{	
	CreateIntfData cIntfData;
	otkxTypeEnums cEnum;

	pfcCurveType CurveType = CurveDesc->GetCurveType();
	switch(CurveType) {
		case pfcCURVE_COMPOSITE:
		{
			curveInfoFile << "\t // type : pfcCURVE_COMPOSITE"<< endl;	
			pfcCompositeCurveDescriptor_ptr pCompCurveDesc = pfcCompositeCurveDescriptor::cast(CurveDesc);
			wfcWCompositeCurveDescriptor_ptr CompCurveDesc = wfcWCompositeCurveDescriptor::cast(pCompCurveDesc);
		

			pfcCurveDescriptors_ptr CurveDescs = CompCurveDesc->GetElements();

			curveInfoFile << "\t pfcCurveDescriptors_ptr pCurveDescs"<<i+1<<" = pfcCurveDescriptors::create();" << endl;
			for(int j = 0; j < CurveDescs->getarraysize();j++)
			{							
				char cString[100];
				sprintf (cString, "WDComp_%d", i+1);
			
				cIntfData.CreateCompCurveDescriptor (CurveDescs->get(j), j, cString, className, curveInfoFile);
				curveInfoFile << "\t pCurveDescs"<<i+1<<"->append("<< cString<<"CurveDesc"<<j+1<<");\n" << endl;
			}
			
			wfcCurveDirections_ptr CompDirs = CompCurveDesc->GetCompDirections();
			curveInfoFile << "\t wfcCurveDirections_ptr "<<string<<"CompDirs"<<i+1<<" = wfcCurveDirections::create();" << endl;
			for(int k = 0; k < CompDirs->getarraysize(); k++)
			{
				curveInfoFile << "\t "<<string<<"CompDirs"<<i+1<<"->append("<<cEnum.wfcCurveDirectionGet(CompDirs->get(k))<<");" << endl;
			}

			curveInfoFile << "\t wfcWCompositeCurveDescriptor_ptr "<<string<<"wCompCurveDesc"<<i+1<<" = wfcWCompositeCurveDescriptor::Create(pCurveDescs"<<i+1<<");"<<endl;
			curveInfoFile << "\t  "<<string<<"wCompCurveDesc"<<i+1<<"->SetCompDirections("<<string<<"CompDirs"<< i+1<<");" << endl;	

			curveInfoFile << "\t pfcCompositeCurveDescriptor_ptr "<<string<<"CompCurveDesc"<<i+1<<" = pfcCompositeCurveDescriptor::cast("<<string<<"wCompCurveDesc"<<i+1<<");" << endl;
			curveInfoFile << "\t pfcCurveDescriptor_ptr "<<string<<"CurveDesc"<<i+1<<" = pfcCurveDescriptor::cast("<<string<<"CompCurveDesc"<<i+1<<");\n " << endl;
			break;
		}
		case pfcCURVE_POINT:
		{
			curveInfoFile << "\t // type : pfcCURVE_POINT"<< endl;
			pfcPointDescriptor_ptr PointDesc = pfcPointDescriptor::cast(CurveDesc);
			pfcPoint3D_ptr Point = PointDesc->GetPoint ();
			curveInfoFile << "\t pfcPoint3D_ptr "<<string<<"Point"<<i+1<<" = "<<className<<".CreatePfcPoint3D( "<< Point->get(0) << " , " << Point->get(1) << " , " << Point->get(2) << " );" << endl;

			curveInfoFile << "\t pfcPointDescriptor_ptr "<<string<<"PointDesc"<<i+1<<" = pfcPointDescriptor::Create("<<string<<"Point"<<i+1<<");" << endl;
			curveInfoFile << "\t pfcCurveDescriptor_ptr "<<string<<"CurveDesc"<<i+1<<" = pfcCurveDescriptor::cast("<<string<<"PointDesc"<<i+1<<");\n " << endl;
			break;
		}
		case pfcCURVE_LINE:
		{
			curveInfoFile << "\t // type : pfcCURVE_LINE"<< endl;
			pfcLineDescriptor_ptr LineDesc = pfcLineDescriptor::cast(CurveDesc);
			pfcPoint3D_ptr Point1 = LineDesc->GetEnd1 ();
			curveInfoFile << "\t pfcPoint3D_ptr "<<string<<"Point1"<<i+1<<" = "<<className<<".CreatePfcPoint3D( "<< Point1->get(0) << " , " << Point1->get(1) << " , " << Point1->get(2) << " );" << endl;
			pfcPoint3D_ptr Point2 = LineDesc->GetEnd2 ();
			curveInfoFile << "\t pfcPoint3D_ptr "<<string<<"Point2"<<i+1<<" = "<<className<<".CreatePfcPoint3D( "<< Point2->get(0) << " , " << Point2->get(1) << " , " << Point2->get(2) << " );" << endl;

			curveInfoFile << "\t pfcLineDescriptor_ptr "<<string<<"LineDesc"<<i+1<<" = pfcLineDescriptor::Create("<<string<<"Point1"<<i+1<<", "<<string<<"Point2"<<i+1<<");" << endl;
			curveInfoFile << "\t pfcCurveDescriptor_ptr "<<string<<"CurveDesc"<<i+1<<" = pfcCurveDescriptor::cast("<<string<<"LineDesc"<<i+1<<");\n " << endl;
			break;
		}
		case pfcCURVE_ARROW:
		{
			curveInfoFile << "\t // type : pfcCURVE_ARROW"<< endl;
			pfcArrowDescriptor_ptr ArrowDesc = pfcArrowDescriptor::cast(CurveDesc);
			pfcPoint3D_ptr Point1 = ArrowDesc->GetEnd1 ();
			curveInfoFile << "\t pfcPoint3D_ptr "<<string<<"Point1"<<i+1<<" = "<<className<<".CreatePfcPoint3D( "<< Point1->get(0) << " , " << Point1->get(1) << " , " << Point1->get(2) << " );" << endl;
			pfcPoint3D_ptr Point2 = ArrowDesc->GetEnd2 ();
			curveInfoFile << "\t pfcPoint3D_ptr "<<string<<"Point2"<<i+1<<" = "<<className<<".CreatePfcPoint3D( "<< Point2->get(0) << " , " << Point2->get(1) << " , " << Point2->get(2) << " );" << endl;

			curveInfoFile << "\t pfcArrowDescriptor_ptr "<<string<<"ArrowDesc"<<i+1<<" = pfcArrowDescriptor::Create("<<string<<"Point1"<<i+1<<", "<<string<<"Point2"<<i+1<<");" << endl;
			curveInfoFile << "\t pfcCurveDescriptor_ptr "<<string<<"CurveDesc"<<i+1<<" = pfcCurveDescriptor::cast("<<string<<"ArrowDesc"<<i+1<<");\n " << endl;
			break;
		}
		case pfcCURVE_ARC:
		{
			curveInfoFile << "\t // type : pfcCURVE_ARC"<< endl;
			pfcArcDescriptor_ptr ArcDesc = pfcArcDescriptor::cast(CurveDesc);
			pfcVector3D_ptr Vector1 = ArcDesc->GetVector1 ();
			curveInfoFile << "\t pfcVector3D_ptr "<<string<<"ArcVector1"<<i+1<<" = "<<className<<".CreatePfcVector3D( "<< Vector1->get(0) << " , " << Vector1->get(1) << " , " << Vector1->get(2) << " );" << endl;
			pfcVector3D_ptr Vector2 = ArcDesc->GetVector2 ();
			curveInfoFile << "\t pfcVector3D_ptr "<<string<<"ArcVector2"<<i+1<<" = "<<className<<".CreatePfcVector3D( "<< Vector2->get(0) << " , " << Vector2->get(1) << " , " << Vector2->get(2) << " );" << endl;
			pfcPoint3D_ptr Point = ArcDesc->GetCenter ();
			curveInfoFile << "\t pfcPoint3D_ptr "<<string<<"ArcPoint"<<i+1<<" = "<<className<<".CreatePfcPoint3D( "<< Point->get(0) << " , " << Point->get(1) << " , " << Point->get(2) << " );" << endl;

			curveInfoFile << "\t pfcArcDescriptor_ptr "<<string<<"ArcDesc"<<i+1<<" = pfcArcDescriptor::Create("<<string<<"ArcVector1"<<i+1<<", "<<string<<"ArcVector2"<<i+1<<", "<<string<<"ArcPoint"<<i+1<<"," << ArcDesc->GetStartAngle() << ", " << ArcDesc->GetEndAngle() << ", " << ArcDesc->GetRadius()<< " );" << endl;
			curveInfoFile << "\t pfcCurveDescriptor_ptr "<<string<<"CurveDesc"<<i+1<<" = pfcCurveDescriptor::cast("<<string<<"ArcDesc"<<i+1<<");\n " << endl;
			break;
		}
		case pfcCURVE_SPLINE:
		{
			curveInfoFile << "\t // type : pfcCURVE_SPLINE"<< endl;
			pfcSplineDescriptor_ptr SplineDesc = pfcSplineDescriptor::cast(CurveDesc);
						
			pfcSplinePoints_ptr SplPoints = SplineDesc->GetPoints();
			
			cIntfData.CreateSplinePoints(SplPoints, i, string, className, curveInfoFile);

			curveInfoFile << "\t pfcSplineDescriptor_ptr "<<string<<"splineDesc"<<i+1<<" = pfcSplineDescriptor::Create("<<string<<"SplPoints"<<i+1<<");"<< endl;
			curveInfoFile << "\t pfcCurveDescriptor_ptr "<<string<<"CurveDesc"<<i+1<<" = pfcCurveDescriptor::cast("<<string<<"splineDesc"<<i+1<<");\n " << endl;
			break;
		}
		case pfcCURVE_BSPLINE:
		{
			curveInfoFile << "\t // type : pfcCURVE_BSPLINE"<< endl;
			pfcBSplineDescriptor_ptr bSplineDesc = pfcBSplineDescriptor::cast(CurveDesc);
			
			curveInfoFile << "\t pfcBSplinePoints_ptr "<<string<<"BSplPoints"<< i+1 <<" = pfcBSplinePoints::create();" << endl;
			pfcBSplinePoints_ptr bSplPoints = bSplineDesc->GetPoints();
			for(int k = 0; k < bSplPoints->getarraysize(); k++)
			{
				pfcBSplinePoint_ptr bSplPoint = bSplPoints->get(k);

				pfcPoint3D_ptr Point1 = bSplPoint->GetPoint();
				curveInfoFile << "\t pfcPoint3D_ptr "<<string<<"BPoint"<<i+1<<"_"<< k+1<<" = "<<className<<".CreatePfcPoint3D( "<< Point1->get(0) << " , " << Point1->get(1) << " , " << Point1->get(2) << " );" << endl;							
				if(bSplPoint->GetWeight() != xrealnil)
				{
					curveInfoFile << "\t pfcBSplinePoint_ptr "<<string<<"BSplPoint"<<i+1<<"_"<<k+1<<" = pfcBSplinePoint::Create("<<string <<"BPoint"<< i+1 <<"_"<<k+1 <<", "<<bSplPoint->GetWeight()<<");" << endl;		
				} else {
					curveInfoFile << "\t pfcBSplinePoint_ptr "<<string<<"BSplPoint"<<i+1<<"_"<<k+1<<" = pfcBSplinePoint::Create("<< string<<"BPoint"<< i+1 <<"_"<<k+1 <<", xrealnil);" << endl;
				}
				curveInfoFile << "\t "<<string<<"BSplPoints"<<i+1<<"->append("<<string<<"BSplPoint"<<i+1<<"_"<<k+1<<");\n" <<endl;
			}			
			xrealsequence_ptr splKnots = bSplineDesc->GetKnots();
			curveInfoFile << "\t xrealsequence_ptr "<<string<<"BSplKnots"<< i+1 <<" =new  xrealsequence;" << endl;
			for(int l = 0; l < splKnots->getarraysize(); l++)
			{
				curveInfoFile<< "\t "<<string<<"BSplKnots"<< i+1<<"->append("<<splKnots->get(l)<<");" << endl;
			}

			curveInfoFile << "\t pfcBSplineDescriptor_ptr "<<string<<"BSplineDesc"<<i+1<<" = pfcBSplineDescriptor::Create("<<bSplineDesc->GetDegree()<<", "<<string<<"BSplPoints"<<i+1<<", "<<string <<"BSplKnots"<<i+1<<");"<< endl;
			curveInfoFile << "\t pfcCurveDescriptor_ptr "<<string<<"CurveDesc"<<i+1<<" = pfcCurveDescriptor::cast("<<string<<"BSplineDesc"<<i+1<<");\n " << endl;
			break;
		}
		case pfcCURVE_CIRCLE:
		{
			curveInfoFile << "\t // type : pfcCURVE_CIRCLE"<< endl;
			pfcCircleDescriptor_ptr CircleDesc = pfcCircleDescriptor::cast(CurveDesc);
			pfcPoint3D_ptr Point = CircleDesc->GetCenter ();
			curveInfoFile << "\t pfcPoint3D_ptr "<<string<<"Point"<<i+1<<" = "<<className<<".CreatePfcPoint3D( "<< Point->get(0) << " , " << Point->get(1) << " , " << Point->get(2) << " );" << endl;
			pfcVector3D_ptr Vector1 = CircleDesc->GetUnitNormal ();
			curveInfoFile << "\t pfcVector3D_ptr "<<string<<"Vector"<<i+1<<" = "<<className<<".CreatePfcVector3D( "<< Vector1->get(0) << " , " << Vector1->get(1) << " , " << Vector1->get(2) << " );" << endl;

			curveInfoFile << "\t pfcCircleDescriptor_ptr "<<string<<"CircleDesc"<<i+1<<" = pfcCircleDescriptor::Create("<<string<<"Point"<<i+1<<", "<< CircleDesc->GetRadius() << ", "<<string<<"Vector"<<i+1<<");" << endl;
			curveInfoFile << "\t pfcCurveDescriptor_ptr "<<string<<"CurveDesc"<<i+1<<" = pfcCurveDescriptor::cast("<<string<<"CircleDesc"<<i+1<<");\n " << endl;
			break;
		}
		case pfcCURVE_ELLIPSE:
		{
			curveInfoFile << "\t // type : pfcCURVE_ELLIPSE"<< endl;
			pfcEllipseDescriptor_ptr EllipDesc = pfcEllipseDescriptor::cast(CurveDesc);
			pfcVector3D_ptr Vector1 = EllipDesc->GetUnitMajorAxis ();
			curveInfoFile << "\t pfcVector3D_ptr "<<string<<"Vector1"<<i+1<<" = "<<className<<".CreatePfcVector3D( "<< Vector1->get(0) << " , " << Vector1->get(1) << " , " << Vector1->get(2) << " );" << endl;
			pfcVector3D_ptr Vector2 = EllipDesc->GetUnitNormal ();
			curveInfoFile << "\t pfcVector3D_ptr "<<string<<"Vector2"<<i+1<<" = "<<className<<".CreatePfcVector3D( "<< Vector2->get(0) << " , " << Vector2->get(1) << " , " << Vector2->get(2) << " );" << endl;
			pfcPoint3D_ptr Point = EllipDesc->GetCenter ();
			curveInfoFile << "\t pfcPoint3D_ptr "<<string<<"Point"<<i+1<<" = "<<className<<".CreatePfcPoint3D( "<< Point->get(0) << " , " << Point->get(1) << " , " << Point->get(2) << " );" << endl;

			curveInfoFile << "\t pfcEllipseDescriptor_ptr "<<string<<"EllipDesc"<<i+1<<" = pfcEllipseDescriptor::Create("<<string<<"Point"<<i+1<<", "<<string<<"Vector1"<<i+1<<", "<<string<<"Vector2"<<i+1<<", " << EllipDesc->GetMajorLength() << ", " << EllipDesc->GetMinorLength() << ", " << EllipDesc->GetStartAngle()<< ", " << EllipDesc->GetEndAngle()<<" );" << endl;
			curveInfoFile << "\t pfcCurveDescriptor_ptr "<<string<<"CurveDesc"<<i+1<<" = pfcCurveDescriptor::cast("<<string<<"EllipDesc"<<i+1<<");\n " << endl;
			break;
		}
		case pfcCURVE_POLYGON:
		{
			curveInfoFile << "\t // type : pfcCURVE_POLYGON"<< endl;
			pfcPolygonDescriptor_ptr PolygDesc = pfcPolygonDescriptor::cast(CurveDesc);
			pfcPoint3Ds_ptr Points = PolygDesc->GetVertices();
			curveInfoFile << "pfcPoint3Ds_ptr "<<string<<"Points"<<i+1<<" = pfcPoint3Ds::create();" << endl;
			for(int j = 0; j < Points->getarraysize(); j++)
			{
				pfcPoint3D_ptr Point = Points->get(j);
				curveInfoFile << "\t pfcPoint3D_ptr "<<string<<"Point"<< j+1<<"_"<<i+1<<" = "<<className<<".CreatePfcPoint3D( "<< Point->get(0) << " , " << Point->get(1) << " , " << Point->get(2) << " );" << endl;
				curveInfoFile << "\t "<<string<<"Points"<<i+1<<"->append("<<string<<"Point" << j+1 << "_"<<i+1<<");" << endl;
			}
			curveInfoFile << "\n\t pfcPolygonDescriptor_ptr "<<string<<"PolygDesc"<<i+1<<" = pfcPolygonDescriptor::Create("<<string<<"Points"<<i+1<<");" << endl;
			curveInfoFile << "\t pfcCurveDescriptor_ptr "<<string<<"CurveDesc"<<i+1<<" = pfcCurveDescriptor::cast("<<string<<"PolygDesc"<<i+1<<");\n " << endl;
			break;
		}
		case pfcCURVE_TEXT:
		{
			curveInfoFile << "\t // type : pfcCURVE_TEXT"<< endl;
			pfcTextDescriptor_ptr TextDesc = pfcTextDescriptor::cast(CurveDesc);
			pfcPoint3D_ptr Point = TextDesc->GetPoint ();
			curveInfoFile << "\t pfcPoint3D_ptr "<<string<<"Point"<<i+1<<" = "<<className<<".CreatePfcPoint3D( "<< Point->get(0) << " , " << Point->get(1) << " , " << Point->get(2) << " );" << endl	;	
			pfcTextStyle_ptr TextStyle = TextDesc->GetStyle();
			curveInfoFile << "\t pfcTextStyle_ptr "<<string<<"TextStyle"<<i+1<<" =  new pfcTextStyle(" << TextStyle->GetAngle() << ", \" "<< TextStyle->GetFontName()<< " \", "<< TextStyle->GetHeight() << ", " << TextStyle->GetWidthFactor() << ", " << TextStyle->GetSlantAngle() << ", " << TextStyle->GetThickness() << ", " << TextStyle->GetIsUnderlined() << ", " << TextStyle->GetIsMirrored() << ");" << endl;
			curveInfoFile << "\t pfcTextDescriptor_ptr "<<string<<"TextDesc"<<i+1<<" = pfcTextDescriptor::Create(\"_" << TextDesc->GetTextString()<< "\", "<<string<<"Point"<<i+1<<", "<<string<<"TextStyle"<<i+1<<");" << endl;
			curveInfoFile << "\t pfcCurveDescriptor_ptr "<<string<<"CurveDesc"<<i+1<<" = pfcCurveDescriptor::cast("<<string<<"TextDesc"<<i+1<<");\n " << endl;
			break;
		}
	}
}

// ************************************************************************************************** /
void CreateIntfData::CreateCompCurveDescriptor (pfcCurveDescriptor_ptr CurveDesc, int i, xstring string, xstring className, ofstream& ccInfoFile)
{
	CreateIntfData ccIntfData;
	ccIntfData.CreateCurveDescriptor(CurveDesc, i, string, className, ccInfoFile);
}

// ************************************************************************************************** /
void CreateIntfData::CreateSurfaceDescriptor(wfcWSurfaceDescriptor_ptr SurfDesc, int i, xstring string, xstring className, ofstream& surfInfoFile)
{
	CreateIntfData sdIntfData;
	otkxTypeEnums sEnum;

	wfcContourDescriptors_ptr ContDatas = SurfDesc->GetContourData();
	surfInfoFile << "\t wfcContourDescriptors_ptr "<< string <<"ContDescs"<<i+1<<" = wfcContourDescriptors::create();"<< endl;
	if(ContDatas != 0)
	{				
		for(int j = 0; j < ContDatas->getarraysize(); j++)
		{
			surfInfoFile << "\t xintsequence_ptr "<< string <<"EdgeIds"<<i+1<<" = new xintsequence;"<< endl;
			
			wfcContourDescriptor_ptr ContData = ContDatas->get(j);
			xintsequence_ptr EdgeIds = ContData->GetEdgeIds();
			PrintEdgeIdArray(EdgeIds, string, i, surfInfoFile);

			surfInfoFile << "\t wfcContourDescriptor_ptr "<< string <<"ContDesc"<<i+1<<" = wfcContourDescriptor::Create("<< sEnum.wfcContourTraversalGet(ContData->GetContourTraversal()) <<", "<< string <<"EdgeIds"<<i+1<<");"<< endl;
			surfInfoFile << "\t "<< string <<"ContDescs"<<i+1<<"->append("<< string <<"ContDesc"<<i+1<<");\n"<< endl;
		} // for(int j)
	}  else {
		surfInfoFile <<	"\t "<< string <<"ContDescs"<<i+1<<" = 0;" << endl;
	}

	switch(SurfDesc->GetSurfaceType())
	{
		case pfcSURFACE_PLANE:
		{
			surfInfoFile << "\t // pfcSURFACE_PLANE" << endl;
			pfcPlaneDescriptor_ptr Plane = pfcPlaneDescriptor::cast(pfcSurfaceDescriptor::cast(SurfDesc));
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(Plane);
			
			sdIntfData.CreateTransformedSurface(TransSurfDesc, i, className, string, surfInfoFile);
									
			pfcSurfaceExtents_ptr SurfExtents = TransSurfDesc->GetExtents();
			sdIntfData.CreateSurfaceExtents(SurfExtents, i, className, string, surfInfoFile);
			
			surfInfoFile << "\t wfcWPlaneDescriptor_ptr "<< string <<"PlaneDesc"<< i+1 <<" = wfcWPlaneDescriptor::Create("<<string<<"SurfaceExtents"<<i+1<<", "<< sEnum.wfcSurfaceOrientationGet(TransSurfDesc->GetOrientation()) << ", "<<string<<"TransformD"<<i+1<<", " << SurfDesc->GetSurfaceId()<<", "<< string <<"ContDescs"<<i+1<<"); \n" << endl;

			surfInfoFile << "\t wfcWSurfaceDescriptor_ptr "<< string <<"SurfDesc"<< i+1 <<" = wfcWSurfaceDescriptor::cast("<< string <<"PlaneDesc"<< i+1 <<");\n"<< endl;
			break;
		} 
		case pfcSURFACE_CYLINDER:
		{
			surfInfoFile << "\t // pfcSURFACE_CYLINDER" << endl;
			pfcCylinderDescriptor_ptr Cylinder = pfcCylinderDescriptor::cast(pfcSurfaceDescriptor::cast(SurfDesc));
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(Cylinder);
			
			sdIntfData.CreateTransformedSurface(TransSurfDesc, i, className, string, surfInfoFile);
													
			pfcSurfaceExtents_ptr SurfExtents = TransSurfDesc->GetExtents();
			sdIntfData.CreateSurfaceExtents(SurfExtents, i, className, string, surfInfoFile);
			
			surfInfoFile << "\t wfcWCylinderDescriptor_ptr "<< string <<"CylinDesc"<< i+1 <<" = wfcWCylinderDescriptor::Create("<<string<<"SurfaceExtents"<<i+1<<", "<< sEnum.wfcSurfaceOrientationGet(TransSurfDesc->GetOrientation()) << ", "<<string<<"TransformD"<<i+1<<", "<< Cylinder->GetRadius()<<", "<< SurfDesc->GetSurfaceId() <<", "<<string <<"ContDescs"<<i+1<<"); \n" << endl;

			surfInfoFile << "\t wfcWSurfaceDescriptor_ptr "<< string <<"SurfDesc"<< i+1 <<" = wfcWSurfaceDescriptor::cast("<< string <<"CylinDesc"<< i+1 <<");\n"<< endl;
			break;
		}
		case pfcSURFACE_CONE:
		{
			surfInfoFile << "\t // pfcSURFACE_CONE" << endl;
			pfcConeDescriptor_ptr Cone = pfcConeDescriptor::cast(pfcSurfaceDescriptor::cast(SurfDesc));
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(Cone);
			
			sdIntfData.CreateTransformedSurface(TransSurfDesc, i, className, string, surfInfoFile);
													
			pfcSurfaceExtents_ptr SurfExtents = TransSurfDesc->GetExtents();
			sdIntfData.CreateSurfaceExtents(SurfExtents, i, className, string, surfInfoFile);
			
			surfInfoFile << "\t wfcWConeDescriptor_ptr "<< string <<"ConeDesc"<< i+1 <<" = wfcWConeDescriptor::Create("<<string<<"SurfaceExtents"<<i+1<<", "<< sEnum.wfcSurfaceOrientationGet(TransSurfDesc->GetOrientation()) << ", "<<string<<"TransformD"<<i+1<<", "<< Cone->GetAlpha()<<", "<< SurfDesc->GetSurfaceId()<< ", "<<string << "ContDescs"<<i+1<<"); \n" << endl;

			surfInfoFile << "\t wfcWSurfaceDescriptor_ptr "<< string <<"SurfDesc"<< i+1 <<" = wfcWSurfaceDescriptor::cast("<< string <<"ConeDesc"<< i+1 <<");\n"<< endl;
			break;
		}
		case pfcSURFACE_TORUS:
		{
			surfInfoFile << "\t // pfcSURFACE_TORUS" << endl;
			pfcTorusDescriptor_ptr Torus = pfcTorusDescriptor::cast(pfcSurfaceDescriptor::cast(SurfDesc));
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(Torus);
			
			sdIntfData.CreateTransformedSurface(TransSurfDesc, i, className, string, surfInfoFile);
													
			pfcSurfaceExtents_ptr SurfExtents = TransSurfDesc->GetExtents();
			sdIntfData.CreateSurfaceExtents(SurfExtents, i, className, string, surfInfoFile);
			
			surfInfoFile << "\t wfcWTorusDescriptor_ptr "<< string <<"TorusDesc"<< i+1 <<" = wfcWTorusDescriptor::Create("<<string<<"SurfaceExtents"<<i+1<<", "<< sEnum.wfcSurfaceOrientationGet(TransSurfDesc->GetOrientation()) << ", "<<string<<"TransformD"<<i+1<<", "<< Torus->GetRadius1()<<","<<Torus->GetRadius2()<<", "<< SurfDesc->GetSurfaceId()<< ", "<<string << "ContDescs"<<i+1<<"); \n" << endl;
			
			surfInfoFile << "\t wfcWSurfaceDescriptor_ptr "<< string <<"SurfDesc"<< i+1 <<" = wfcWSurfaceDescriptor::cast("<< string <<"TorusDesc"<< i+1 <<");\n"<< endl;
			break;
		}
		case pfcSURFACE_RULED:
		{
			surfInfoFile << "\t // pfcSURFACE_RULED" << endl;
			pfcRuledSurfaceDescriptor_ptr RuledSurf = pfcRuledSurfaceDescriptor::cast(pfcSurfaceDescriptor::cast(SurfDesc));
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(RuledSurf);
			
			sdIntfData.CreateTransformedSurface(TransSurfDesc, i, className, string, surfInfoFile);
													
			pfcSurfaceExtents_ptr SurfExtents = TransSurfDesc->GetExtents();
			sdIntfData.CreateSurfaceExtents(SurfExtents, i, className, string, surfInfoFile);
			
			pfcCurveDescriptor_ptr CurveDesc1 = RuledSurf->GetProfile1();
			sdIntfData.CreateCurveDescriptor (CurveDesc1, i, "SD1", className, surfInfoFile);

			pfcCurveDescriptor_ptr CurveDesc2 = RuledSurf->GetProfile2();
			sdIntfData.CreateCurveDescriptor (CurveDesc2, i, "SD2", className, surfInfoFile);

			surfInfoFile << "\t wfcWRuledSurfaceDescriptor_ptr "<< string <<"RuledSurfDesc"<< i+1 <<" = wfcWRuledSurfaceDescriptor::Create("<<string<<"SurfaceExtents"<<i+1<<", "<< sEnum.wfcSurfaceOrientationGet(TransSurfDesc->GetOrientation()) << ", "<<string<<"TransformD"<<i+1<<", SD1CurvDesc"<<i+1<<", SD2CurvDesc"<<i+1<<", "<< SurfDesc->GetSurfaceId()<< ", "<<string << "ContDescs"<<i+1<<"); \n" << endl;

			surfInfoFile << "\t wfcWSurfaceDescriptor_ptr "<< string <<"SurfDesc"<< i+1 <<" = wfcWSurfaceDescriptor::cast("<< string <<"RuledSurfDesc"<< i+1 <<");\n"<< endl;
			break;
		}
		case pfcSURFACE_REVOLVED:
		{
			surfInfoFile << "\t // pfcSURFACE_REVOLVED" << endl;
			pfcRevolvedSurfaceDescriptor_ptr RevSurf = pfcRevolvedSurfaceDescriptor::cast(pfcSurfaceDescriptor::cast(SurfDesc));
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(RevSurf);
			
			sdIntfData.CreateTransformedSurface(TransSurfDesc, i, className, string, surfInfoFile);
													
			pfcSurfaceExtents_ptr SurfExtents = TransSurfDesc->GetExtents();
			sdIntfData.CreateSurfaceExtents(SurfExtents, i, className, string, surfInfoFile);
			
			pfcCurveDescriptor_ptr CurveDesc = RevSurf->GetProfile();
			sdIntfData.CreateCurveDescriptor (CurveDesc, i, "SDR", className, surfInfoFile);

			surfInfoFile << "\t wfcWRevolvedSurfaceDescriptor_ptr "<< string <<"RevSurfDesc"<< i+1 <<" = wfcWRevolvedSurfaceDescriptor::Create("<<string<<"SurfaceExtents"<<i+1<<", "<< sEnum.wfcSurfaceOrientationGet(TransSurfDesc->GetOrientation()) << ", "<<string<<"TransformD"<<i+1<<", SDRCurveDesc"<<i+1<<", "<< SurfDesc->GetSurfaceId()<< ", "<<string << "ContDescs"<<i+1<<"); \n" << endl;
			
			surfInfoFile << "\t wfcWSurfaceDescriptor_ptr "<< string <<"SurfDesc"<< i+1 <<" = wfcWSurfaceDescriptor::cast("<< string <<"RevSurfDesc"<< i+1 <<");\n"<< endl;
			break;
		}
		case pfcSURFACE_TABULATED_CYLINDER:
		{
			surfInfoFile << "\t // pfcSURFACE_TABULATED_CYLINDER" << endl;
			pfcTabulatedCylinderDescriptor_ptr TabCylider = pfcTabulatedCylinderDescriptor::cast(pfcSurfaceDescriptor::cast(SurfDesc));
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(TabCylider);
			
			sdIntfData.CreateTransformedSurface(TransSurfDesc, i, className, string, surfInfoFile);
													
			pfcSurfaceExtents_ptr SurfExtents = TransSurfDesc->GetExtents();
			sdIntfData.CreateSurfaceExtents(SurfExtents, i, className, string, surfInfoFile);
			
			pfcCurveDescriptor_ptr CurveDesc = TabCylider->GetProfile();
			sdIntfData.CreateCurveDescriptor (CurveDesc, i, "SD", className, surfInfoFile);

			surfInfoFile << "\t wfcWTabulatedCylinderDescriptor_ptr "<< string <<"TabCyliderDesc"<< i+1 <<" = wfcWTabulatedCylinderDescriptor::Create("<<string<<"SurfaceExtents"<<i+1<<", "<< sEnum.wfcSurfaceOrientationGet(TransSurfDesc->GetOrientation()) << ", "<<string<<"TransformD"<<i+1<<", SDCurveDesc"<<i+1<<", "<< SurfDesc->GetSurfaceId()<< ", "<<string << "ContDescs"<<i+1<<"); \n" << endl;
			
			surfInfoFile << "\t wfcWSurfaceDescriptor_ptr "<< string <<"SurfDesc"<< i+1 <<" = wfcWSurfaceDescriptor::cast("<< string <<"TabCyliderDesc"<< i+1 <<");\n"<< endl;
			break;
		}		
		case pfcSURFACE_SPLINE:
		{
			surfInfoFile << "\t // pfcSURFACE_SPLINE" << endl;
			pfcSplineSurfaceDescriptor_ptr SplineSurf = pfcSplineSurfaceDescriptor::cast(pfcSurfaceDescriptor::cast(SurfDesc));
			wfcWSplineSurfaceDescriptor_ptr WSplineSurf = wfcWSplineSurfaceDescriptor::cast(SplineSurf);
			
			sdIntfData.CreatePfcSplineSurfaceDescriptor(SplineSurf, i, string, className, surfInfoFile);

			surfInfoFile << "\t wfcWSplineSurfaceDescriptor_ptr "<< string <<"WSurfDesc"<< i+1 <<" = wfcWSplineSurfaceDescriptor::cast("<< string <<"SplineSurfDesc"<< i+1<<");" << endl;
			surfInfoFile << "\t "<< string <<"WSurfDesc"<< i+1 <<"->SetNumberOfUParams("<<WSplineSurf->GetNumberOfUParams()<<");" << endl;
			surfInfoFile << "\t "<< string <<"WSurfDesc"<< i+1 <<"->SetNumberOfVParams("<<WSplineSurf->GetNumberOfVParams()<<");" << endl;

			surfInfoFile << "\t "<< string <<"WSurfDesc"<< i+1 <<"->SetContourData("<<string << "ContDescs"<<i+1<<");" << endl;
			surfInfoFile << "\t "<< string <<"WSurfDesc"<< i+1 <<"->SetSurfaceId("<<WSplineSurf->GetSurfaceId()<<");" << endl;
			
			surfInfoFile << "\t wfcWSurfaceDescriptor_ptr "<< string <<"SurfDesc"<< i+1 <<" = wfcWSurfaceDescriptor::cast("<< string <<"WSurfDesc"<< i+1 <<");\n"<< endl;
			break;
		}
		case pfcSURFACE_NURBS:
		{
			surfInfoFile << "\t // pfcSURFACE_NURBS" << endl;
			pfcNURBSSurfaceDescriptor_ptr Nurbs = pfcNURBSSurfaceDescriptor::cast(pfcSurfaceDescriptor::cast(SurfDesc));
						
			pfcSurfaceExtents_ptr SurfExtents = SurfDesc->GetExtents();
			sdIntfData.CreateSurfaceExtents(SurfExtents, i, className, string, surfInfoFile);			

			xrealsequence_ptr UKnots = Nurbs->GetUKnots();
			surfInfoFile << "\t xrealsequence_ptr wUKnots"<<i+1 << " = new xrealsequence;"<< endl;
			for(int j = 0; j < UKnots->getarraysize(); j++)
			{
				surfInfoFile << "\t wUKnots"<<i+1<<"->append("<< UKnots->get(j)<<"); " << endl;
			}

			xrealsequence_ptr Vknots = Nurbs->GetVKnots();
			surfInfoFile << "\t xrealsequence_ptr wVKnots"<<i+1 << " = new  xrealsequence;"<< endl;
			for(int j = 0; j < Vknots->getarraysize(); j++)
			{
				surfInfoFile << "\t wVKnots"<<i+1<<"->append("<< Vknots->get(j)<<"); " << endl;
			}

			pfcBSplinePoints_ptr BSplines = Nurbs->GetPoints();
			surfInfoFile << "\t pfcBSplinePoints_ptr wBSplines"<<i+1 <<" = pfcBSplinePoints::create();"<< endl;
			for(int k = 0; k < BSplines->getarraysize(); k++)
			{
				pfcBSplinePoint_ptr BSpline = BSplines->get(k);
				
				pfcPoint3D_ptr Point = BSpline->GetPoint();
				surfInfoFile << "\t pfcPoint3D_ptr "<<string<<"Point"<<i+1<<"_"<<k+1<<" = "<<className<<".CreatePfcPoint3D( "<< Point->get(0) << " , " << Point->get(1) << " , " << Point->get(2) << ");" << endl;

				if(BSpline->GetWeight() != xrealnil)
					surfInfoFile << "\t CreatePfcBSplinePoints("<<string << "Point"<<i+1<<"_"<<k+1<<", "<< BSpline->GetWeight() <<", wBSplines"<<i+1 <<");" << endl;
				else
					surfInfoFile << "\t CreatePfcBSplinePoints("<<string << "Point"<<i+1<<"_"<<k+1<<", 0.00, wBSplines"<<i+1 <<");" << endl;
			}
			surfInfoFile << "\t wfcWNURBSSurfaceDescriptor_ptr "<< string <<"NurbsDesc"<< i+1 <<" = wfcWNURBSSurfaceDescriptor::Create("<<string<<"SurfaceExtents"<<i+1<<", "<< sEnum.wfcSurfaceOrientationGet(SurfDesc->GetOrientation()) <<", "<< Nurbs->GetUDegree() <<"," << Nurbs->GetVDegree()<<", wUKnots"<< i+1 <<", wVKnots"<< i+1 <<", wBSplines"<< i+1<<", "<< SurfDesc->GetSurfaceId()<< ", "<<string << "ContDescs"<<i+1<<"); \n" << endl;
			
			surfInfoFile << "\t wfcWSurfaceDescriptor_ptr "<< string <<"SurfDesc"<< i+1 <<" = wfcWSurfaceDescriptor::cast("<< string <<"NurbsDesc"<< i+1 <<");\n"<< endl;
			break;
		}
		case pfcSURFACE_CYLINDRICAL_SPLINE:
		{
			surfInfoFile << "\t // pfcSURFACE_CYLINDRICAL_SPLINE" << endl;
			pfcCylindricalSplineSurfaceDescriptor_ptr CylinSpline = pfcCylindricalSplineSurfaceDescriptor::cast(pfcSurfaceDescriptor::cast(SurfDesc));
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(CylinSpline);
			
			sdIntfData.CreateTransformedSurface(TransSurfDesc, i, className, string, surfInfoFile);
													
			pfcSurfaceExtents_ptr SurfExtents = TransSurfDesc->GetExtents();
			sdIntfData.CreateSurfaceExtents(SurfExtents, i, className, string, surfInfoFile);

			pfcSplineSurfaceDescriptor_ptr SplineSurf = CylinSpline->GetSplineSurfaceData();
			sdIntfData.CreatePfcSplineSurfaceDescriptor(SplineSurf, i, string, className, surfInfoFile);
			
			surfInfoFile << "\t wfcWCylindricalSplineSurfaceDescriptor_ptr "<< string <<"CylinSplineSurfDesc"<< i+1 <<" = wfcWCylindricalSplineSurfaceDescriptor::Create("<<string<<"SurfaceExtents"<<i+1<<", "<< sEnum.wfcSurfaceOrientationGet(SurfDesc->GetOrientation())<<", " << string<<"TransformD"<< i+1<<", "<< string <<"SplineSurfDesc"<< i+1<<", "<< SurfDesc->GetSurfaceId()<< ", "<<string << "ContDescs"<<i+1<<"); \n" << endl;
			 
			surfInfoFile << "\t wfcWSurfaceDescriptor_ptr "<< string <<"SurfDesc"<< i+1 <<" = wfcWSurfaceDescriptor::cast("<< string <<"CylinSplineSurfDesc"<< i+1 <<");\n"<< endl;		
			break;
		}
		case pfcSURFACE_SPHERICAL_SPLINE:
		{
			surfInfoFile << "\t // pfcSURFACE_SPHERICAL_SPLINE" << endl;
			pfcSphericalSplineSurfaceDescriptor_ptr SphSpline = pfcSphericalSplineSurfaceDescriptor::cast(pfcSurfaceDescriptor::cast(SurfDesc));
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(SphSpline);
			
			sdIntfData.CreateTransformedSurface(TransSurfDesc, i, className, string, surfInfoFile);
													
			pfcSurfaceExtents_ptr SurfExtents = TransSurfDesc->GetExtents();
			sdIntfData.CreateSurfaceExtents(SurfExtents, i, className, string, surfInfoFile);

			pfcSplineSurfaceDescriptor_ptr SplineSurf = SphSpline->GetSplineSurfaceData();
			sdIntfData.CreatePfcSplineSurfaceDescriptor(SplineSurf, i, string, className,surfInfoFile);
			
			surfInfoFile << "\t pfcSphericalSplineSurfaceDescriptor_ptr "<< string <<"SphSplineSurfDesc"<< i+1 <<" = pfcSphericalSplineSurfaceDescriptor::Create("<<string<<"SurfaceExtents"<<i+1<<", "<< sEnum.wfcSurfaceOrientationGet(SurfDesc->GetOrientation())<<", " << string<<"TransformD"<< i+1<<", "<< string <<"SplineSurfDesc"<< i+1<<"); \n" << endl;

			surfInfoFile << "\t pfcSurfaceDescriptor_ptr p"<< string <<"SurfDesc"<< i+1 <<" = pfcSurfaceDescriptor::cast("<< string <<"SphSplineSurfDesc"<< i+1<<");" << endl;
			surfInfoFile << "\t wfcWSurfaceDescriptor_ptr "<< string <<"SurfDesc"<< i+1 <<" = wfcWSurfaceDescriptor::cast("<< string <<"SurfDesc"<< i+1 <<");\n"<< endl;
			break;
		}
		case pfcSURFACE_FOREIGN:
		{
			surfInfoFile << "\t // pfcSURFACE_FOREIGN" << endl;
			pfcForeignSurfaceDescriptor_ptr ForeignSurf = pfcForeignSurfaceDescriptor::cast(pfcSurfaceDescriptor::cast(SurfDesc));
			pfcTransformedSurfaceDescriptor_ptr TransSurfDesc = pfcTransformedSurfaceDescriptor::cast(ForeignSurf);
			
			sdIntfData.CreateTransformedSurface(TransSurfDesc, i, className, string, surfInfoFile);
													
			pfcSurfaceExtents_ptr SurfExtents = TransSurfDesc->GetExtents();
			sdIntfData.CreateSurfaceExtents(SurfExtents, i, className, string, surfInfoFile);

			surfInfoFile << "\t wfcWForeignSurfaceDescriptor_ptr "<< string <<"ForeignSurfDesc"<< i+1 <<" = wfcWForeignSurfaceDescriptor::Create("<<string<<"SurfaceExtents"<<i+1<<", "<< sEnum.wfcSurfaceOrientationGet(SurfDesc->GetOrientation())<<", " << string<<"TransformD"<< i+1<<", "<<ForeignSurf->GetForeignID()<<", "<< SurfDesc->GetSurfaceId()<< ", "<<string << "ContDescs"<<i+1<<"); \n" << endl;

			surfInfoFile << "\t wfcWSurfaceDescriptor_ptr "<< string <<"SurfDesc"<< i+1 <<" = wfcWSurfaceDescriptor::cast("<< string <<"ForeignSurfDesc"<< i+1 <<");\n"<< endl;
			break;
		}
	} // switch()
}

// ************************************************************************************************* /
void CreateIntfData::CreatePfcSplineSurfaceDescriptor(pfcSplineSurfaceDescriptor_ptr SplineSurf, int i, xstring string, xstring className, ofstream& splSurfInfoFile)
{
	CreateIntfData splIntfData;
	otkxTypeEnums sEnum;

	pfcSurfaceExtents_ptr SurfExtents = SplineSurf->GetExtents();
	splIntfData.CreateSurfaceExtents(SurfExtents, i, className, string, splSurfInfoFile);
			
	pfcSplineSurfacePoints_ptr Points = SplineSurf->GetPoints();
	splSurfInfoFile << "\t pfcSplineSurfacePoints_ptr "<<string<<"Points"<< i+1 <<" =  pfcSplineSurfacePoints::create();" << endl;
	for(int j = 0; j < Points->getarraysize(); j++)
	 {
		 pfcSplineSurfacePoint_ptr Point = Points->get(j);
		 pfcUVParams_ptr UVParam = Point->GetParameter();

		 splSurfInfoFile << "\t pfcUVParams_ptr "<< string <<"UVParam"<< i+1 <<"_"<< j+1 <<" = "<< className<<".CreatePfcUVParams(" << UVParam->get(0) << ", " << UVParam->get(1) << ");" << endl;
		 
		 pfcPoint3D_ptr	PointD = Point->GetPoint();
		 splSurfInfoFile << "\t pfcPoint3D_ptr "<<string<<"PointD"<<i+1<<"_"<<j+1<<" = "<< className<<".CreatePfcPoint3D( "<<PointD->get(0)<< " , " <<PointD->get(1)<< " , " <<PointD->get(2)<< ");"<< endl;
		 pfcVector3D_ptr UTangent = Point->GetUTangent();
		 if(UTangent != 0)
			splSurfInfoFile << "\t pfcVector3D_ptr "<<string << "UTangent"<< i+1 <<"_"<<j+1<<" = "<< className<<".CreatePfcVector3D ( "<< UTangent->get(0) << " , "<< UTangent->get(1) << " , "<< UTangent->get(2) << " );"<< endl;
		 else
			splSurfInfoFile << "\t pfcVector3D_ptr "<<string << "UTangent"<< i+1 <<"_"<<j+1<<" = 0;"<<endl;

		 pfcVector3D_ptr VTangent = Point->GetVTangent();
		 if(VTangent != 0)
			splSurfInfoFile << "\t pfcVector3D_ptr "<<string << "VTangent"<< i+1 <<"_"<<j+1<<" = "<< className<<".CreatePfcVector3D ( "<< VTangent->get(0) << " , "<< VTangent->get(1) << " , "<< VTangent->get(2) << " );"<< endl;
		 else
			splSurfInfoFile << "\t pfcVector3D_ptr "<<string << "VTangent"<< i+1 <<"_"<<j+1<<" = 0;"<<endl;

		 pfcVector3D_ptr UVDeriv = Point->GetUVDerivative();
		 if(UVDeriv != 0)
			splSurfInfoFile << "\t pfcVector3D_ptr "<<string << "UVDeriv"<< i+1 <<"_"<<j+1<<" = "<< className<<".CreatePfcVector3D ( "<< UVDeriv->get(0) << " , "<< UVDeriv->get(1) << " , "<< UVDeriv->get(2) << " );"<< endl;
		 else
			splSurfInfoFile << "\t pfcVector3D_ptr "<<string << "UVDeriv"<< i+1 <<"_"<<j+1<<" = 0;"<<endl;

		 splSurfInfoFile<<"\t CreatePfcSplineSurfacePoint("<< string <<"UVParam"<< i+1 <<"_"<< j+1 <<", "<<string<<"PointD"<<i+1<<"_"<<j+1<<", "<<string << "UTangent"<< i+1 <<"_"<<j+1<<", "<<string << "VTangent"<< i+1 <<"_"<<j+1<<", "<<string << "UVDeriv"<< i+1 <<"_"<<j+1<<", "<<string<<"Points"<< i+1 <<");\n"<< endl;			 
	 }

	splSurfInfoFile << "\t pfcSplineSurfaceDescriptor_ptr "<< string <<"SplineSurfDesc"<< i+1 <<" = pfcSplineSurfaceDescriptor::Create("<<string<<"SurfaceExtents"<<i+1<<", "<< sEnum.wfcSurfaceOrientationGet(SplineSurf->GetOrientation())<<", " << string<<"Points"<< i+1<<"); \n" << endl;
}

// ************************************************************************************************* /
void CreateIntfData::CreateSplinePoints (pfcSplinePoints_ptr SplPoints, int i, xstring string, xstring className, ofstream& splineInfoFile)
{
	splineInfoFile << "\t pfcSplinePoints_ptr "<<string<<"SplPoints"<<i+1<<" =  pfcSplinePoints::create();" << endl;
	for(int j = 0; j < SplPoints->getarraysize(); j++)
	{
		pfcSplinePoint_ptr Spline = SplPoints->get(j);

		pfcPoint3D_ptr Point = Spline->GetPoint();
		splineInfoFile << "\t pfcPoint3D_ptr "<< string <<"Point" << i+1 <<"_"<<j+1 <<" = "<<className<<".CreatePfcPoint3D( "<< Point->get(0) << " , " << Point->get(1) << " , " << Point->get(2) << ");" << endl;
		
		pfcVector3D_ptr Vector = Spline->GetTangent();
		splineInfoFile << "\t pfcVector3D_ptr "<< string <<"Vector" << i+1 <<"_"<< j+1 <<" = "<< className <<".CreatePfcVector3D( "<< Vector->get(0) << " , " << Vector->get(1) << " , " << Vector->get(2) << ");" << endl;

		splineInfoFile << "\t CreatePfcSplinePoints((xreal)"<<Spline->GetParameter()<<", "<<string <<"Point" << i+1 <<"_"<<j+1 <<", "<< string <<"Vector" << i+1 <<"_"<<j+1 << ", "<< string<<"SplPoints"<<i+1<<");" << endl;		
	}		
}

// ************************************************************************************************* /
void CreateIntfData::CreateTransformedSurface(pfcTransformedSurfaceDescriptor_ptr TransSurfDesc, int i, xstring className, xstring string, ofstream& transInfoFile)
{
	pfcTransform3D_ptr Trans3D = TransSurfDesc->GetCoordSys ();
	pfcVector3D_ptr VecX = Trans3D->GetXAxis ();
	pfcVector3D_ptr VecY = Trans3D->GetYAxis ();
	pfcVector3D_ptr VecZ = Trans3D->GetZAxis ();
	pfcPoint3D_ptr  Origin = Trans3D->GetOrigin ();
	pfcMatrix3D_ptr Matrix = Trans3D->GetMatrix();

	transInfoFile << "\t pfcVector3D_ptr "<<string<<"XAxis"<<i+1<<" = "<< className <<".CreatePfcVector3D ( "<< VecX->get(0) << " , "<< VecX->get(1) << " , "<< VecX->get(2) << " );"<< endl;
	transInfoFile << "\t pfcVector3D_ptr "<<string<<"YAxis"<<i+1<<" = "<< className <<".CreatePfcVector3D ( "<< VecY->get(0) << " , "<< VecY->get(1) << " , "<< VecY->get(2) << " );"<< endl;
	transInfoFile << "\t pfcVector3D_ptr "<<string<<"ZAxis"<<i+1<<" = "<< className <<".CreatePfcVector3D ( "<< VecZ->get(0) << " , "<< VecZ->get(1) << " , "<< VecZ->get(2) << " );"<< endl;
	transInfoFile << "\t pfcPoint3D_ptr "<<string<<"origin"<<i+1<<" = "<< className <<".CreatePfcPoint3D( "<< Origin->get(0) << " , " << Origin->get(1) << " , " << Origin->get(2) << " );" << endl;
		
	transInfoFile << "\t pfcMatrix3D_ptr "<<string<<"MatrixD"<<i+1<<" = "<< className <<".CreatePfcMatrix3D("<<
						Matrix->get(0,0) <<","<<Matrix->get(0,1) <<","<<Matrix->get(0,2) <<","<<Matrix->get(0,3) <<","<<
						Matrix->get(1,0) <<","<<Matrix->get(1,1) <<","<<Matrix->get(1,2) <<","<<Matrix->get(1,3) <<","<<
						Matrix->get(2,0) <<","<<Matrix->get(2,1) <<","<<Matrix->get(2,2) <<","<<Matrix->get(2,3) <<","<<
						Matrix->get(3,0) <<","<<Matrix->get(3,1) <<","<<Matrix->get(3,2) <<","<<Matrix->get(3,3) <<"); "<< endl;
	
	transInfoFile << "\t pfcTransform3D_ptr  "<<string<<"TransformD"<<i+1<<" = pfcTransform3D::Create("<<string<<"MatrixD"<< i+1 <<");\n" << endl;	
	transInfoFile << "\t CreatePfcTransform3D("<<string<<"XAxis"<<i+1<<", "<<string<<"YAxis"<<i+1<<","<<string<<"ZAxis"<<i+1<<", "<<string<<"origin"<<i+1<<", "<<string<<"TransformD"<<i+1<<");\n"<< endl;
}

// ************************************************************************************************* /
void CreateIntfData::CreateSurfaceExtents(pfcSurfaceExtents_ptr SurfExtents, int i, xstring className, xstring string,  ofstream& surfExtInfoFile)
{
	pfcUVOutline_ptr outline = SurfExtents->GetUVExtents();
	pfcUVParams_ptr param1 = outline->get(0);
	pfcUVParams_ptr param2 = outline->get(1);

	surfExtInfoFile << "\t pfcUVParams_ptr "<<string<<"UVParam1"<<i+1<<" = "<< className<<".CreatePfcUVParams(" << param1->get(0) << ", " << param1->get(1) << ");" << endl;
	surfExtInfoFile << "\t pfcUVParams_ptr "<<string<<"UVParam2"<<i+1<<" = "<< className<<".CreatePfcUVParams(" << param2->get(0) << ", " << param2->get(1) << ");" << endl;

	surfExtInfoFile << "\t pfcUVOutline_ptr "<<string<<"UVOutline"<<i+1<<" = "<< className<<".CreatePfcUVOutline("<<string<<"UVParam1"<<i+1<<", "<<string<<"UVParam2"<<i+1<<");" << endl;
	surfExtInfoFile << "\t pfcSurfaceExtents_ptr "<<string<<"SurfaceExtents"<<i+1<<" = pfcSurfaceExtents::Create ("<<string<<"UVOutline"<<i+1<<");" << endl;
}

// ************************************************************************************************* /
pfcMatrix3D_ptr CreateIntfData::CreatePfcMatrix3D(xreal a1, xreal a2, xreal a3, xreal a4,
									  xreal b1, xreal b2, xreal b3, xreal b4,
									  xreal c1, xreal c2, xreal c3, xreal c4,
									  xreal d1, xreal d2, xreal d3, xreal d4)
{
		pfcMatrix3D_ptr Ret = new pfcMatrix3D();
		Ret->set(0,0,a1);
		Ret->set(0,1,a2);
		Ret->set(0,2,a3);
		Ret->set(0,3,a4);
		Ret->set(1,0,b1);
		Ret->set(1,1,b2);
		Ret->set(1,2,b3);
		Ret->set(1,3,b4);
		Ret->set(2,0,c1);
		Ret->set(2,1,c2);
		Ret->set(2,2,c3);
		Ret->set(2,3,c4);
		Ret->set(3,0,d1);
		Ret->set(3,1,d2);
		Ret->set(3,2,d3);
		Ret->set(3,3,d4);

		return(Ret);
}

// ************************************************************************************************* /
pfcUVOutline_ptr CreateIntfData::CreatePfcUVOutline(pfcUVParams_ptr param1, pfcUVParams_ptr param2)
{
	pfcUVOutline_ptr Ret = new pfcUVOutline;
	Ret->set(0, param1);
	Ret->set(1, param2);
	
	return(Ret);
}

// ************************************************************************************************* /
pfcUVParams_ptr CreateIntfData::CreatePfcUVParams(xreal param1, xreal param2)
{
	pfcUVParams_ptr Ret = pfcUVParams::create();

	Ret->set(0, param1);
	Ret->set(1, param2);

	return(Ret);
}

// ************************************************************************************************** /
 pfcPoint3D_ptr CreateIntfData::CreatePfcPoint3D (xreal X, xreal Y, xreal Z)
 {
 	pfcPoint3D_ptr		Ret =  new pfcPoint3D;
 
 	Ret -> set (0, X);
 	Ret -> set (1, Y);
 	Ret -> set (2, Z);

 	return (Ret);
 }

 // ************************************************************************************************** /
pfcVector3D_ptr CreateIntfData::CreatePfcVector3D (xreal X, xreal Y, xreal Z)
 {
 	pfcVector3D_ptr		Ret =  new pfcVector3D;
 
 	Ret -> set (0, X);
 	Ret -> set (1, Y);
 	Ret -> set (2, Z);

 	return (Ret);
 }

// ************************************************************************************************* /
char* otkxTypeEnums::wfcCurveDirectionGet (int Id)
{
	switch(Id)
	{
		case wfcCURVE_NO_FLIP:			return((char*)"wfcCURVE_NO_FLIP");
		case wfcCURVE_FLIP:				return((char*)"wfcCURVE_FLIP");		
		case wfcCurveDirection_nil:		return((char*)"wfcCurveDirection_nil");		
		
		default: return (char*) "*** Unknown enum in method wfcCurveDirectionGet";
	}
}

// ************************************************************************************************* /
char* otkxTypeEnums::wfcAccuracyTypeGet (int Id)
{
	switch(Id)
	{
		case wfcACCU_NONE:			return((char*)"wfcACCU_NONE");
		case wfcACCU_RELATIVE:		return((char*)"wfcACCU_RELATIVE");
		case wfcACCU_ABSOLUTE:		return((char*)"wfcACCU_ABSOLUTE");
		case wfcAccuracytype_nil:	return((char*)"wfcAccuracytype_nil");		
		
		default: return (char*) "*** Unknown enum in method wfcAccuracyTypeGet";
	}
}

// ************************************************************************************************* /
char* otkxTypeEnums::wfcEdgeDirectionTypeGet (int Id)
{
	switch(Id)
	{
		case wfcEDGE_NO_FLIP:			return((char*)"wfcEDGE_NO_FLIP");
		case wfcEDGE_FLIP:				return((char*)"wfcEDGE_FLIP");	
		case wfcEdgeDirection_nil:		return((char*)"wfcEdgeDirection_nil");	
		
		default: return (char*) "*** Unknown enum in method wfcEdgeDirectionTypeGet";
	}
}

// ************************************************************************************************* /
char* otkxTypeEnums::wfcContourTraversalGet (int Id)
{
	switch(Id)
	{
		case pfcCONTOUR_TRAV_NONE:			return((char*)"pfcCONTOUR_TRAV_NONE");
		case pfcCONTOUR_TRAV_INTERNAL:		return((char*)"pfcCONTOUR_TRAV_INTERNAL");	
		case pfcCONTOUR_TRAV_EXTERNAL:		return((char*)"pfcCONTOUR_TRAV_EXTERNAL");	
		case pfcContourTraversal_nil:		return((char*)"pfcContourTraversal_nil");	
		
		default: return (char*) "*** Unknown enum in method wfcContourTraversalGet";
	}
}

// ************************************************************************************************* /
char* otkxTypeEnums::wfcSurfaceOrientationGet (int Id)
{
	switch(Id)
	{
		case pfcSURFACEORIENT_NONE:			return((char*)"pfcSURFACEORIENT_NONE");
		case pfcSURFACEORIENT_OUTWARD:		return((char*)"pfcSURFACEORIENT_OUTWARD");	
		case pfcSURFACEORIENT_INWARD:		return((char*)"pfcSURFACEORIENT_INWARD");	
		case pfcSurfaceOrientation_nil:		return((char*)"pfcSurfaceOrientation_nil");	
		
		default: return (char*) "*** Unknown enum in method wfcSurfaceOrientationGet";
	}
}
// ************************************************************************************************* /
void CreatePfcBSplinePoints(pfcPoint3D_ptr Point, xreal weight, pfcBSplinePoints_ptr BPoints)
{
	pfcBSplinePoint_ptr BPoint = pfcBSplinePoint::Create(Point, weight);

	BPoints->append(BPoint);
}
void CreatePfcSplinePoints(xreal param, pfcPoint3D_ptr Point, pfcVector3D_ptr Tangent, pfcSplinePoints_ptr Points)
{
	pfcSplinePoint_ptr splPoint = pfcSplinePoint::Create(param, Point, Tangent);

	Points->append(splPoint);
}
void CreatePfcSplineSurfacePoint(pfcUVParams_ptr UVParam, pfcPoint3D_ptr PointD, pfcVector3D_ptr UTangent, pfcVector3D_ptr VTangent, pfcVector3D_ptr UVDeriv, pfcSplineSurfacePoints_ptr splPoint)
{
	pfcSplineSurfacePoint_ptr Point = pfcSplineSurfacePoint::Create(UVParam, PointD, UTangent, VTangent, UVDeriv);

	splPoint->append(Point);
}
void CreatePfcTransform3D(pfcVector3D_ptr XAxis, pfcVector3D_ptr YAxis, pfcVector3D_ptr ZAxis, pfcPoint3D_ptr Origin, pfcTransform3D_ptr Trasform3D)
{
	Trasform3D->SetXAxis(XAxis);
	Trasform3D->SetYAxis(YAxis);
	Trasform3D->SetZAxis(ZAxis);
	Trasform3D->SetOrigin(Origin);
}
void CreatePfcUVParamsSeq(xreal param1, xreal param2, pfcUVParamsSequence_ptr UVParamSeq)
{
	pfcUVParams_ptr Ret = pfcUVParams::create();

	Ret->set(0, param1);
	Ret->set(1, param2);

	UVParamSeq->append(Ret);
}
pfcUVParams_ptr CreatePfcUVParams(xreal param1, xreal param2)
{
	pfcUVParams_ptr Ret = pfcUVParams::create();

	Ret->set(0, param1);
	Ret->set(1, param2);

	return(Ret);
}
pfcUVOutline_ptr CreatePfcUVOutline(pfcUVParams_ptr param1, pfcUVParams_ptr param2)
{
	pfcUVOutline_ptr Ret = new pfcUVOutline;
	Ret->set(0, param1);
	Ret->set(1, param2);
	
	return(Ret);
}

// ******************************************************************************************** /
void PrintEdgeIdArray(xintsequence_ptr EdgeIds, xstring string, int i, ofstream& sinfoFile)
{
	char buff[200];
	char buff1[100];
	xstring line;
	char numstr[10];

	line += "\t CreateEdgeIdArray(";
	line += string;
	line += "EdgeIds";
        
	sprintf(numstr, "%d", i+1);
        line += numstr;

	int EdgeSize = EdgeIds->getarraysize();

	sprintf(buff1, ", %d ", EdgeSize);
	 line += buff1;

	for(int j = 0; j < EdgeSize; j++)
	{
		sprintf(buff, ", %d ", EdgeIds->get(j));
		line += buff;
	}
	line += ");";
	sinfoFile << line << endl;
}

// ******************************************************************************************** /
void CreateEdgeIdArray(xintsequence_ptr EdgeId, int size, ...)
{
	va_list args;
	va_start(args, size);
	int max;

	for(int k = 0; k < size; k++)
	{
		max = va_arg(args, int);
		EdgeId->append(max);
	}
	va_end(args);
}

// ******************************************************************************************** /
void CreatePfcUVParamsSeqArr(xint count, xreal param1, xreal param2, pfcUVParamsSequence_ptr UVParamSeq)
{
	for(int a = 0; a < count; a++)
	{
		CreatePfcUVParamsSeq(param1, param2, UVParamSeq);
	}
}
// ************************************************************************************************* /
