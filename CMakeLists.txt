project(CFE_RCVTEST_APP C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(${hk_MISSION_DIR}/fsw/platform_inc)
include_directories(${hyun_app_MISSION_DIR}/fsw/platform_inc)

# Create the app module
add_cfe_app(rcvtest_app fsw/src/rcvtest_app.c)

# Include the public API from sample_lib to demonstrate how
# to call library-provided functions
add_cfe_app_dependency(rcvtest_app sample_lib)
add_cfe_app_dependency(rcvtest_app spacey_lib)
# Add table
add_cfe_tables(rcvtestappTable fsw/tables/rcvtest_app_tbl.c)

# If UT is enabled, then add the tests from the subdirectory
# Note that this is an app, and therefore does not provide
# stub functions, as other entities would not typically make 
# direct function calls into this application.
if (ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif (ENABLE_UNIT_TESTS)

# This is for Visual Studio Code C/C++ include path settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)