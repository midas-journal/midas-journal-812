/*

	Author:  John R Durkin
	Date last modified:  6/13/2011

	creates a mean image and set of principle components from a series 
	of training images


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

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif
using namespace std;
int main(  )
{

	//declaring variables for parameters
	int NUMTRAININGIMAGES = 6;
	int NUMPC = 3;
	std::string IMAGENAME = "fixed";
	/*

	//reading in parameters from console
	cout << "Enter number of training images " << endl;
	cin >> NUMTRAININGIMAGES;
	cout << "Enter number of principle components " << endl;
	cin >> NUMPC;
	cout << "Enter the training image prefix for example:  for image000.jpg, image001.jpg, image002.jpg... enter \"image\"" << endl;
	cin >> IMAGENAME;

*/
	//defining internal pixel type
	typedef   float           InternalPixelType;
	const     unsigned int    Dimension = 2;
	typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;


	//defining image type for threshold filter and image
	typedef float BinaryOutputPixelType;
	typedef itk::Image< BinaryOutputPixelType, Dimension > OutputImageType;
	typedef itk::BinaryThresholdImageFilter< InternalImageType, OutputImageType>  ThresholdingFilterType;



	//defining types for danielsson distance map image filter
	typedef float MapOutputPixelType;
	typedef itk::Image< BinaryOutputPixelType, Dimension> MapInputImageType;
	typedef itk::Image< MapOutputPixelType, Dimension> MapOutputImageType;
	typedef itk::SignedDanielssonDistanceMapImageFilter< OutputImageType, MapOutputImageType> FilterType;


	//setting up reader and writer types
	typedef  itk::ImageFileReader< InternalImageType > ReaderType;
	typedef  itk::ImageFileWriter<  InternalImageType  > WriterType;




	//making a vector full of the names of the input files
	itk::NumericSeriesFileNames::Pointer fileNamesCreator = 
	itk::NumericSeriesFileNames::New();

	fileNamesCreator->SetStartIndex( 0 );
	fileNamesCreator->SetEndIndex( NUMTRAININGIMAGES  );
	fileNamesCreator->SetSeriesFormat( IMAGENAME + "%d.tif" );
	const std::vector<std::string> & trainingImageNames = 
	fileNamesCreator->GetFileNames();

	for( int p = 0; p < NUMTRAININGIMAGES; p++)
	{
		cout << trainingImageNames[p].c_str() << endl;
	}

	//creating the PCAShapeModelEstimator and initializing
	typedef itk::ImagePCAShapeModelEstimator<MapOutputImageType, InternalImageType> PCAEstimatorType;
	PCAEstimatorType::Pointer model = PCAEstimatorType::New();
	model->DebugOn();

	model->SetNumberOfTrainingImages(NUMTRAININGIMAGES);
	model->SetNumberOfPrincipalComponentsRequired(NUMPC);





	//setting up and running the filters on all of the training images
	for ( unsigned int k = 0; k < NUMTRAININGIMAGES; k++ )
	{

		ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
	                    
		thresholder->SetLowerThreshold( 255 );
		thresholder->SetUpperThreshold( 255 );
		thresholder->SetOutsideValue(  0  );
		thresholder->SetInsideValue(  255 );
		
		ReaderType::Pointer reader = ReaderType::New();
		FilterType::Pointer filter = FilterType::New();


		reader->SetFileName(trainingImageNames[k].c_str());
		thresholder->SetInput(reader->GetOutput());
		filter->SetInput(thresholder->GetOutput());
		filter->Update();
		
		model->SetInput(k, filter->GetOutput());
	}


	model->Update();
	model->Print(cout);


                                     
	//divide filter
	typedef itk::ShiftScaleImageFilter<InternalImageType, InternalImageType> DivideFilterType;


	

	//creating the file names for the output images
	itk::NumericSeriesFileNames::Pointer fileNamesCreator2 = itk::NumericSeriesFileNames::New();

	fileNamesCreator2->SetStartIndex( 0 );
	fileNamesCreator2->SetEndIndex( NUMPC - 1 );
	fileNamesCreator2->SetSeriesFormat( "pcImage%d.mha" );
	const std::vector<std::string> & outputImageNames = 
	fileNamesCreator2->GetFileNames();

	//writing the mean images
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName("meanImage.mha");
	writer->SetInput(model->GetOutput(0));
	writer->Update();
	for( int j = 0; j < NUMPC; j++)
	{
		cout << outputImageNames[j].c_str() << endl;
	}


	//printin the eigen values
	vnl_vector<double> eigenValues(NUMPC);
	eigenValues = model->GetEigenValues();
	cout << eigenValues<<endl;

	//writing the principle components
	for(unsigned int i = 0; i < NUMPC; i++)
	{
		//normalizing the images
		DivideFilterType::Pointer divider = DivideFilterType::New();
		divider->SetInput(model->GetOutput(i+1));
		divider->SetScale(1.0/sqrt(eigenValues(i)));
		
		WriterType::Pointer myWriter = WriterType::New();
		myWriter->SetFileName(outputImageNames[i].c_str());
		myWriter->SetInput(divider->GetOutput());
		myWriter->Update();
	}

	return 0;
}

