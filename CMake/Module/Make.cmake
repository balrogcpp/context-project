# This source file is part of Glue Engine. Created by Andrey Vasiliev

include(ProcessorCount)
processorcount(PROCESSOR_COUNT)
set(MAKE_COMMAND make -j${PROCESSOR_COUNT})
