set(This MemoryUtils)

set(Sources
  include/MemoryUtils/BlockMemoryAllocator.hpp
  include/MemoryUtils/ObjectPool.hpp
)

add_library(${This} INTERFACE ${Sources})
target_include_directories(${This} INTERFACE include)

add_subdirectory(test)