/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/



#include <pfcSession.h>
#include <pfcModel.h>

#include <ciplib.h>

#include <pfcGlobal.h>
#include <pfcFeature.h>
#include <wfcElementTree.h>
#include <wfcCollection.h>
#include <wfcElemIds.h>
#include <wfcFeature.h>
#include <wfcSolid.h>
#include <pfcSelect.h>
#include <wfcSession.h>
#include <pfcArgument.h>
#include <wfcFeatureInstructions.h>
#include <OTKXSaveFeatCxx.h>
#include <wfcGlobal.h>
#include <OTKXEnum.h>
#include <OTKXUtils.h>


#include <fstream>

ofstream fp_out;

void WriteCxxTemplate (pfcModel_ptr Model, ofstream &exception_info);
void CreateCxxTemplate (wfcElementTree_ptr ElemTree, pfcModel_ptr Model, int featId,  ofstream& fp_out);


/* Select a feature to save as cxx */
extern "C" wfcStatus otkSaveFeatAsCxx ()
{
	ofstream exception_info;
	exception_info.open("exception_info.inf",ios::out);
	try
	{		
		pfcSession_ptr  Session = pfcGetCurrentSession ();

		wfcWSession_ptr wSession = wfcWSession::cast (Session);
		
		pfcModel_ptr Model = Session->GetCurrentModel();
		
		if (Model != NULL)
		{

		pfcModelType modelType;
			
		modelType = Model->GetType();

		if (modelType != pfcMDL_ASSEMBLY && modelType != pfcMDL_PART)
			return wfcTK_BAD_INPUTS;
	
		pfcSelectionOptions_ptr SelOpts = pfcSelectionOptions::Create ("feature");
		SelOpts->SetMaxNumSels(1);
		
		pfcSelections_ptr Sels = Session->Select (SelOpts, NULL);

		if (Sels != NULL)
		{	
			pfcModelItem_ptr Modelitem = Sels->get (0)->GetSelItem();
			pfcFeature_ptr Feat =  pfcFeature::cast (Modelitem);

			wfcWFeature_ptr wFeat = wfcWFeature::cast (Feat);

			wfcElementTree_ptr ElemTree = wFeat->GetElementTree (NULL,wfcFEAT_EXTRACT_NO_OPTS);					
					
			int featId = (int)Feat->GetId();
			char buffer [50];

			xstring name = Model->GetInstanceName();
			xstring Lname = name.ToLower();

			/* Saving the feature as cxx file in the form:
			modelName_feat_<feat ID>.cxx */
			sprintf (buffer, "%s_feat_%d.cxx", const_cast<char*>((cStringT)Lname), featId);

			fp_out.open(buffer, ios::out);				
			
			CreateCxxTemplate (ElemTree, Model, featId, fp_out);
			fp_out.close();	
			return wfcTK_NO_ERROR;
		}
	}
	else
	{ 
		return wfcTK_BAD_INPUTS;
	}

	return wfcTK_NO_ERROR;
	}
	OTK_EXCEPTION_HANDLER(exception_info);
			
	return wfcTK_GENERAL_ERROR;
}

/* Saves all features as cxx */
extern "C" wfcStatus otkSaveAllFeatsAsCxx ()
{
	ofstream exception_info;
	exception_info.open("exception_info.inf",ios::out);

	try
	{		
		pfcSession_ptr		Session = pfcGetCurrentSession ();

		wfcWSession_ptr wSession = wfcWSession::cast (Session);
		
		pfcModel_ptr Model = Session->GetCurrentModel();
		
		if (Model != NULL)
		{
			WriteCxxTemplate (Model, exception_info);
		}

		return wfcTK_NO_ERROR;
		
	}
	OTK_EXCEPTION_HANDLER(exception_info);
			
	return wfcTK_GENERAL_ERROR;
}


