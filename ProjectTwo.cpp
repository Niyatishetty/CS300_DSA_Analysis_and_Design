
//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Niyati Shetty
// Version     : 1.0
// Copyright   : Copyright © 2025 SNHU COCE
// Description : 7.1 Project Two
//============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cctype>
#include <time.h>
#include <vector>

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 179;

// define a structure to hold Course information
struct Course {
    string courseId; // unique identifier
    string courseName;
    vector<string> prerequisites;
};

// forward declarations
string prerequisitesString(Course course);
void displayCourse(Course course);

//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:
    // Define structures to hold Course
    struct Node {
        Course course;
        unsigned int key;
        Node* next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a Course
        Node(Course aCourse) : Node() {
            course = aCourse;
        }

        // initialize with a Course and a key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) {
            key = aKey;
        }
    };

    vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(string key);
    void quickSort(vector<Course>& courses, int begin, int end);
    int partition(vector<Course> & courses, int begin, int end);
    bool validatePrerequisites(vector<string> prerequisites);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAll();
    void Remove(string courseId);
    void ValidateCourse();
    Course Search(string courseId);
};

/**
 * Default constructor
 */
HashTable::HashTable() {
    // Initalize node structure by resizing tableSize
    nodes.resize(tableSize);
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    // invoke local tableSize to size with this->
    this->tableSize = size;

    // resize nodes size
    nodes.resize(size);
}


/**
 * Destructor
 */
HashTable::~HashTable() {
    // Free storage when class is destroyed and erase nodes beginning
    nodes.erase(nodes.begin());
}


/**
 * Calculate the hash value of a given key.
 * Note that key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(string key) {
    // logic to calculate a hash value and return key tableSize 
    int asciiSum = 0;
    for (int index = 0; index < 4; index++) {
        asciiSum += static_cast<int>(key[index]);
    }
    // now add the integer value of last three characters
    asciiSum += atoi(key.substr(4, 3).c_str());
    return asciiSum % tableSize;
}

/**
 * Insert a Course
 *
 * @param The Course to insert 
 */
void HashTable::Insert(Course course) {
    // create the hash for the given courseId
    unsigned key = hash(course.courseId);
    //std::cout << course.courseId << " :" << key << endl;
    
    // retrieve node using key
    // if no entry found for the key
    Node* oldNode = &(nodes.at(key));
    if (oldNode == nullptr) {
        Node* newNode = new Node(course, key);
        nodes.insert(nodes.begin() + key, (*newNode));
    }
    else {
        // if node is not used
        if (oldNode->key == UINT_MAX) {
            //set oldNode key to key, set old node to course and old node next to null pointer
            oldNode->key = key;
            oldNode->course = course;
            oldNode->next = nullptr;
        }
        else {
            // you have key collision, find the next open node
            while (oldNode->next != nullptr) {
                oldNode = oldNode->next;
            }
            // add new Node to the end
            oldNode->next = new Node(course, key);
        }
    }
    
}

/**
 * Remove a Course
 *
 * @param courseId The course id to search for
 */
void HashTable::Remove(string courseId) {
    // Remove a Course for courseId
    unsigned int key = hash(courseId);

    Node* node = &(nodes.at(key));
    if (node->key != UINT_MAX) {
        if (node->course.courseId.compare(courseId) == 0) {
            //remove this course
            //cout << "remove top node for " << courseId << endl;
            if (node->next == nullptr) {
                node->key = UINT_MAX;
                return;
            }
            else {
                nodes.at(key) = *(node->next);
                return;
            }
        }
        else {
            Node* cur = node->next;
            Node* pre = node;
            while (cur != nullptr) {
                if (cur->course.courseId.compare(courseId) == 0) {
                    //delete this node
                    pre->next = cur->next;
                    delete cur;
                    cur = nullptr;
                    return;

                }
                pre = cur;
                cur = cur->next;
            }
        }
    }

}

/**
 * Validate the prerequisites courses and verify if that
 * course exists.
 */
bool HashTable::validatePrerequisites(vector<string> prerequisites) {
    
    // loop all the prerequisites and validates the course exists
    vector<string>::iterator it;
    for (it = prerequisites.begin(); it != prerequisites.end(); it++) {
        Course course = Search(*it);
        if (course.courseId.empty()) {
            return false;
        }
    }
    return true;
}

/**
 * Validate the courses
 */
