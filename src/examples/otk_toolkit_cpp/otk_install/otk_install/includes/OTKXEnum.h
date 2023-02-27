/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

#ifndef OTKXENUM_h
#define OTKXENUM_h




class otkxEnums
{
public:
	/* Returns enum wfcCurveCollectionInstrAttribute for given ID */
	char* wfcCurveCollectionInstrAttributeGet (int Id);
	
	/* Returns enum wfcCurveCollectionInstrType for given ID */
	char* wfcCurveCollectionInstrTypeGet (int Id);
	
	/* Returns enum wfcSurfaceCollectionRefType for given ID */
	char* wfcSurfaceCollectionRefTypeGet (int Id);
	
	/* Returns enum wfcSurfaceCollectionInstrType for given ID */
	char* wfcSurfaceCollectionInstrTypeGet (int Id);

	/* Returns enum SimpRepActionType for given ID */
	char* pfcSimpRepActionTypeGet (int id);

	/* Returns enum ExplodedAnimationMoveType for given ID */
	char* wfcExplodedAnimationMoveTypeGet (int id);

};

#endif /* OTKXENUM_h */
