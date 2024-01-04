#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <utility>
#include <chrono>
#include <ctime>
using namespace std;

void quicksortLastElement(vector<pair<string, int>> &array, int lowerBound, int upperBound);
int partition(vector<pair<string, int>> &array, int lowerBound, int upperBound);
int getRandomPivot(int lowerBound, int upperBound);
void quicksortRandomElement(vector<pair<string, int>> &array, int lowerBound, int upperBound);
void quicksortMedianOfThree(vector<pair<string, int>> &array, int lowerBound, int upperBound);
void insertionSort(vector<pair<string, int>> &array, int size);
void printElapsedTimeQuick(const chrono::nanoseconds &elapsedTime, char pivotingStrategy, int threshold);
void printElapsedTimeInsertion(const chrono::nanoseconds &elapsedTime);
void swap(pair<string, int> &a, pair<string, int> &b);
void swap(int &a, int &b);

// global variables for accessing the original data and pivot before sorting
vector<pair<string, int>> originalData;
int pivotNum;

int main(int argc, char* argv[]){
    
    //to check if arguments passed in the command line fit the program
    if (argc < 5 || argc > 6) {
        cerr << "Usage: " << argv[0] << " [DataSetFileName].csv [pivoting_strategy] [k] [output_file].csv [v]" << endl;
        return 1;
    }

    //save the arguments passed from the command line into variables
    string datasetFileName = argv[1];
    char pivotingStrategy = argv[2][0];
    int k = stoi(argv[3]);
    string outputFileName = argv[4];
    bool verbose = (argc == 6 && argv[5][0] == 'v');
	
	fstream file (datasetFileName, ios::in); //open the data set file

    // Check if the file is open
    if (!file.is_open()) {
        cerr << "Error opening the file!" << endl;
        return 1;
    }

    vector<pair<string, int>> data;
    string line;
    
    //copy the data from the file to a vector
    while (getline(file, line)) {
        stringstream str(line);
        string strValue;
        int intValue;

        if (getline(str, strValue, ';')) {
            if (str >> intValue) {
                data.push_back(make_pair(strValue, intValue));
            }    
        }  
    }
    file.close();
 
    //copy the unsorted data to the global vector to write the unsorted array to the log file
    originalData = data; 

    //start the clock for calculating run time
    auto start = chrono::high_resolution_clock::now(); 
    

    //if data size is bigger than threshold, run quick sort. If not, run insertion sort
    int size = data.size();
    if(size > k){
   		switch (pivotingStrategy) {
            case 'l':
                quicksortLastElement(data, 0, data.size() - 1);
                break;
            case 'r':
                srand(time(0));
                quicksortRandomElement(data, 0, data.size() - 1);
                break;
            case 'm':
                srand(time(0));
                quicksortMedianOfThree(data, 0, data.size() - 1);
                break;
            default:
                cerr << "Invalid pivoting strategy." << endl;
                return 1;
        }
        
    }
    else{
   		insertionSort(data, size);
    }

    //runtime calculation stops here
    auto end = chrono::high_resolution_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::nanoseconds>(end - start); 
    
    //copy the sorted array into the output file
    ofstream filesort;
    filesort.open (outputFileName);

    for (int i = 0; i < data.size(); i++) {
        filesort << data[i].first << ";" << data[i].second << endl;
    }

    //log file is only generated when quick sort is used.
    if(size > k){
        if (verbose) {
            ofstream logFile("log.txt");
            logFile << "Pivot " << pivotNum << endl;
            logFile << "Array: ";
            for (int i = 0; i < originalData.size(); i++) {
                logFile << originalData[i].second << ", ";
            }
            logFile.close();
        }

        printElapsedTimeQuick(elapsedTime, pivotingStrategy, k);
    }
    else{
        printElapsedTimeInsertion(elapsedTime);
    }

	return 0;
}

