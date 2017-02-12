cmake_minimum_required (VERSION 2.6)

project (TestProj)
	find_package(Threads REQUIRED)

	if(THREADS_HAVE_PTHREAD_ARG)
		set_property(TARGET TestProj PROPERTY COMPILE_OPTIONS "-pthread")
		set_property(TARGET TestProj PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
	endif()

	FILE(GLOB APPSRC "*.cpp" "*.h")
	FILE(GLOB_RECURSE THIRDPARTY "ThirdParty/*.cpp" "ThirdParty/*.h")

	add_executable(TestProj  ${APPSRC} ${THIRDPARTY})
	target_link_libraries(TestProj ${CMAKE_THREAD_LIBS_INIT})