void HashTable::ValidateCourse() {
    // Validate all Courses and delete invalid courses with
    // non-existent prerequisites

    // for node begin to end iterate
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        // if key not equal to UINT_MAX
        if (it->key != UINT_MAX) {
            if (!validatePrerequisites(it->course.prerequisites)) {
                Remove(it->course.courseId);
            }

            // node is equal to next iter
            Node* node = it->next;

            //while node not equal to nullptr
            while (node != nullptr) {
                if (!validatePrerequisites(it->course.prerequisites)) {
                    Remove(it->course.courseId);
                }
                // node is equal to next node
                node = node->next;
            }
        }
    }


}

/**
 * Partition the vector of courses into two parts, low and high
 *
 * @param courses Address of the vector<Course> instance to be partitioned
 * @param begin Beginning index to partition
 * @param end Ending index to partition
 */
int HashTable::partition(vector<Course>& courses, int begin, int end) {
    //set low and high equal to begin and end
    int low = begin;
    int high = end;

    // Calculate the middle element as middlePoint (int)
    // Set Pivot as middlePoint element title to compare (string)  
    string pivot = courses[low + (high - low) / 2].courseId;

    bool done = false;
    // while not done 
    while (!done) {
        // keep incrementing low index while courses[low].courseId < Pivot
        while (courses[low].courseId < pivot) {
            ++low;

        }

        // keep decrementing high index while Pivot < courses[high].courseId
        while (courses[high].courseId > pivot) {
            --high;
        }

        if (low >= high) {
            done = true;
        }
        else {
            // else swap the low and high course (built in vector method)
            // move low and high closer ++low, --high
            std::swap(courses[low], courses[high]);
            ++low;
            --high;
        }
    }

    /* If there are zero or one elements remaining,
    all courses are partitioned. Return high */

    return high;
}

/**
 * Perform a quick sort on courseId
 * Average performance: O(n log(n))
 * Worst case performance O(n^2))
 *
 * @param Course address of the vector<Course> instance to be sorted
 * @param begin the beginning index to sort on
 * @param end the ending index to sort on
 */
void HashTable::quickSort(vector<Course>& courses, int begin, int end) {
    //set mid equal to 0

    int mid = 0;

    /* Base case: If there are 1 or zero Courses to sort,
     partition is already sorted otherwise if begin is greater
     than or equal to end then return*/
    if (begin >= end) {
        return;
    }

    /* Partition courses into low and high such that
     midpoint is location of last element in low */
    mid = partition(courses, begin, end);

    // recursively sort low partition (begin to mid)
    quickSort(courses, begin, mid);

    // recursively sort high par
    quickSort(courses, mid + 1, end);
}
/**
 * Print all Courses
 */
void HashTable::PrintAll() {
    // Print all Course List

    // If the courses are not added or deleted. We can move "sortCourseList" 
    // as data member and store and sort only once and print all the times.
    vector<Course> courseList;

    // for node begin to end iterate
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        // if key not equal to UINT_MAX
        if (it->key != UINT_MAX) {
            courseList.push_back(it->course);

            // node is equal to next iter
            Node* node = it->next;

            //while node not equal to nullptr
            while (node != nullptr) {
                courseList.push_back(it->course);
                // node is equal to next node
                node = node->next;
            }
        }
    }

    // sort courseList using quick sort and print
    quickSort(courseList, 0, courseList.size() - 1);

    std::cout << "Here is the Course List:" << endl << endl;
    vector<Course>::iterator cit;
    for (cit = courseList.begin(); cit != courseList.end(); cit++) {
        std::cout << cit->courseId << ", " << cit->courseName << ", " 
            << prerequisitesString(*cit) << endl;
    }
    std::cout << endl;


}


/**
 * Search for the specified courseId
 *
 * @param courseId The course id to search for
 */
Course HashTable::Search(string courseId) {
    Course course;

    // convert to upper case, search for and return a Course
    transform(courseId.begin(), courseId.end(), courseId.begin(), ::toupper);

    // create the key for given courseId
    unsigned int key = hash(courseId);
    Node* node = &(nodes.at(key));
    if (node != nullptr && node->key != UINT_MAX && node->course.courseId.compare(courseId) == 0) {
        return node->course;
    }

    if (node == nullptr || node->key == UINT_MAX) {
        return course;
    }

    // while node not equal to nullptr
    while (node != nullptr) {

        // if the current node matches, return it
        if (node->key != UINT_MAX && node->course.courseId.compare(courseId) == 0) {
            return node->course;
        }
        //node is equal to next node
        node = node->next;
    }

    
    return course;
}

