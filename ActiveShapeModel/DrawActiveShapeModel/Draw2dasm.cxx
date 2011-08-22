/*

	Author:  John R Durkin
	Date last modified:  6/13/2011

	Displays +/- some number of standard deviations of a shape model using the mean image and a principal component image


*/



#include "itkImage.h"
#include "itkImagePCAShapeModelEstimator.h"
#include "itkBinaryThresholdImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSignedDanielssonDistanceMapImageFilter.h"

#include "itkShiftScaleImageFilter.h"
#include "itkNumericSeriesFileNames.h"
#include <iostream>
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkAddImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkMultiplyByConstantImageFilter.h"
using namespace std;


double SCALE_CONSTANT;
int main(int argc, char* argv[]  )
{
	if(argc < 8)
	{
		cout << "error param" << endl;
		for(int p = 0; p < argc; p++)
			cout << argv[p] << endl;
	}
	for(int p = 0; p < argc; p++)
			cout << argv[p] << endl;
	//declaring variables for parameters
	char* fileName1 = argv[1];
	char* fileName2 = argv[2];
	char* outputfile1 = argv[3];
	char* outputfile2 = argv[4];
	double low = atof(argv[5]);
	double high = atof(argv[6]);
	double EV = atof(argv[7]);
	SCALE_CONSTANT = atof(argv[8]);




	//defining internal pixel type
	typedef   float           InternalPixelType;
	const     unsigned int    Dimension = 2;
	typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;


	//defining image type for threshold filter and image
	typedef unsigned char OutputPixelType;
	typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
	typedef itk::BinaryThresholdImageFilter< InternalImageType, OutputImageType>  ThresholdingFilterType;




	//setting up reader and writer types
	typedef  itk::ImageFileReader< InternalImageType > ReaderType;
	typedef  itk::ImageFileWriter<  OutputImageType  > WriterType;

	
	ReaderType::Pointer reader1 = ReaderType::New();
	ReaderType::Pointer reader2 = ReaderType::New();
	ReaderType::Pointer reader3 = ReaderType::New();

	WriterType::Pointer writer = WriterType::New();
	WriterType::Pointer writer2 = WriterType::New();

	reader1->SetFileName(fileName1);
	try
	{
		reader1->Update();
	}
	catch( itk::ExceptionObject & err )
	{
	std::cout << "reader1 exception" << std::endl;
	std::cout << err << std::endl;
	//return EXIT_FAILURE;
	}
	
	reader2->SetFileName(fileName2);
	reader3->SetFileName(fileName2);
	try
	{
		reader2->Update();
		reader3->Update();
	}
	catch( itk::ExceptionObject & err )
	{
	std::cout << "reader2 exception" << std::endl;
	std::cout << err << std::endl;
	//return EXIT_FAILURE;
	}


	typedef itk::MultiplyByConstantImageFilter<InternalImageType,float,InternalImageType> MultType;
	MultType::Pointer multFilter = MultType::New();
	MultType::Pointer multFilter2 = MultType::New();
	

	multFilter->SetInput(reader2->GetOutput());

	multFilter->SetConstant((float)SCALE_CONSTANT*EV);

	multFilter2->SetInput(reader3->GetOutput());

	multFilter2->SetConstant((float)SCALE_CONSTANT*EV);

	try
	{
		multFilter->Update();
		multFilter2->Update();
	}
	catch( itk::ExceptionObject & err )
	{
		std::cout << "multFilter exception caught !" << std::endl;
		std::cout << err << std::endl;
		//return EXIT_FAILURE;
	}

	typedef itk::AddImageFilter<InternalImageType, InternalImageType, InternalImageType> AddFilterType;
	AddFilterType::Pointer addFilter = AddFilterType::New();
	addFilter->SetInput1(reader1->GetOutput());
	addFilter->SetInput2(multFilter->GetOutput());

	typedef itk::SubtractImageFilter<InternalImageType, InternalImageType, InternalImageType> SubFilterType;
	SubFilterType::Pointer subFilter = SubFilterType::New();
	subFilter->SetInput1(reader1->GetOutput());
	subFilter->SetInput2(multFilter2->GetOutput());

	try
	{
		addFilter->Update();
	}
	catch( itk::ExceptionObject & err )
	{
		std::cout << "ExceptionObject caught !" << std::endl;
		std::cout << err << std::endl;
		//return EXIT_FAILURE;
	}

		try
	{
		subFilter->Update();
	}
	catch( itk::ExceptionObject & err )
	{
		std::cout << "ExceptionObject caught !" << std::endl;
		std::cout << err << std::endl;
		//return EXIT_FAILURE;
	}



	typedef itk::BinaryThresholdImageFilter< 
                        InternalImageType, 
                        OutputImageType    >    ThresholdingFilterType;
  
  ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
  ThresholdingFilterType::Pointer thresholder2 = ThresholdingFilterType::New();
                        
  thresholder->SetLowerThreshold( low );
  thresholder->SetUpperThreshold(     high );

  thresholder->SetOutsideValue(  0  );
  thresholder->SetInsideValue(  255 );
  thresholder->SetInput(addFilter->GetOutput());

    thresholder2->SetLowerThreshold( low );
  thresholder2->SetUpperThreshold(     high );

  thresholder2->SetOutsideValue(  0  );
  thresholder2->SetInsideValue(  255 );
  thresholder2->SetInput(subFilter->GetOutput());

	  try
	{
	thresholder->Update();
	}
	catch( itk::ExceptionObject & err )
	{
	std::cout << "ExceptionObject caught !" << std::endl;
	std::cout << err << std::endl;
	//return EXIT_FAILURE;
	}
	writer->SetInput( thresholder->GetOutput() );
	writer->SetFileName(outputfile1);

	writer2->SetInput( thresholder2->GetOutput() );
	writer2->SetFileName(outputfile2);

	 try
	{
	writer->Update();
	writer2->Update();
	}
	catch( itk::ExceptionObject & err )
	{
	std::cout << "ExceptionObject caught !" << std::endl;
	std::cout << err << std::endl;

	}

	return 0;
}

