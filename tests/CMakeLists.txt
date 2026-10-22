include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/8240fa7d62f73e01c7af27d61ed965d6d66698fa.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

find_package(Java REQUIRED COMPONENTS Development)
include(UseJava)

add_jar(EasyJniTestsSubject SOURCES "java/Subject.java")
get_target_property(EasyJniTestsSubjectPath EasyJniTestsSubject JAR_FILE)

add_executable(EasyJniTests
    "environment/JvmEnvironment.h"
    "environment/JvmEnvironment.cpp"
    "environment/SubjectTest.h"
    "environment/SubjectTest.cpp"
    "mirror/SubjectParentInterface.h"
    "mirror/SubjectParent.h"
    "mirror/SubjectInterface.h"
    "mirror/Subject.h"
    "test_main.cpp"
    "test_signature.cpp"
    "test_class.cpp"
    "test_method.cpp"
    "test_field.cpp"
    "test_ref.cpp"
    "test_exception.cpp"
    "test_string.cpp"
    "test_array.cpp"
    "test_mirror.cpp"
)

target_link_libraries(EasyJniTests EasyJni GTest::gtest)
target_compile_definitions(EasyJniTests PRIVATE EASYJNI_TESTCLASSPATH="${EasyJniTestsSubjectPath}")

# idk there is another way
get_filename_component(JVM_LIB_DIR ${JAVA_JVM_LIBRARY} DIRECTORY)
# for windows JAVA_JVM_LIBRARY is jvm.lib, not jvm.dll

if(WIN32)
    get_filename_component(EASYJNI_JAVA_HOME ${JVM_LIB_DIR} DIRECTORY)
    set(JVM_DIR "${EASYJNI_JAVA_HOME}/bin/server")
    set(ENV_VAR "PATH")
else()
    set(JVM_DIR "${JVM_LIB_DIR}")
    set(ENV_VAR "LD_LIBRARY_PATH")
endif()

# https://gitlab.kitware.com/cmake/cmake/-/work_items/21453
# gtest_discover_tests(EasyJniTests DISCOVERY_MODE PRE_TEST PROPERTIES ENVIRONMENT "${GTEST_ENV}")
get_target_property(OTEST_LAUNCHER EasyJniTests TEST_LAUNCHER)
set(EASYJNI_TEST_LAUNCHER "${CMAKE_COMMAND}" -E env --modify "${ENV_VAR}=path_list_append:${JVM_DIR}")
if(OTEST_LAUNCHER)
    list(APPEND EASYJNI_TEST_LAUNCHER ${OTEST_LAUNCHER})
endif()
set_target_properties(EasyJniTests PROPERTIES TEST_LAUNCHER "${EASYJNI_TEST_LAUNCHER}")
gtest_discover_tests(EasyJniTests DISCOVERY_MODE PRE_TEST)