//============================================================================
// Static methods used for testing
//============================================================================
/**
 * Return Prerequisites string from Course.prerequisites vector 
 *
 * @param Course which contains Prerequisites.
 */
string prerequisitesString(Course course) {

    string prerequisites = "";
    vector<string>::iterator it;
    for (it = course.prerequisites.begin(); it != course.prerequisites.end(); it++) {
        if (prerequisites != "") {
            prerequisites += ", ";
        }
        prerequisites += *it;
    }
    return prerequisites;
}



/**
 * Display the course information to the console (std::out)
 *
 * @param course struct containing the course info
 */
void displayCourse(Course course) {
    
    std::cout << course.courseId << ", " << course.courseName << endl;
    std::cout << "Prerequisites: " << prerequisitesString(course) << endl << endl;
    
    return;
}

/**
 * Load a CSV file containing course into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the course list
 */
void readInputFile(string csvPath, HashTable* hashTable) {
    
    string line;
    vector<string> originalFile;

    //Open the file and read all lines
    std::ifstream ifile(csvPath.c_str());
    if (ifile.is_open())
    {
        while (ifile.good())
        {
            getline(ifile, line);
            if (line != "")
                originalFile.push_back(line);
        }
        ifile.close();

        if (originalFile.size() == 0)
            throw runtime_error(std::string("No Data in the file ").append(csvPath));
    }

    // Now loop through all the entries in the vector and split the line
    // and store it in Course structure and call HashTable::Insert()
    vector<string>::iterator it;
    for (it = originalFile.begin(); it != originalFile.end(); it++)
    {
        //cout <<  *it << endl;
        vector<string> tokens;
        string token;
        stringstream line(*it);
        while (getline(line, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 3) {
            //std::cout << "Skipping line: " << *it << endl;
            continue;
        }
        Course course;
        course.courseId = tokens.at(0);
        course.courseName = tokens.at(1);
        vector<string> prerequisites;

        for (int index = 2; index < tokens.size(); index++) {
            if (tokens.at(index) != "") {
                prerequisites.push_back(tokens.at(index));
            }
        }

        if (!prerequisites.empty()) {
            course.prerequisites = prerequisites;
        }

        hashTable->Insert(course);
        //displayCourse(course);
    }

    // Validate courses after reading and inserting all lines
    hashTable->ValidateCourse();
}


/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        break;
    default:
        cout << "Please enter the input file name: ";
        cin >> csvPath;
        //csvPath = "CS_300_ABCU_Advising_Program_Input.csv";
        //csvPath = "Test_Input.csv";
    }

    std::ifstream ifile(csvPath.c_str());
    if (!ifile.is_open()) {
        std::cout << "Input file not found " << csvPath << endl;
        std::cout << "Please provide valid input file. Exiting!" << endl;
        return 1;
    }


    // Define a hash table to hold all the courses
    HashTable* courseList;
    courseList = new HashTable();

    int choice = 0;
    string searchCourseId = "";
    Course searchedCourse;

    cout << "Welcome to the course planner." << endl << endl;
    while (choice != 9) {
        cout << "  1. Load Data Structure." << endl;
        cout << "  2. Print Course List." << endl;
        cout << "  3. Print Course." << endl;
        cout << "  9. Exit" << endl;
        cout << "What would you like to do?: ";
        cin >> choice;

        switch (choice) {

        case 1:
           // read the file and store in courseList
            readInputFile(csvPath, courseList);
            cout << "Course List read from file and loaded." << endl;

            break;

        case 2:
            // print all courses
            courseList->PrintAll();
            break;

        case 3:
            cout << "What course do you want to know about? ";
            cin >> searchCourseId;
            
            searchedCourse = courseList->Search(searchCourseId);
            if (!searchedCourse.courseId.empty()) {
                displayCourse(searchedCourse);
            }
            else {
                cout << "Course Id " << searchCourseId << " not found." << endl;
            }
           break;
        case 9:
            break;
        default:
            cout << choice << " is not a valid option." << endl << endl;
            break;
        }
    }

    cout << "Thank you for using the course planner!" << endl;

    return 0;
}




