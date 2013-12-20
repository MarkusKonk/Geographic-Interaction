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

#ifndef GRT_CLASS_LABEL_TIMEOUT_FILTER_HEADER
#define GRT_CLASS_LABEL_TIMEOUT_FILTER_HEADER

#include "../Util/GRTCommon.h"
#include "../GestureRecognitionPipeline/PostProcessing.h"

namespace GRT{
    
class ClassLabelAndTimer{
public:
    ClassLabelAndTimer(){
        classLabel = 0;
    }
    ClassLabelAndTimer(UINT classLabel,double timeoutDuration){
        this->classLabel = classLabel;
        timer.start(timeoutDuration);
    }
    
    //Setters
    bool set(UINT classLabel,double timeoutDuration){
        if( classLabel > 0 && timeoutDuration > 0 ){
            this->classLabel = classLabel;
            timer.start(timeoutDuration);
            return true;
        }
        return false;
    }
    
    //Getters
    UINT getClassLabel(){ return classLabel; }
    bool timerReached(){ return timer.timerReached(); }
    double getRemainingTime(){ return timer.getMilliSeconds(); }
    
protected:
    UINT classLabel;
    Timer timer;
};
    
    
    
class ClassLabelTimeoutFilter : public PostProcessing{
public:
    /**
     Default Constructor. Sets the timeoutDuration and filterMode parameters. 
     
     The timeoutDuration sets how long (in milliseconds) the Class Label Timeout Filter will debounce valid 
     predicted class labels for, after it has viewed the first valid predicted class label which triggers the 
     debounce mode to be activated. A valid predicted class label is simply a predicted class label that is not 
     the default null rejection class label (i.e. a label with the class value of 0). The filterMode parameter 
     sets how the Class Label Timeout Filter reacts to different predicted class labels (different from the 
     predicted class label that triggers the debounce mode to be activated). There are two options for the filterMode:
     ALL_CLASS_LABELS or INDEPENDENT_CLASS_LABELS.  In the ALL_CLASS_LABELS filterMode, after the debounce mode 
     has been activated, all class labels will be ignored until the current timeoutDuration period has elapsed, 
     regardless of which class actually triggered the timeout. Alternatively, in the INDEPENDENT_CLASS_LABELS mode, 
     the debounce mode will be reset if a different predicted class label is detected that is different from the 
     predicted class label that initially triggered the debounce mode to be activated. For instance, if the debounce 
     mode was activated with the class label of 1, and then class 2 was input into the class label filter, then the
     debounce mode would be reset to class 2, even if the timeoutDuration for class 1 had not expired.
     
     @param double timeoutDuration: sets the timeoutDuration value (in milliseconds). Default value timeoutDuration=1000
     @param UINT filterMode: sets the filterMode parameter. Default value filterMode=ALL_CLASS_LABELS
     */
    ClassLabelTimeoutFilter(double timeoutDuration = 1000,UINT filterMode = ALL_CLASS_LABELS);
    
    /**
     Copy Constructor.
     
     Copies the values from the rhs ClassLabelTimeoutFilter to this instance of the ClassLabelTimeoutFilter.
     
     @param const ClassLabelTimeoutFilter &rhs: the rhs from which the values will be copied to this this instance of the ClassLabelTimeoutFilter
     */
    ClassLabelTimeoutFilter(const ClassLabelTimeoutFilter &rhs);
    
    /**
     Default Destructor
     */
	virtual ~ClassLabelTimeoutFilter();
    
    /**
     Assigns the equals operator setting how the values from the rhs instance will be copied to this instance.
     
     @param const ClassLabelTimeoutFilter &rhs: the rhs instance from which the values will be copied to this this instance of the ClassLabelTimeoutFilter
     @return returns a reference to this instance of the ClassLabelTimeoutFilter
     */
    ClassLabelTimeoutFilter& operator=(const ClassLabelTimeoutFilter &rhs);
    
    /**
     Sets the PostProcessing clone function, overwriting the base PostProcessing function.
     This function is used to clone the values from the input pointer to this instance of the PostProcessing module.
     This function is called by the GestureRecognitionPipeline when the user adds a new PostProcessing module to the pipeline.
     
     @param const PostProcessing *postProcessing: a pointer to another instance of a ClassLabelTimeoutFilter, the values of that instance will be cloned to this instance
	 @return true if the clone was successful, false otherwise
     */
    virtual bool clone(const PostProcessing *postProcessing);
    
    /**
     Sets the PostProcessing process function, overwriting the base PostProcessing function.
     This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
     This function calls the ClassLabelTimeoutFilter's filter(...) function.
     
     @param const vector< double > &inputVector: the inputVector that should be processed.  This should be a 1-dimensional vector containing a predicted class label
	 @return true if the data was processed, false otherwise
     */
    virtual bool process(const vector< double > &inputVector);
    
