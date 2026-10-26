This repository holds two pieces of my CS 300 work for ABCU's Computer Science department:

Project One: runtime and memory analysis. Pseudocode and Big-O analysis comparing a vector, a hash table, and a binary search tree, with a recommendation. (Connor McDonald 300 Project 1 Final.docx)
Project Two: working code. A command-line C++ program that loads the course file into a binary search tree, prints the courses in alphanumeric order, and looks up a single course and its prerequisites. (ProjectTwo.cpp)
What was the problem you were solving?

ABCU's advisors needed software to load a file of Computer Science courses (course number, title, and zero or more prerequisites) and use it throughout the day. They needed to print every course in alphanumeric order and look up one course with its prerequisites. The file also had to be checked for errors: each line needs at least a number and a title, and every prerequisite has to be a course that appears in the file.

How did you approach the problem?

I started with the access pattern. The data is loaded once at the start of the day, but advisors then search for courses and print the sorted list over and over. That made the cost of repeated operations more important than the cost of loading.

I wrote pseudocode for a vector, a hash table, and a binary search tree, then counted the cost of each line of code to get a Big-O analysis. The vector needs a linear scan for every search and has to be sorted before every print. The hash table has fast average lookups, but collisions can make it O(n) in the worst case, and it has no order, so it has to be copied out and sorted for every print. The binary search tree is already in order, so an in-order traversal prints the list in O(n), and search and insert are O(log n) on average. I recommended the binary search tree and built it in Project Two as a hand-written Node and CourseTree.

This is why data structures matter: the same program can be fast or slow depending on how the data is stored, and the right choice depends on which operations happen most often.

How did you overcome roadblocks?
Checking prerequisites. A prerequisite may be listed on a later line than the course that needs it, so it can't be checked while each line is being read. I load every course first and then check every prerequisite in a second pass.
Matching my recommendation to my code. My first version of Project One recommended a hash table, but my first Project Two used std::map, which is a different structure. My instructor pointed out the mismatch. I changed the recommendation to the binary search tree, which better fits the way the program is used, and rewrote Project Two with a hand-built tree so the two projects agree.
Revising the analysis. My first Big-O analysis was too general. I redid it line by line for each structure, added worst-case cases (hash collisions, an unbalanced tree), and added memory.
Tooling. Setting up the compiler and include path in my editor took a few tries, so I tested by compiling and running the program directly.
How has this work expanded your approach to designing software?

I now plan before I code. Writing the pseudocode first and comparing structures on paper made the C++ much easier, because the decisions were already made. I also think about how a program will actually be used before picking a structure, and I check that my design documents and my code say the same thing.

How has this work evolved the way you write maintainable, readable, and adaptable programs?
The program is split into small functions with one job each (loadCourses, printCourseList, printCourseInfo, printMenu), so a change in one place doesn't ripple everywhere.
The tree is its own class, so the storage can be swapped without rewriting the menu or the parsing.
I use descriptive names, comment the reason behind a decision and not just the code, and check input so bad entries produce a clear error message and not a crash or garbage output.
I free memory with a destructor that deletes every node, and I tested with bad files, missing files, and invalid menu entries.
Reusing one parsing routine across all three pseudocode versions showed me the value of modular design: only the storage and lookup had to change.
