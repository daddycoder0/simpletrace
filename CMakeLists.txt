cmake_minimum_required (VERSION 2.9)

project (simpletrace)
	find_package(Threads REQUIRED)

	if(THREADS_HAVE_PTHREAD_ARG)
		set_property(TARGET simpletrace PROPERTY COMPILE_OPTIONS "-pthread")
		set_property(TARGET simpletrace PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
	endif()

	FILE(GLOB APPSRC "src/*.cpp" "src/*.h")
	FILE(GLOB_RECURSE THIRDPARTY "ThirdParty/*.cpp" "ThirdParty/*.h")

	add_executable(simpletrace  ${APPSRC} ${THIRDPARTY})
	target_link_libraries(simpletrace ${CMAKE_THREAD_LIBS_INIT})
