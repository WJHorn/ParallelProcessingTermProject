#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <omp.h>
#include "timer.h"

using std::vector;
using std::cout;
using std::string;
using std::endl;

vector<double> salaries;
int threadNum;
struct Compare { int val; int index; };
double mean;

void readSalaries(vector<double>&);
int partitionVec(vector<double>&, int, int);
void sortSalaries(vector<double>&, int, int);
void medianSalaries(vector<double>);
void meanSalaries(vector<double>);

int main (int argc, char* argv[]) {
   if (argc == 3){
      threadNum = strtol(argv[1], NULL, 10);
      double start, finish, elapsed;
      cout << "Total threads: " << threadNum << endl;
      if (argv[2] == "f" || argv[2] == "F") {
         readSalaries(salaries);
      }
      else {
         srand(time(NULL));
         for (int i = 0; i < 10000000; i++) {
            //populate vector with 1 million random double vals between 0 and 100000
            int temp = rand() % 100000;
            salaries.push_back(temp);
         }
      }
      
      
      GET_TIME(start);
      sortSalaries(salaries, 0, salaries.size() - 1);
      GET_TIME(finish);
      elapsed = finish - start;
      printf("   The elapsed time to sort the Salaries is %e seconds\n", elapsed);
      
      GET_TIME(start);
      medianSalaries(salaries);
      GET_TIME(finish);
      elapsed = finish - start;
      printf("   The elapsed time to find the median Salary is %e seconds\n", elapsed);
      
      GET_TIME(start);
      meanSalaries(salaries);
      GET_TIME(finish);
      elapsed = finish - start;
      printf("   The elapsed time to find the mean Salary is %e seconds\n", elapsed);
   }
   else {
      cout << "Invalid run command!\nMake sure run command is in the form:\n./a.out [NumberOfThreads]\n";
   }
}

void readSalaries(vector<double>& values) {
   string line;
   int count = 0;
   double temp;
   std::ifstream salaries ("Salaries.txt");
   if (salaries.is_open()) {
      while (getline(salaries, line)) {
         temp = std::stod(line, 0);
         values.push_back(temp);
         count++;
      }
      salaries.close();
      cout << "Salaries have been read in!\n";
   }
   else {
      cout << "Unable to open file\n";
   }
}

int partitionVec(vector<double>& arr, int low, int high) {
   int pivot = arr[high];
   int i = (low - 1);
   
   #pragma omp parallel for num_threads(threadNum)
   for (int j = low; j <= high - 1; j++) {
      if (arr[j] <= pivot) {
         i++;
         std::swap(arr[i], arr[j]);
      }
   }
   std::swap(arr[i + 1], arr[high]);
   return (i + 1);
}
void sortSalaries(vector<double>& arr, int low, int high) {
   if (low < high) {
      int p = partitionVec(arr, low, high);
      
      #pragma omp parallel sections 
      {
         #pragma omp section
         sortSalaries(arr, low, p - 1);
         #pragma omp section
         sortSalaries(arr, p + 1, high);
      }
   }
}

void medianSalaries(vector<double> arr){
   //cannot be parallelized
   int size = arr.size();
   double median;
   if ((size % 2) == 0) {
      median = (arr[size / 2 - 1] + arr[size / 2]) / 2;
   }
   else {
      median = arr[size / 2];
   }
   cout << " Median Salary: " << median << endl;
}
void meanSalaries(vector<double> arr){
   //implement parallel mean calculation
   # pragma omp parallel for num_threads(threadNum) \
      reduction(+:mean) 
   for (int i = 0; i < arr.size(); i++) {
      mean += arr[i];
   }
   mean = mean / arr.size();
   cout << " Mean Salary: " << mean << endl;
}