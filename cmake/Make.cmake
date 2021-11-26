include(ProcessorCount)
processorcount(PROCESSOR_COUNT)
set(MAKE_COMMAND make -j${PROCESSOR_COUNT})
