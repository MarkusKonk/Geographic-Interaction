/*
GRT MIT License
Copyright (c) <2012> <Nicholas Gillian, Media Lab, MIT>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*! \mainpage Gesture Recognition Toolkit

 \section intro_sec Introduction
 
  The Gesture Recognition Toolkit (GRT) is a cross-platform, open-source, c++ machine learning library that has been specifically designed for real-time gesture recognition.
 The GRT has been designed to:
 - be easy to use and integrate into your existing c++ projects
 - be compatible with any type of sensor or data input
 - be easy to rapidly train with your own gestures
 - be easy to extend and adapt with your own custom processing or feature extraction algorithms (if needed)
 
 The GRT features a large number of algorithms that can be used to:
 - recognize static postures (such as if a user has their hands in a specific posture or if a device fitted with an accelerometer is being held in a distinct orientation)
 - recognize dynamic temporal gestures (such as a swipe or tap gesture)
 - perform regression (i.e. continually map an input signal to an output signal, such as mapping the angle of a user's hands to the angle a steering wheel should be turned in a driving game)
 
 You can access the main GRT wiki <a href="http://www.nickgillian.com/wiki/pmwiki.php/GRT/GestureRecognitionToolkit">here</a>.
 */

#ifndef GRT_MAIN_HEADER
#define GRT_MAIN_HEADER

//Include the Utilities
#include "Util/GRTVersionInfo.h"
#include "Util/GRTCommon.h"
#include "Util/Matrix.h"
#include "Util/MatrixDouble.h"
#include "Util/RangeTracker.h"
#include "Util/TrainingDataRecordingTimer.h"
#include "Util/SVD.h"
#include "Util/LUDecomposition.h"
#include "Util/Cholesky.h"
#include "Util/EigenvalueDecomposition.h"
#include "Util/TestResult.h"
#include "Util/ClassificationResult.h"

//Include the data structures
#include "DataStructures/LabelledClassificationData.h"
#include "DataStructures/LabelledTimeSeriesClassificationData.h"
#include "DataStructures/LabelledContinuousTimeSeriesClassificationData.h"
#include "DataStructures/LabelledRegressionData.h"
#include "DataStructures/UnlabelledClassificationData.h"

//Include the PreProcessing Modules
#include "PreProcessingModules/Derivative.h"
#include "PreProcessingModules/LowPassFilter.h"
#include "PreProcessingModules/HighPassFilter.h"
#include "PreProcessingModules/MovingAverageFilter.h"
#include "PreProcessingModules/DoubleMovingAverageFilter.h"
#include "PreProcessingModules/SavitzkyGolayFilter.h"
#include "PreProcessingModules/DeadZone.h"

//Include the FeatureExtraction Modules
#include "FeatureExtractionModules/PeakDetection.h"
#include "FeatureExtractionModules/ZeroCrossingCounter/ZeroCrossingCounter.h"
#include "FeatureExtractionModules/FFT/FFT.h"
#include "FeatureExtractionModules/FFT/FFTFeatures.h"
#include "FeatureExtractionModules/MovementTrajectoryFeatures/MovementTrajectoryFeatures.h"
#include "FeatureExtractionModules/MovementIndex/MovementIndex.h"
#include "FeatureExtractionModules/TimeDomainFeatures/TimeDomainFeatures.h"
#include "FeatureExtractionModules/PCA/PrincipalComponentAnalysis.h"
#include "FeatureExtractionModules/KMeansQuantizer/KMeansQuantizer.h"
#include "FeatureExtractionModules/TimeseriesBuffer/TimeseriesBuffer.h"

//Include the PostProcessing Modules
#include "PostProcessingModules/ClassLabelFilter.h"
#include "PostProcessingModules/ClassLabelTimeoutFilter.h"
#include "PostProcessingModules/ClassLabelChangeFilter.h"

//Include Classification Modules
#include "ClassificationModules/AdaBoost/AdaBoost.h"
#include "ClassificationModules/ANBC/ANBC.h"
#include "ClassificationModules/BAG/BAG.h"
#include "ClassificationModules/DTW/DTW.h"
#include "ClassificationModules/GMM/GMM.h"
#include "ClassificationModules/HMM/HMM.h"
#include "ClassificationModules/KNN/KNN.h"
#include "ClassificationModules/LDA/LDA.h"
#include "ClassificationModules/MinDist/MinDist.h"
#include "ClassificationModules/Softmax/Softmax.h"
#include "ClassificationModules/SVM/SVM.h"

//Include the Regression Modules
#include "RegressionModules/ArtificialNeuralNetworks/MLP/MLP.h"
#include "RegressionModules/LinearRegression/LinearRegression.h"
#include "RegressionModules/LogisticRegression/LogisticRegression.h"

//Include the Clustering algorithms
#include "ClusteringModules/KMeans/KMeans.h"
#include "ClusteringModules/GaussianMixtureModels/GaussianMixtureModels.h"

//Include the Context Modules
#include "ContextModules/Gate.h"

//Include the Recognition Pipeline
#include "GestureRecognitionPipeline/GestureRecognitionPipeline.h"

#endif //GRT_MAIN_HEADER