//partition function of quick sort
int partition(vector<pair<string, int>> &array, int lowerBound, int upperBound){
    int pivot = array[upperBound].second;
    pivotNum = originalData[upperBound].second; //save pivot before sorting to put on the log file 
    int start = lowerBound;
    int end = upperBound-1;
    while(start < end){
        while(array[start].second <= pivot && start <= end){
            start++;
        }
        while(array[end].second > pivot && start <= end){
            end--;
        }
        if(start < end){
            swap(array[start].second, array[end].second);
            swap(array[start].first, array[end].first);
        }
    }
    swap(array[start].second, array[upperBound].second);
    swap(array[start].first, array[upperBound].first);
    return start;
}

//used when quick sort pivot is the last element
void quicksortLastElement(vector<pair<string, int>> &array, int lowerBound, int upperBound){
    if(lowerBound < upperBound){
        int location = partition(array, lowerBound, upperBound);
        quicksortLastElement(array, lowerBound, location-1);
        quicksortLastElement(array, location+1, upperBound);
    }
}

//function to get a random number for necessary pivoting strategies
int getRandomPivot(int lowerBound, int upperBound) {
    return rand() % (upperBound - lowerBound + 1) + lowerBound;
}

//run quick sort for random element
void quicksortRandomElement(vector<pair<string, int>> &array, int lowerBound, int upperBound) {
    if (lowerBound < upperBound) {
        int pivotIndex = getRandomPivot(lowerBound, upperBound);
        swap(array[pivotIndex], array[upperBound]);
        int location = partition(array, lowerBound, upperBound);
        quicksortRandomElement(array, lowerBound, location - 1);
        quicksortRandomElement(array, location + 1, upperBound);
    }
}

//run quick sort for the median of 3 random elements as pivot
void quicksortMedianOfThree(vector<pair<string, int>> &array, int lowerBound, int upperBound){
    if (lowerBound < upperBound) {
        int random1 = getRandomPivot(lowerBound, upperBound);
        int random2 = getRandomPivot(lowerBound, upperBound);
        int random3 = getRandomPivot(lowerBound, upperBound);
        int median;

        if (random1 > random2){
            if (random2 > random3)
                median = random2;
            else if (random1 > random3)
                median = random3;
            else
                median = random1;
        }
        else{
            if (random1 > random3)
                median = random1;
            else if (random2 > random3)
                median = random3;
            else
                median = random2;
        }

        swap(array[median], array[upperBound]);
        int location = partition(array, lowerBound, upperBound);
        quicksortRandomElement(array, lowerBound, location - 1);
        quicksortRandomElement(array, location + 1, upperBound);

    }
}

//insertion sort implementation
void insertionSort(vector<pair<string, int>> &array, int size){
	for (int pointer = 1; pointer < size ; pointer++){
		int currentInt = array[pointer].second;
        string currentStr = array[pointer].first;
		int position = pointer;
		while (position > 0 && array[position - 1].second > currentInt){
			array[position].second = array[position -1].second;
            array[position].first = array[position - 1].first;
			position = position -1;
		} 
		array[position].second = currentInt;
        array[position].first = currentStr;
	}
}

//print pivoting strategy, threshold and runtime for quick sort
void printElapsedTimeQuick(const chrono::nanoseconds &elapsedTime, char pivotingStrategy, int threshold) {
    cout << "Time taken by QuickSort with pivot strategy '"<< pivotingStrategy <<"' and threshold "<< threshold << ": "
         << elapsedTime.count() << " ns." << endl;
}

//function to get the runtime of insertion sort
void printElapsedTimeInsertion(const chrono::nanoseconds &elapsedTime){
    cout << "Insertion Sort is used. Time Elapsed: "
         << elapsedTime.count() << " ns." << endl;
}

//swap function for string 
void swap(pair<string, int> &a, pair<string, int> &b) {
    pair<string, int> temp = a;
    a = b;
    b = temp;
}

//swap function for integer
void swap(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}
