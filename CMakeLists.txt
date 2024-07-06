cmake_minimum_required(VERSION 3.0.0)
project(aesd-assignments)
# A list of all automated test source files
# At minimum it should include the files in the test/assignmentX directory
# Students can also add paths to their own test files using a ../ prefix to
# reference this working directory
set(AUTOTEST_SOURCES
    ../student-test/assignment1/Test_validate_username.c
)
# A list of all files containing test code that is used for assignment validation
set(TESTED_SOURCE
    ../examples/autotest-validate/autotest-validate.c
    ../finder-app/finder.sh
    ../finder-app/writer.sh
    ../finder-app/finder-test.sh
)
add_subdirectory(assignment-autotest)
