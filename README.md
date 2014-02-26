OS_Course_Projects
==================

Two course projects of Operating Systems, Fall 2013

1. Several separate programs for creating and maintaining a Student database. The users of this database are the student advisors and students. The advisors have the right to change the entries in the database. Students only have the right to read the database. Several students and advisors
are allowed to access the database concurrently. Each student entry in the database has the following fields: Name, Student ID, Address, and Telephone Number. 

The program support the following operations:

Load: Loads the database. It should read the initial database from a file and load it into shared memory.

Print: Prints the contents of the shared memory to the screen with proper formating.

Change: Lets a student advisor change the data related to any student with the given Student ID. A student
advisor is required to have a password to change the database. You can assume the password is “000”, for simplicity.

Query: Lets anyone query the database. Anyone should be able to retrieve the record of any student by typing in the student’s ID.

Clean: Lets a student advisor save the contents of the shared memory to a file in a format that is suitable for loading into memory later and also deletes the shared memory segments and the semaphores created for synchronization.

2. A simple Stateless Network File Server

The file server uses a UNIX file as a virtual disk to store the files created by the clients. The server and client are implemented as Sun RPC server and client. The server exports the following operations for the client:

Create: creates a file with the give name in the user’s directory. Each user is assigned his/her own directory with his/her login name as the name of the directory. The login name of a user can obtained from the password file (using getpwuid(getuid()))->pw name). Initially, a newly created file is allocated 8 blocks. Dynamical memory allocation is employed here.

Read: reads a specified number of bytes from the specified position of a specified file and returns it to the client. Use variable length buffer. Returns appropriate error message if trying to read past the end of file.

Write: writes the specified number of bytes from a buffer to the specified file from a specified location. Use variable length buffer. Returns appropriate error message if trying to write in a location past the end of file.

List: lists the names of all files in the user’s directory.

Copy: takes the names of two files as parameter and copies the contents of the first file to the second file.

Delete: deletes the specified file.




