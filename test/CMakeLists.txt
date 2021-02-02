set(This MemoryUtilsTests)

set(Sources
    src/BlockMemoryAllocatorTests.cpp
    src/ObjectPoolTests.cpp)

add_executable(${This} ${Sources})

target_link_libraries(${This}
  gtest_main
  MemoryUtils
)

add_test(
  NAME ${This}
  COMMAND ${This}
)