void WriteCxxTemplate (pfcModel_ptr Model, ofstream &exception_info)
{
	try
	{

	pfcModelItems_ptr ModelItems = Model->ListItems();                  
    xint NumItems = ModelItems->getarraysize ();

	 for (xint ItemIdx = 0; ItemIdx < NumItems; ItemIdx ++)
	 {
		pfcModelItem_ptr	Item = ModelItems->get (ItemIdx);

		if ((pfcModelItem *) Item == 0)
				continue;

		if ( pfcFeature::isObjKindOf (Item) )
		{
			pfcFeature_ptr	Feat = pfcFeature::cast (Item);
			wfcWFeature_ptr wFeat = wfcWFeature::cast (Feat);

			wfcElementTree_ptr ElemTree = wFeat->GetElementTree (NULL,wfcFEAT_EXTRACT_NO_OPTS);					
				
			int featId = (int)Feat->GetId();
			char buffer [50];
			
			xstring name = Model->GetInstanceName();
			xstring Lname = name.ToLower();

			/* Saving the feature as cxx file in the form:
			modelName_feat_<feat ID>.cxx */
			sprintf (buffer, "%s_feat_%d.cxx", const_cast<char*>((cStringT)Lname), featId);
			fp_out.open(buffer, ios::out);				
			
			CreateCxxTemplate (ElemTree, Model, featId, fp_out);
			fp_out.close();				

		}
	 }

	
	}
	OTK_EXCEPTION_HANDLER(exception_info);
}


