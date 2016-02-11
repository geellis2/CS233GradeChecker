#include <iostream>
#include <fstream>
//#include <boost>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
using namespace std;
using std::vector;
using namespace boost::filesystem;


vector<string> getFiles(const path & directory, string fND){
    vector<string> files;
    //make sure user gave me a directory
    if ( !exists(directory) || !is_directory(directory)) {
        std::cout << "Error: " << directory << " is not a valid directory" << std::endl;
        exit(1);
    }
    //look at all paths in the given directory
    for(directory_iterator it = directory_iterator(directory); it != directory_iterator(); ++it){
        if (is_directory(*it)){ 
            vector<string> temp = getFiles(it->path(), fND);//recurse into path if path is a dir.
            files.insert( files.end(), temp.begin(), temp.end() ); //concatenate vectors
        }
        else {
            //check to see if given file is a lab grade report
            string curr = it->path().native();
            //get file name descriptors out of string
            size_t i = 0; 
            bool add = true;
            string subs;
            //this is calculated for every file. inefficient, might not be worth fixing.
            while (i < fND.size()){
                //if fND was "I like ham" subs would be "I" then "like" then "ham"
                subs = fND.substr(i, fND.find(" ", i+1) - i);
                if (subs[0] != '~') //if subs is supposed to be allowed in grade filename
                    add = add && curr.find(subs) != string::npos;
                else //if subs is not supposed to be allowed in grade filename
                    add = add && curr.find(subs.substr(1)) == string::npos;
                i = fND.find(" ", i+1);  // iterate i to the beginning of next word
                if ( i != string::npos) //prevents looping back to i = 0 when done
                    i++;
            }
            // the "~" is used in backup files i.e. duplicates
            if (add && curr.find("~") == string::npos)
                files.push_back(curr);
        }        
    }
    return files;
};

double getSingleScore(const string & filename, string scoreLineIndicator, string seperator, double & total){
    std::ifstream infile; /**< std::ifstream used for reading the file */
    string currentLine = "";
    infile.open (filename);
    //in case for some reason filename actually isn't a file...
	if ( ! infile ) {
        std::cout << "Error: Can't open the file named " << filename << std::endl;
        exit(1);
    }

    while ( ! infile.eof()){
    //check if we found the right line
        getline(infile ,currentLine);
        if (currentLine.find(scoreLineIndicator) != string::npos){
            size_t seperatorLoc = currentLine.find(seperator);
            //find last ' ' before seperator in order to find first num
            size_t start = currentLine.rfind(" ", seperatorLoc - 1) + 1;
            double score = stod(currentLine.substr(start, seperatorLoc - start));
            //end of seperator to next space is second number
            start = seperatorLoc + seperator.length();
            size_t end = currentLine.find(" ", start);
            double sub = stod(currentLine.substr(start, end));
            total += sub;
            cout << filename << ": " << score << seperator << sub << endl;    
            return score;
        }
    }
    return 0;
};

double getGrade(vector<string> files, string scoreLineTemplate){
    //iterator to find important parts of scoreLineTemplate    
    int i = 0;
    bool foundScore = false;
    //first word of scoreLineTemplate
    string scoreLineIndicator = scoreLineTemplate.substr(0, scoreLineTemplate.find(" ")); 
    //while(!foundScore){
    //    if(scoreLineTemplate[i]
    string seperator = " / ";//find first number then find first non number after it, then find number
    //to be implemented if wanted^

    double sum = 0;
    double pointTotal = 0;
    //loop through files
    for(vector<string>::iterator it = files.begin(); it != files.end(); ++it)
        sum += getSingleScore(*it, scoreLineIndicator, seperator, pointTotal);
    return sum * 100 / pointTotal;
};

int main(int argc , char * argv[])
{
  if ( argc < 2 ){
    cout<<"usage: "<< argv[0] <<" <directory containing grade reports> [filename descriptors] [example line with score]" << endl;  
    return 0;
  }
    string fileNameDescriptors = "grade_report ~Exam"; //cs233 settings
    if (argc > 2)
        fileNameDescriptors = argv[2];
    string scoreLineTemplate = "Final Score: 20.0 / 20.0"; //cs233 settings
    if (argc > 3)
        scoreLineTemplate = argv[3];
    vector<string> filenames; 
    double grade; 
    //call function to get paths to grade reports
    path dir (argv[1]);
    filenames = getFiles(dir, fileNameDescriptors);
    //call function to determine grade from those .txt files
    grade = getGrade(filenames, scoreLineTemplate);
    //print current grade
    cout << "current grade = " << grade << "%\n";


  return 0;
}

