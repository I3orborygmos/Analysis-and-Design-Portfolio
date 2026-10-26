
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <cctype>

using namespace std;

// ---------------------------------------------------------------------------
// Course
// One course: its number, title, and the course numbers of its prerequisites.
// ---------------------------------------------------------------------------
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;   // 0 to n prerequisite course numbers
};

// ---------------------------------------------------------------------------
// Node
// One node of the binary search tree.
// ---------------------------------------------------------------------------
struct Node {
    Course course;
    Node* left;
    Node* right;

    explicit Node(const Course& c) : course(c), left(nullptr), right(nullptr) {}
};

// ---------------------------------------------------------------------------
// CourseTree
// Binary search tree of Course objects, ordered by course number.
// ---------------------------------------------------------------------------
class CourseTree {
public:
    CourseTree() : root(nullptr), count(0) {}

    ~CourseTree() { clear(); }

    // The tree owns its nodes, so copying is not allowed.
    CourseTree(const CourseTree&) = delete;
    CourseTree& operator=(const CourseTree&) = delete;

    // Insert a course. Returns false (and ignores the course) if a course
    // with the same number is already in the tree.
    bool insert(const Course& course) {
        return insertNode(root, course);
    }

    // Find a course by number. Returns nullptr if it is not in the tree.
    const Course* search(const string& courseNumber) const {
        Node* current = root;
        while (current != nullptr) {
            if (courseNumber == current->course.courseNumber) {
                return &current->course;
            } else if (courseNumber < current->course.courseNumber) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return nullptr;
    }

    // Print every course in alphanumeric order (in-order traversal).
    void printInOrder() const {
        printInOrder(root);
    }

    // Remove every node from the tree.
    void clear() {
        clear(root);
        root = nullptr;
        count = 0;
    }

    bool empty() const { return root == nullptr; }
    size_t size() const { return count; }

private:
    Node* root;
    size_t count;

    // Recursive insert. 'node' is a reference to the pointer being examined
    // so a new node can be attached directly where the search ends.
    bool insertNode(Node*& node, const Course& course) {
        if (node == nullptr) {
            node = new Node(course);
            count++;
            return true;
        }
        if (course.courseNumber < node->course.courseNumber) {
            return insertNode(node->left, course);
        } else if (course.courseNumber > node->course.courseNumber) {
            return insertNode(node->right, course);
        }
        return false;   // duplicate course number
    }

    // Left subtree, then this node, then right subtree = sorted order.
    void printInOrder(const Node* node) const {
        if (node == nullptr) {
            return;
        }
        printInOrder(node->left);
        cout << node->course.courseNumber << ", "
             << node->course.courseTitle << endl;
        printInOrder(node->right);
    }

    void clear(Node* node) {
        if (node == nullptr) {
            return;
        }
        clear(node->left);
        clear(node->right);
        delete node;
    }
};

// ---------------------------------------------------------------------------
// trim
// Removes leading and trailing whitespace from a string.
// ---------------------------------------------------------------------------
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// ---------------------------------------------------------------------------
// toUpper
// Returns an uppercase copy of a string so course numbers are not case
// sensitive (for example, "csci400" matches "CSCI400").
// ---------------------------------------------------------------------------
string toUpper(string s) {
    for (char& c : s) {
        c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    }
    return s;
}

// ---------------------------------------------------------------------------
// splitCSV
// Splits one line on commas and trims each field. A trailing comma produces
// no extra field, which is fine because empty prerequisites are ignored.
// ---------------------------------------------------------------------------
vector<string> splitCSV(const string& line) {
    vector<string> tokens;
    stringstream ss(line);
    string token;
    while (getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

// ---------------------------------------------------------------------------
// loadCourses
// Reads the course file into the tree. Expected line format:
//   courseNumber,courseTitle,prerequisite1,prerequisite2,...
// A course may have no prerequisites.
//
// Mirrors the Project One pseudocode:
//   Pass 1: read every line into a list.
//   Pass 2: parse each line (format check), create a Course, and insert it.
//   Pass 3: check that every prerequisite is a course that is in the file.
// The prerequisite check is a separate pass because a prerequisite may be
// listed on a later line than the course that needs it.
//
// If any error is found, a message is printed, the tree is emptied, and
// false is returned, so the advisor never works from partial data.
// ---------------------------------------------------------------------------
bool loadCourses(const string& fileName, CourseTree& courses) {
    courses.clear();

    ifstream inFile(fileName);
    if (!inFile.is_open()) {
        cout << "Error: could not open file \"" << fileName << "\"." << endl;
        return false;
    }

    // ---- Pass 1: read every line ----
    vector<string> rawLines;
    string line;
    while (getline(inFile, line)) {
        rawLines.push_back(line);
    }
    inFile.close();

    if (rawLines.empty()) {
        cout << "Error: file is empty." << endl;
        return false;
    }

    // Course numbers seen in the file, used for the prerequisite check.
    unordered_set<string> courseNumbers;

    // ---- Pass 2: parse each line into a Course and insert it ----
    for (size_t i = 0; i < rawLines.size(); i++) {
        string text = trim(rawLines[i]);

        // Skip blank lines.
        if (text.empty()) {
            continue;
        }

        vector<string> fields = splitCSV(text);

        // Format rule 1: at least a course number and a title.
        if (fields.size() < 2) {
            cout << "Error on line " << (i + 1)
                 << ": missing course number or title." << endl;
            courses.clear();
            return false;
        }

        // Format rule 2: course number must not be empty.
        if (fields[0].empty()) {
            cout << "Error on line " << (i + 1)
                 << ": empty course number." << endl;
            courses.clear();
            return false;
        }

        // Format rule 3: course title must not be empty.
        if (fields[1].empty()) {
            cout << "Error on line " << (i + 1)
                 << ": empty course title." << endl;
            courses.clear();
            return false;
        }

        Course course;
        course.courseNumber = toUpper(fields[0]);
        course.courseTitle = fields[1];

        // Every field after the title is a prerequisite course number.
        for (size_t j = 2; j < fields.size(); j++) {
            if (!fields[j].empty()) {
                course.prerequisites.push_back(toUpper(fields[j]));
            }
        }

        courseNumbers.insert(course.courseNumber);

        if (!courses.insert(course)) {
            cout << "Warning: duplicate course " << course.courseNumber
                 << " on line " << (i + 1) << " was ignored." << endl;
        }
    }

    // ---- Pass 3: every prerequisite must be a course in the file ----
    bool fileIsValid = true;
    for (size_t i = 0; i < rawLines.size(); i++) {
        string text = trim(rawLines[i]);
        if (text.empty()) {
            continue;
        }

        vector<string> fields = splitCSV(text);

        // Prerequisites start at the third field.
        for (size_t j = 2; j < fields.size(); j++) {
            string prereq = toUpper(fields[j]);
            if (!prereq.empty() && courseNumbers.count(prereq) == 0) {
                cout << "Error on line " << (i + 1) << ": prerequisite "
                     << prereq << " is not a course in the file." << endl;
                fileIsValid = false;
            }
        }
    }

    if (!fileIsValid) {
        cout << "The file has errors, so no data was loaded." << endl;
        courses.clear();
        return false;
    }

    if (courses.empty()) {
        cout << "Error: no course data was loaded from the file." << endl;
        return false;
    }

    cout << "Data loaded successfully (" << courses.size()
         << " courses read)." << endl;
    return true;
}

// ---------------------------------------------------------------------------
// printCourseList
// Prints every course in alphanumeric order. The tree is already ordered,
// so an in-order traversal needs no separate sort.
// ---------------------------------------------------------------------------
void printCourseList(const CourseTree& courses) {
    cout << "Here is a sample schedule:" << endl << endl;
    courses.printInOrder();
}

// ---------------------------------------------------------------------------
// printCourseInfo
// Prompts for a course number, looks it up in the tree, and prints the course
// number and title followed by its prerequisite course numbers.
// ---------------------------------------------------------------------------
void printCourseInfo(const CourseTree& courses) {
    cout << "What course do you want to know about? ";
    string input;
    cin >> input;
    input = toUpper(trim(input));

    const Course* course = courses.search(input);
    if (course == nullptr) {
        cout << "Error: course " << input << " was not found." << endl;
        return;
    }

    cout << course->courseNumber << ", " << course->courseTitle << endl;

    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites: ";
    for (size_t i = 0; i < course->prerequisites.size(); i++) {
        if (i > 0) {
            cout << ", ";
        }
        cout << course->prerequisites[i];
    }
    cout << endl;
}

// ---------------------------------------------------------------------------
// printMenu
// Displays the main menu.
// ---------------------------------------------------------------------------
void printMenu() {
    cout << endl;
    cout << "   1. Load Data Structure." << endl;
    cout << "   2. Print Course List." << endl;
    cout << "   3. Print Course." << endl;
    cout << "   9. Exit" << endl;
    cout << endl;
}

// ---------------------------------------------------------------------------
// main
// Runs the menu loop and calls the matching function for each option.
// Options 2 and 3 are blocked until data has been loaded with option 1.
// ---------------------------------------------------------------------------
int main() {
    CourseTree courses;
    bool dataLoaded = false;
    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    while (true) {
        printMenu();
        cout << "What would you like to do? ";

        // Menu input must be a number.
        if (!(cin >> choice)) {
            cin.clear();                  // reset the error state
            cin.ignore(10000, '\n');      // throw away the bad input
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
        case 1: {
            cout << "Enter the name of the data file: ";
            string fileName;
            cin >> fileName;
            dataLoaded = loadCourses(fileName, courses);
            break;
        }
        case 2:
            if (!dataLoaded) {
                cout << "Please load the data first (option 1)." << endl;
            } else {
                printCourseList(courses);
            }
            break;
        case 3:
            if (!dataLoaded) {
                cout << "Please load the data first (option 1)." << endl;
            } else {
                printCourseInfo(courses);
            }
            break;
        case 9:
            cout << "Thank you for using the course planner!" << endl;
            return 0;
        default:
            cout << choice << " is not a valid option." << endl;
            break;
        }
    }

    return 0;
}
