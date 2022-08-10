target_sources(CppUTest
    PRIVATE
        src/UtestPlatform.cpp
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

include(CheckCXXSymbolExists)
check_cxx_symbol_exists(fork "unistd.h" HAVE_FORK)
check_cxx_symbol_exists(waitpid "sys/wait.h" HAVE_WAITPID)
check_cxx_symbol_exists(gettimeofday "sys/time.h" HAVE_GETTIMEOFDAY)
check_cxx_symbol_exists(localtime_s "time.h" HAVE_LOCALTIME_S)
include(CheckStructHasMember)
check_struct_has_member("struct timespec" tv_sec time.h CPPUTEST_HAVE_STRUCT_TIMESPEC)

# The tests need to know these too:
set(CPPUTEST_HAVE_FORK ${HAVE_FORK} CACHE INTERNAL "")
set(CPPUTEST_HAVE_WAITPID ${HAVE_WAITPID} CACHE INTERNAL "")

target_link_libraries(CppUTest
    PRIVATE
        $<$<BOOL:${CMAKE_USE_PTHREADS_INIT}>:Threads::Threads>
)

target_compile_definitions(CppUTest
    PRIVATE
        $<$<BOOL:${CMAKE_USE_PTHREADS_INIT}>:CPPUTEST_HAVE_PTHREAD_MUTEX_LOCK>
        $<$<BOOL:${CPPUTEST_HAVE_FORK}>:CPPUTEST_HAVE_FORK>
        $<$<BOOL:${CPPUTEST_HAVE_WAITPID}>:CPPUTEST_HAVE_WAITPID>
        $<$<BOOL:${HAVE_GETTIMEOFDAY}>:CPPUTEST_HAVE_GETTIMEOFDAY>
        $<$<AND:$<BOOL:${MINGW}>,$<BOOL:${CPPUTEST_HAVE_STRUCT_TIMESPEC}>>:_TIMESPEC_DEFINED>
        $<$<AND:$<BOOL:${WIN32}>,$<BOOL:${HAVE_LOCALTIME_S}>>:MINGW_HAS_SECURE_API>
)