void CreateCxxTemplate (wfcElementTree_ptr ElemTree, pfcModel_ptr Model, int featId, ofstream& fp_out)
{
	
	wfcElements_ptr newElems = new wfcElements();
	wfcElements_ptr Elems = ElemTree->ListTreeElements();
	xstring ElemIdString;
	char* CurveCollectionInstrAttr;
	char* CurveCollectionInstrType;
	char* SurfaceCollectionInstrType;
	char* SurfaceCollectionRefType;
	otkxEnums localEnum;
	int ElementsSize = Elems->getarraysize();

	fp_out << "wfcElements_ptr CreateFeature_"<<featId<<"(pfcModel_ptr Model)" <<endl;
	fp_out << "{" <<endl;
	fp_out << "  wfcElements_ptr newElems = new wfcElements();		" <<endl;
	fp_out << "  wfcElement_ptr newElem;		"<<endl;
	fp_out << "  CreateElem localElem;		"<<endl;
	fp_out << "  otkxEnums localEnum;		"<<endl;
	fp_out << "  CollectionHelpers localCollnHelper; \n" << endl;
	CollectionHelpers localCollnHelper;
	wfcCrvCollectionInstrAttributes_ptr w = localCollnHelper.CreateCurveCollInstrAttrs_One (1);
		  
	

	for (int i=0;i<ElementsSize;i++)
	{
		wfcElement_ptr Elem = Elems->get(i);
		int elemId = Elem->GetId();
		int level = Elem->GetLevel();
    ElemIdString = Elem->GetIdAsString();
		
		pfcArgValue_ptr elemArgVal = Elem->GetValue();
		pfcArgValue_ptr newElemArgVal = new pfcArgValue();
	//	ElemIdString = localEnum.wfcElemIdGet (elemId);
		fp_out << "  /*	  Element id:	"<< elemId<< " : "<<ElemIdString<<"	*/ "<<endl;		

		if (elemArgVal != 0 || elemId == wfcPRO_E_STD_CURVE_COLLECTION_APPL || elemId == wfcPRO_E_STD_SURF_COLLECTION_APPL)
		{		
			pfcArgValueType argValueType;

			if (elemId != wfcPRO_E_STD_CURVE_COLLECTION_APPL && elemId != wfcPRO_E_STD_SURF_COLLECTION_APPL)
			{
				argValueType = elemArgVal->Getdiscr();
			}
			else
			{
				argValueType = pfcARG_V_SELECTION;
			}

			switch (argValueType)
			{
				case (pfcARG_V_INTEGER):
				{
					int iVal = elemArgVal->GetIntValue();
					newElemArgVal->SetIntValue(iVal);
					wfcElement_ptr newElem = wfcElement::Create (elemId, newElemArgVal, level);					
					
					fp_out << "  localElem.CreateIntegerElem ("<< ElemIdString<< ", "<< iVal<<", "<< level<<", newElems); \n" << endl;
					
					break;
				}
				case (pfcARG_V_DOUBLE):
				{
					double dVal = elemArgVal->GetDoubleValue();
					newElemArgVal->SetDoubleValue(dVal);
					wfcElement_ptr newElem = wfcElement::Create (elemId, newElemArgVal, level);

					fp_out << "  localElem.CreateDoubleElem ("<< ElemIdString<< ", "<< dVal<<", "<< level<<", newElems) ; \n" << endl;
					
					break;
				}
				case (pfcARG_V_BOOLEAN):
				{
					bool bVal = elemArgVal->GetBoolValue();
					newElemArgVal->SetBoolValue(bVal);
					wfcElement_ptr newElem = wfcElement::Create (elemId, newElemArgVal, level);

					fp_out << "  localElem.CreateBooleanElem ("<< ElemIdString<< ", "<< bVal<<", "<< level<<",newElems); \n" << endl;
										
					break;
				}
				case (pfcARG_V_SELECTION):
				{
					if (elemId == wfcPRO_E_STD_CURVE_COLLECTION_APPL)
					{						
						wfcCollection_ptr WCollection = Elem->GetElemCollection();
						if (WCollection != 0)
						{
							wfcCurveCollection_ptr WCurveCollection = WCollection->GetCrvCollection();
							if (WCurveCollection != 0)
							{

								wfcCurveCollectionInstructions_ptr WCrvCollInstrs = WCurveCollection->GetInstructions();
								
								int	numCrvCollInstrs = WCrvCollInstrs->getarraysize();
								
								wfcCurveCollectionInstruction_ptr WCrvCollInstr_one = WCrvCollInstrs->get(0);
								wfcCrvCollectionInstrAttributes_ptr WCrvCollInstrAttributes_one = WCrvCollInstr_one->GetAttributes();	
								int numRefs, numCrvCollInstrAttrs;
								if (WCrvCollInstrAttributes_one != 0)
								{
									numCrvCollInstrAttrs = WCrvCollInstrAttributes_one->getarraysize();
								}
								else
								{
									numCrvCollInstrAttrs = 0;
								}

								
								pfcSelections_ptr Sels_one = WCrvCollInstr_one->GetReferences();
								if (Sels_one !=0)
								{
									numRefs = Sels_one->getarraysize();
								}
								else
								{
									numRefs = 0;
								}

								if (numCrvCollInstrs == 1 && (numRefs == 1 || numRefs == 0) && (numCrvCollInstrAttrs == 0 || numCrvCollInstrAttrs == 1))
								{
									wfcCurveCollectionInstrType Type = WCrvCollInstr_one->GetType();
									CurveCollectionInstrType = localEnum.wfcCurveCollectionInstrTypeGet(Type);

									double Value = WCrvCollInstr_one->GetValue();

									if (numCrvCollInstrAttrs == 1)
									{
										wfcCrvCollectionInstrAttribute_ptr WCrvCollInstrAttribute = WCrvCollInstrAttributes_one->get(0);
										wfcCurveCollectionInstrAttribute Attribute = WCrvCollInstrAttribute->GetAttribute();
										CurveCollectionInstrAttr = localEnum.wfcCurveCollectionInstrAttributeGet(Attribute);
										if (numRefs ==1)
										{
											pfcSelection_ptr Sel1 = Sels_one->get(0);
											pfcModelItem_ptr modelItem1 = Sel1->GetSelItem();
											if (modelItem1 != 0)
											{
												pfcModelItemType itemType1 = modelItem1->GetType();
												int itemId1 = modelItem1->GetId();

												fp_out<<"  localElem.CreateCurveCollectionElem ("<<ElemIdString<<", "<<level<<", "<<CurveCollectionInstrType<<", "<<Value<<", Model, " << itemType1<<", "<< itemId1<<", "<< CurveCollectionInstrAttr <<", newElems); \n"<<endl;
											}

										}
										else
										{
											fp_out<<"  localElem.CreateCurveCollectionElem ("<<ElemIdString<<", "<<level<<", "<<CurveCollectionInstrType<<", "<<Value<<", "<< CurveCollectionInstrAttr <<", newElems); \n"<<endl;

										}	

									}
									else
									{
										if (numRefs ==1)
										{
											pfcSelection_ptr Sel1 = Sels_one->get(0);
											pfcModelItem_ptr modelItem1 = Sel1->GetSelItem();
											if (modelItem1 != 0)
											{
												pfcModelItemType itemType1 = modelItem1->GetType();
												int itemId1 = modelItem1->GetId();

												fp_out<<"  localElem.CreateCurveCollectionElem ("<<ElemIdString<<", "<<level<<", "<<CurveCollectionInstrType<<", "<<Value<<", Model,"
																			 << itemType1<<", "<< itemId1<<", newElems); \n"<<endl;
											}

										}
										else
										{
											fp_out<<"  localElem.CreateCurveCollectionElem ("<<ElemIdString<<", "<<level<<", "<<CurveCollectionInstrType<<", "<<Value<<", newElems); \n"<<endl;

										}

									}// else loop for numCrvCollInstrAttrs == 1

								}
								else
								{								
									fp_out << "  wfcCurveCollectionInstructions_ptr WCrvCollInstrs_"<<i<<" = wfcCurveCollectionInstructions::create(); " << endl;
									for (int k=0;k<numCrvCollInstrs;k++)
									{
										wfcCurveCollectionInstruction_ptr WCrvCollInstr = WCrvCollInstrs->get(k);
										wfcCurveCollectionInstrType Type = WCrvCollInstr->GetType();
										CurveCollectionInstrType = localEnum.wfcCurveCollectionInstrTypeGet(Type);
																		
										double Value = WCrvCollInstr->GetValue();
										wfcCrvCollectionInstrAttributes_ptr WCrvCollInstrAttributes = WCrvCollInstr->GetAttributes();																
										if (WCrvCollInstrAttributes != 0)
										{
											int numCrvCollnstrAttrs = WCrvCollInstrAttributes->getarraysize();
											int *Attr = new int [numCrvCollnstrAttrs];
											wfcCrvCollectionInstrAttribute_ptr WCrvCollInstrAttribute;
											wfcCurveCollectionInstrAttribute Attribute;
										
											if (numCrvCollnstrAttrs == 1)
											{
												WCrvCollInstrAttribute = WCrvCollInstrAttributes->get(0);
												Attribute = WCrvCollInstrAttribute->GetAttribute();
												CurveCollectionInstrAttr = localEnum.wfcCurveCollectionInstrAttributeGet(Attribute);
												
												fp_out<<"  wfcCrvCollectionInstrAttributes_ptr WCrvCollInstrAttributes_"<<i<<"_"<<k<<"= localCollnHelper.CreateCurveCollInstrAttrs_One("<<CurveCollectionInstrAttr<<");"<<endl;																																	
											}
											else
											{
												fp_out<< "  int Attr_"<<i<<"_"<<k<<"["<<numCrvCollnstrAttrs<<"];"<<endl;
												for (int m=0;m<numCrvCollnstrAttrs;m++)
												{
													
													WCrvCollInstrAttribute = WCrvCollInstrAttributes->get(m);
													Attribute = WCrvCollInstrAttribute->GetAttribute();	
													CurveCollectionInstrAttr = localEnum.wfcCurveCollectionInstrAttributeGet(Attribute);
													
													Attr[m] = int(Attribute);											
													fp_out<<"  Attr_"<<i<<"_"<<k<<"["<<m<<"] = "<<CurveCollectionInstrAttr<<";"<<endl;
												}
												fp_out<<"  wfcCrvCollectionInstrAttributes_ptr WCrvCollInstrAttributes_"<<i<<"_"<<k<<"= localCollnHelper.CreateCurveCollInstrAttrs(Attr_"<<i<<"_"<<k<<", "<<numCrvCollnstrAttrs<<");"<<endl;										
											}
	
										}

										pfcSelections_ptr Sels = WCrvCollInstr->GetReferences();
										if (Sels != 0)
										{							
											int numSels = Sels->getarraysize();
											if (numSels == 1)
											{
												pfcSelection_ptr Sel1 = Sels->get(0);
												pfcModelItem_ptr modelItem1 = Sel1->GetSelItem();
												if (modelItem1 != 0)
												{
													pfcModelItemType itemType1 = modelItem1->GetType();
													int itemId1 = modelItem1->GetId();
													fp_out <<"  pfcSelections_ptr Sels_"<<i<<"_"<<k<<" = localCollnHelper.CreateSelections_One (Model,"<< itemType1<<","<< itemId1<<");"<<endl;		
												}

											}
											else
											{
												for (int j = 0;j<numSels;j++)
												{
													fp_out <<"  pfcSelections_ptr Sels_"<<i<<"_"<<k<<" = pfcSelections::create();"<<endl;
													pfcSelection_ptr Sel = Sels->get(j);
													pfcModelItem_ptr modelItem = Sel->GetSelItem();
													if (modelItem != 0)
													{
														pfcModelItemType itemType = modelItem->GetType();
														int itemId = modelItem->GetId();	
														
														fp_out <<"  localCollnHelper.CreateSelections (Model,"<< itemType<<","<< itemId<<", Sels_"<<i<<"_"<<k<<");"<<endl;		
													}

												}//loop over selections
											}
											if (WCrvCollInstrAttributes != 0)
											{
												fp_out << "  wfcCurveCollectionInstruction_ptr WCrvCollInstr_"<<i<<"_"<<k<<" = localCollnHelper.CreateCurveInstruction("
												<<CurveCollectionInstrType<<", "<<Value<<", Sels_"<<i<<"_"<<k<<", WCrvCollInstrAttributes_"<<i<<"_"<<k<<");"<<endl;
											}
											else
											{
												fp_out << "  wfcCurveCollectionInstruction_ptr WCrvCollInstr_"<<i<<"_"<<k<<" = localCollnHelper.CreateCurveInstruction("
												<<CurveCollectionInstrType<<", "<<Value<<", Sels_"<<i<<"_"<<k<<", 0);"<<endl;
											}									

										}//if Sels != 0
										else
										{
											if (WCrvCollInstrAttributes != 0)
											{
												fp_out << "  wfcCurveCollectionInstruction_ptr WCrvCollInstr_"<<i<<"_"<<k<<" = localCollnHelper.CreateCurveInstruction("
												<<CurveCollectionInstrType<<", "<<Value<<", 0, WCrvCollInstrAttributes_"<<i<<"_"<<k<<");"<<endl;
											}
											else
											{
												fp_out << "  wfcCurveCollectionInstruction_ptr WCrvCollInstr_"<<i<<"_"<<k<<" = localCollnHelper.CreateCurveInstruction("
												<<CurveCollectionInstrType<<", "<<Value<<", 0, 0);"<<endl;
											}

										}								
								
										fp_out <<"  localCollnHelper.CreateCurveCollInstrs (WCrvCollInstr_"<<i<<"_"<<k<<", WCrvCollInstrs_"<<i<<"); "<<endl;

									}//loop over curve collection instructions
								
							
									fp_out <<"  localElem.CreateCurveCollectionElem ("<<ElemIdString<<", "<<level<<",  WCrvCollInstrs_"<<i<<", newElems); \n"<<endl;
								}
				
							}//if curve collection != 0
							else
							{
								fp_out << "  localElem.CreateCurveCollectionElem("<<ElemIdString<<", "<<level<<",  newElems); \n"<<endl;
							}
						}// if WCollection != 0
					}// Loop over CURVE_COLLECTION_APPL ends
							

					if (elemId == wfcPRO_E_STD_SURF_COLLECTION_APPL)
					{
						fp_out << "  /* *** SURFACE COLLECTION *** */ " <<endl;
						wfcCollection_ptr WCollection = Elem->GetElemCollection();						

						if (WCollection != 0)
						{
							wfcSurfaceCollection_ptr WSurfaceCollection = WCollection->GetSurfCollection();
					
							if (WSurfaceCollection != 0)
							{
								wfcSurfaceCollectionInstructions_ptr WSurfaceCollInstrs = WSurfaceCollection->GetInstructions();								
								
								int numSrfCollInstrs = WSurfaceCollInstrs->getarraysize();
								wfcSurfaceCollectionInstruction_ptr WSurfaceCollInstr_one = WSurfaceCollInstrs->get(0);
								wfcSurfaceCollectionReferences_ptr SrfCollRefs_one = WSurfaceCollInstr_one->GetSrfCollectionReferences();
								int numSrfCollRefs = SrfCollRefs_one->getarraysize();

								if (numSrfCollInstrs == 1 && numSrfCollRefs == 1)
								{
									wfcSurfaceCollectionInstrType Type = WSurfaceCollInstr_one->GetType();	
									SurfaceCollectionInstrType = localEnum.wfcSurfaceCollectionInstrTypeGet(Type);

									bool Include = WSurfaceCollInstr_one->GetInclude();	

									wfcSurfaceCollectionReference_ptr SrfCollRef_one = SrfCollRefs_one->get(0);
									wfcSurfaceCollectionRefType SrfCollRefType = SrfCollRef_one->GetRefType();
									SurfaceCollectionRefType = localEnum.wfcSurfaceCollectionRefTypeGet(SrfCollRefType);
									pfcSelection_ptr Ref_One = SrfCollRef_one->GetReference();										
										
									pfcModelItem_ptr modelItem = Ref_One->GetSelItem();
									if (modelItem != 0)
									{
										pfcModelItemType itemType = modelItem->GetType();
										int itemId = modelItem->GetId();	
										fp_out << "  localElem.CreateSurfaceCollectionElem("<<ElemIdString<<", "<<level<<", "<<SurfaceCollectionRefType<<", Model, "<< itemType<<", "<< itemId<<", "<<SurfaceCollectionInstrType<<", "<<Include<<", newElems ); \n"<<endl;
																											
									}

								}
								else
								{					

									fp_out << "  wfcSurfaceCollectionInstructions_ptr WSurfaceCollInstrs_"<<i<<" = wfcSurfaceCollectionInstructions::create(); " << endl;
									int numSrfCollInstrs = WSurfaceCollInstrs->getarraysize();
									for (int x=0;x<numSrfCollInstrs;x++)
									{									
										wfcSurfaceCollectionInstruction_ptr WSurfaceCollInstr = WSurfaceCollInstrs->get(x);
										wfcSurfaceCollectionInstrType Type = WSurfaceCollInstr->GetType();	
										SurfaceCollectionInstrType = localEnum.wfcSurfaceCollectionInstrTypeGet(Type);

										bool Include = WSurfaceCollInstr->GetInclude();																

										wfcSurfaceCollectionReferences_ptr SrfCollRefs = WSurfaceCollInstr->GetSrfCollectionReferences();
										fp_out << "  wfcSurfaceCollectionReferences_ptr WSrfCollRefs_"<<i<<"_"<<x<<" = wfcSurfaceCollectionReferences::create();"<<endl;
										int numSrfCollRefs = SrfCollRefs->getarraysize();
										for (int y = 0;y<numSrfCollRefs;y++)
										{
											wfcSurfaceCollectionReference_ptr SrfCollRef = SrfCollRefs->get(y);
											wfcSurfaceCollectionRefType SrfCollRefType = SrfCollRef->GetRefType();
											SurfaceCollectionRefType = localEnum.wfcSurfaceCollectionRefTypeGet(SrfCollRefType);
											pfcSelection_ptr Ref = SrfCollRef->GetReference();										
										
											pfcModelItem_ptr modelItem = Ref->GetSelItem();
											if (modelItem != 0)
											{
												pfcModelItemType itemType = modelItem->GetType();
												int itemId = modelItem->GetId();	
																		
												fp_out <<"  wfcSurfaceCollectionReference_ptr WSrfCollRef_"<<i<<"_"<<x<<"_"<<y<<" = localCollnHelper.CreateSurfCollReference ("<<SurfaceCollectionRefType<<", Model, "<< itemType<<", "<< itemId<<");"<<endl;											
											}
											fp_out <<"  WSrfCollRefs_"<<i<<"_"<<x<<"->append(WSrfCollRef_"<<i<<"_"<<x<<"_"<<y<<");"<<endl;
										}								
										fp_out <<"  wfcSurfaceCollectionInstruction_ptr WSurfaceCollInstr_"<<i<<"_"<<x<<" = localCollnHelper.CreateSurfCollInstr ("<<SurfaceCollectionInstrType<<", "<<Include<<", WSrfCollRefs_"<<i<<"_"<<x<<");"<<endl;

										fp_out <<"  WSurfaceCollInstrs_"<<i<<"->append(WSurfaceCollInstr_"<<i<<"_"<<x<<");"<<endl;

									}// Loops over surf. coll. instructions

									fp_out <<"  localElem.CreateSurfaceCollectionElem("<<ElemIdString<<", "<<level<<",  WSurfaceCollInstrs_"<<i<<", newElems); \n"<<endl;

								}
							}//if surface collection != 0
							else 
							{
								fp_out << "  localElem.CreateSurfaceCollectionElem("<<ElemIdString<<", "<<level<<",  newElems); \n"<<endl;
							}
							
						}// if WCollection != 0
					
					}// Loop over SURF_COLLECTION_APPL ends

					if (elemId != wfcPRO_E_STD_SURF_COLLECTION_APPL && elemId != wfcPRO_E_STD_CURVE_COLLECTION_APPL)
					{						
					
						pfcSelection_ptr selVal = elemArgVal->GetSelectionValue();
						pfcModelItem_ptr modelItem = selVal->GetSelItem();
						if (modelItem != 0)
						{
							pfcModelItemType itemType = modelItem->GetType();
							int itemId = modelItem->GetId();							
							fp_out << "  localElem.CreateSelectionElem ("<< ElemIdString<< ", "<< level<<", Model, "<<itemType<<", "<<itemId<<",  newElems ); \n " <<endl;

						}

					}		

					break;
				}
				case (pfcARG_V_TRANSFORM):
				{				
					fp_out << "  /* *** TRANSFORM *** */ " <<endl;
					pfcTransform3D_ptr Transform = elemArgVal->GetTransformValue();
					pfcMatrix3D_ptr Matrix3D = Transform->GetMatrix();
					fp_out<<"  double Row1[4] = {"<<Matrix3D->get(0,0)<<", "<<Matrix3D->get(0,1)<<", "<<Matrix3D->get(0,2)<<", "<<Matrix3D->get(0,3)<<"}; \n";
					fp_out<<"  double Row2[4] = {"<<Matrix3D->get(1,0)<<", "<<Matrix3D->get(1,1)<<", "<<Matrix3D->get(1,2)<<", "<<Matrix3D->get(1,3)<<"}; \n";
					fp_out<<"  double Row3[4] = {"<<Matrix3D->get(2,0)<<", "<<Matrix3D->get(2,1)<<", "<<Matrix3D->get(2,2)<<", "<<Matrix3D->get(2,3)<<"}; \n";
					fp_out<<"  double Row4[4] = {"<<Matrix3D->get(3,0)<<", "<<Matrix3D->get(3,1)<<", "<<Matrix3D->get(3,2)<<", "<<Matrix3D->get(3,3)<<"}; \n";
					fp_out<<"  localElem.CreateTransformElem ("<<ElemIdString<<", "<<level<<",  newElems, Row1, Row2, Row3, Row4); \n";

					break;
				}
				
				case (pfcARG_V_ASCII_STRING):
				{
					xstring asciiVal = elemArgVal->GetASCIIStringValue();
					newElemArgVal->SetASCIIStringValue(asciiVal);
					wfcElement_ptr newElem = wfcElement::Create (elemId, newElemArgVal, level);
					
					fp_out << "  localElem.CreateASCIIStringElem ("<<ElemIdString<<", \""<<asciiVal<<"\", "<< level<<", newElems ); \n" << endl;
					
					break;
				}
				case (pfcARG_V_STRING):
				case (pfcARG_V_WSTRING):
				{
					xstring sVal = elemArgVal->GetStringValue();
					newElemArgVal->SetStringValue(sVal);
					wfcElement_ptr newElem = wfcElement::Create (elemId, newElemArgVal, level);
					
					fp_out << "  localElem.CreateStringElem ("<<ElemIdString<<", \""<<sVal<<"\", "<< level<<", newElems ); \n" << endl;
					
					break;
				}
				case (pfcARG_V_POINTER):
				{

				}

			}			

		}
		else
		{
			if (elemId != wfcPRO_E_COMPONENT_MODEL)
			{
				fp_out << "  localElem.CreateCompoundElem ("<<ElemIdString<<", "<< level<<", newElems ); \n" << endl;			
			}
			else
			{
				wfcSpecialValue_ptr WSpecialValue = Elem->GetSpecialValueElem();
				pfcSolid_ptr WComponentModel = WSpecialValue->GetComponentModel();
				xstring compModelName = WComponentModel->GetFileName();
				pfcModelType modelType = WComponentModel->GetType();

				fp_out <<"  localElem.CreateComponentModelElem("
					<<ElemIdString<<", "<< level<<", "<<i<<", newElems, \""<<compModelName<<"\",  "<<modelType<<"); \n"<<endl;

			}
		}


	}// End of for loop over elements


	
	fp_out << "\n  return (newElems);" << endl;
	fp_out << "\n}" << endl;
	
}