    /**
     Sets the PostProcessing reset function, overwriting the base PostProcessing function.
     This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
     This function resets the ClassLabelTimeoutFilter by re-initiliazing the instance.
     
     @return true if the ClassLabelTimeoutFilter was reset, false otherwise
     */
    virtual bool reset();
    
    /**
     This saves the post processing settings to a file.
     This overrides the saveSettingsToFile function in the PostProcessing base class.
     
     @param string filename: the name of the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveSettingsToFile(string filename);
    
    /**
     This saves the post processing settings to a file.
     This overrides the saveSettingsToFile function in the PostProcessing base class.
     
     @param string filename: the name of the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveSettingsToFile(fstream &file);
    
    /**
     This loads the post processing  settings from a file.
     This overrides the loadSettingsFromFile function in the PostProcessing base class.
     
     @param string filename: the name of the file to load the settings from
     @return returns true if the settings were loaded successfully, false otherwise
     */
    virtual bool loadSettingsFromFile(string filename);
    
    /**
     This loads the post processing settings from a file.
     This overrides the loadSettingsFromFile function in the PostProcessing base class.
     
     @param string filename: the name of the file to load the settings from
     @return returns true if the settings were loaded successfully, false otherwise
     */
    virtual bool loadSettingsFromFile(fstream &file);

    /**
     This function initializes the ClassLabelTimeoutFilter.
     
     @param double timeoutDuration: sets the timeoutDuration value (in milliseconds)
     @param UINT filterMode: sets the filterMode parameter
     @return returns true if the ClassLabelTimeoutFilter was initialized, false otherwise
     */
    bool init(double timeoutDuration,UINT filterMode = ALL_CLASS_LABELS); 
    
    /**
     This is the main filter function which filters the input predictedClassLabel.
     
     @param UINT predictedClassLabel: the predictedClassLabel which should be filtered
     return returns the filtered class label
     */
	UINT filter(UINT predictedClassLabel);
    
    /**
     Get the most recently filtered class label value.
     
     @return returns the filtered class label
     */
    UINT getFilteredClassLabel(){ return filteredClassLabel; }
    
    /**
     Get if the filter is currently ignorning new inputs because the timeout is active.
     
     @return returns true if the timeout mode is active, false otherwise
     */
    bool isTimeoutActive();
    
    /**
     Sets the timeoutDuration parameter, must be a value greater than 0.
     
     The timeoutDuration sets how long (in milliseconds) the Class Label Timeout Filter will debounce valid 
     predicted class labels for, after it has viewed the first valid predicted class label which triggers the 
     debounce mode to be activated. A valid predicted class label is simply a predicted class label that is not 
     the default null rejection class label (i.e. a label with the class value of 0).  If the Class Label Filter 
     has been initialized then the module will be reset.
     
     @param double timeoutDuration: the new timeoutDuration parameter
     @return returns true if the timeoutDuration parameter was updated, false otherwise
     */
    bool setTimeoutDuration(double timeoutDuration);
    
    /**
     Sets the filterMode parameter, must be a value greater than 0.
     
     The filterMode parameter sets how the Class Label Timeout Filter reacts to different predicted class labels 
     (different from the predicted class label that triggers the debounce mode to be activated). There are two 
     options for the filterMode: ALL_CLASS_LABELS or INDEPENDENT_CLASS_LABELS.  In the ALL_CLASS_LABELS filterMode, 
     after the debounce mode has been activated, all class labels will be ignored until the current timeoutDuration 
     period has elapsed, regardless of which class actually triggered the timeout. Alternatively, in the 
     INDEPENDENT_CLASS_LABELS mode, the debounce mode will be reset if a different predicted class label is detected 
     that is different from the predicted class label that initially triggered the debounce mode to be activated. 
     
     For instance, if the debounce mode was activated with the class label of 1, and then class 2 was input into the class 
     label filter, then the
     debounce mode would be reset to class 2, even if the timeoutDuration for class 1 had not expired.  If the Class Label 
     Filter has been initialized then the module will be reset.
     
     @param UINT filterMode: the new filterMode parameter
     @return returns true if the filterMode parameter was updated, false otherwise
     */
    bool setFilterMode(UINT filterMode);
    
    enum FilterModes{ALL_CLASS_LABELS=0,INDEPENDENT_CLASS_LABELS};
    
protected:
    UINT filteredClassLabel;
    UINT filterMode;
    double timeoutDuration;
    vector< ClassLabelAndTimer > classLabelTimers;
    
    static RegisterPostProcessingModule< ClassLabelTimeoutFilter > registerModule;
};

}//End of namespace GRT

#endif //GRT_CLASS_LABEL_TIMEOUT_FILTER_HEADER
