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

#ifndef GRT_FILE_PARSER_HEADER
#define GRT_FILE_PARSER_HEADER

#include <iostream>     // cout, endl
#include <fstream>      // fstream
#include <vector>
#include <string>
#include <algorithm>    // copy
#include <iterator>     // ostream_operator

namespace GRT {

using namespace std;

class FileParser{
public:
    FileParser(){
	    clear();
    }
    ~FileParser(){
    }
    
    vector< string >& operator[](const unsigned int &index){
        return fileContents[index];
    }

    bool parseCSVFile(string filename,bool removeNewLineCharacter=true){
        return parseFile(filename,removeNewLineCharacter,',');
    }
    
    bool parseTSVFile(string filename,bool removeNewLineCharacter=true){
        return parseFile(filename,removeNewLineCharacter,'\t');
    }
  
    bool getFileParsed(){
	  return fileParsed;
    }
  
    bool getConsistentColumnSize(){
	  return consistentColumnSize;
    }
  
    unsigned int getRowSize(){
	  return (unsigned int)fileContents.size();
    }
  
    unsigned int getColumnSize(){
	  return columnSize;
    }
  
    vector< vector< string > > getFileContents(){
	  return fileContents;
    }
    
    bool clear(){
        fileParsed = false;
        consistentColumnSize = false;
        columnSize = 0;
        fileContents.clear();
        return true;
    }
  
protected:
    
    bool parseFile(string filename,bool removeNewLineCharacter,const char seperator){
        
        //Clear any previous data
        clear();
        
        ifstream file( filename.c_str(), ifstream::in );
        if ( !file.is_open() ){
            return false;
        }
        
        vector< string > vec;
        string line;
        const int sepValue = seperator;
        
        //Loop over each line of data and parse the contents
        while ( getline(file,line) )
        {
            //Scan the line contents for the seperator token and parse the contents between each token
            vec.clear();
            string columnString = "";
            unsigned int length = (unsigned int)line.length();
            for(unsigned int i=0; i<length; i++){
                if( int(line[i]) == sepValue ){
                    vec.push_back( columnString );
                    columnString = "";
                }else columnString += line[i];
            }
            
            //Add the last column
            vec.push_back( columnString );
            
            //Check to make sure all the columns are consistent
            if( columnSize == 0 ){
                consistentColumnSize = true;
                columnSize = (unsigned int)vec.size();
            }else if( columnSize != vec.size() ) consistentColumnSize = false;
            
            //Remove the new line character from the string in the last column
            if( removeNewLineCharacter && vec.size() >= 1 ){
                size_t foundA = vec[ vec.size()-1 ].find('\n');
                size_t foundB = vec[ vec.size()-1 ].find('\r');
                if( foundA != std::string::npos || foundB != std::string::npos ){
                    string temp = vec[ vec.size()-1 ];
                    vec[ vec.size()-1 ] = temp.substr(0,temp.length()-1);
                }
            }
            
            fileContents.push_back( vec );
        }
        
        //Close the file
        file.close();
        
        //Flag that we have parsed the file
        fileParsed = true;
        
        return true;
    }
  
  bool fileParsed;
  bool consistentColumnSize;
  unsigned int columnSize;
  vector< vector< string > > fileContents;

};
    
}//End of namespace GRT

#endif //GRT_FILE_PARSER_HEADER
