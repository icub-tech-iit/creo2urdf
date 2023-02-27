/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


#include <wfcGlobal.h>
#include <wfcSession.h>
#include <pfcExceptions.h>
#include <OTKXUtils.h>


#ifndef OTK_USE_TKOUT
#define tkout cout
#else
extern std::ofstream tkout;
#endif

void ExportRasterImage(pfcWindow_ptr window, pfcRasterImageExportInstructions_ptr rasterExportInstrs, char* rasterType);


extern "C" wfcStatus otkExportToRaster ()
{
	ofstream exception_info;
	exception_info.open("exception_info.inf",ios::out);

    wfcWSession_ptr ses = wfcWSession::cast (pfcGetCurrentSession() );

	try
	{
		pfcModel_ptr model = ses->GetCurrentModel();

		if (model != NULL)
		{
			tkout << "Model name: " << model->GetInstanceName() <<endl;
		}
		else
			return wfcTK_GENERAL_ERROR;


		if (!model ||( model->GetType() != pfcMDL_PART && model->GetType() != pfcMDL_ASSEMBLY) )
			return wfcTK_GENERAL_ERROR;

		pfcWindow_ptr win = ses->GetModelWindow(model);

		if (win != NULL)
		{
			double rasterHeight = 7.5;
			double rasterWidth = 10.0;

			pfcBitmapImageExportInstructions_ptr bmp_instrs = pfcBitmapImageExportInstructions::Create (rasterWidth, rasterHeight);
			pfcRasterImageExportInstructions_ptr raster_instrs = pfcRasterImageExportInstructions::cast(bmp_instrs);
			ExportRasterImage (win, raster_instrs,"bmp");

			pfcTIFFImageExportInstructions_ptr tiff_instrs = pfcTIFFImageExportInstructions::Create (rasterWidth, rasterHeight);
			raster_instrs = pfcRasterImageExportInstructions::cast(tiff_instrs);
			ExportRasterImage (win, raster_instrs,"tiff");

			pfcJPEGImageExportInstructions_ptr jpeg_instrs = pfcJPEGImageExportInstructions::Create (rasterWidth, rasterHeight);
			raster_instrs = pfcRasterImageExportInstructions::cast(jpeg_instrs);
			ExportRasterImage (win, raster_instrs,"jpg");

			pfcEPSImageExportInstructions_ptr eps_instrs = pfcEPSImageExportInstructions::Create (rasterWidth, rasterHeight);
			raster_instrs = pfcRasterImageExportInstructions::cast(eps_instrs);
			ExportRasterImage (win, raster_instrs,"eps");


		}
		else
		{
			return wfcTK_GENERAL_ERROR;
		}

		return wfcTK_NO_ERROR;
	}
	OTK_EXCEPTION_HANDLER(exception_info);
			
	return wfcTK_GENERAL_ERROR;
}


void ExportRasterImage(pfcWindow_ptr window, pfcRasterImageExportInstructions_ptr rasterExportInstrs, char* rasterType)
{
	pfcDotsPerInch dpi = pfcRASTERDPI_400;
	pfcRasterDepth depth = pfcRASTERDEPTH_24;

	rasterExportInstrs->SetDotsPerInch(dpi);
	rasterExportInstrs->SetImageDepth(depth);

	pfcModel_ptr model = window->GetModel();
	xstring name = model->GetInstanceName();
	xstring Lname = name.ToLower();
	char rasterFileName [100];

	sprintf (rasterFileName, "%s.%s", const_cast<char*>((cStringT)Lname), rasterType);

	window->ExportRasterImage(rasterFileName, rasterExportInstrs);
}

