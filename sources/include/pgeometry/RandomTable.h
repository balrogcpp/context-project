/*-------------------------------------------------------------------------------------
Copyright (c) 2006 John Judnich

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable
for any damages arising from the use of this software. Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following
restrictions:
1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If
you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not
required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original
software.
3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------------*/

#pragma once
#include <OgreMath.h>
#include <OgrePrerequisites.h>

#include <chrono>
#include <random>

// random table class that speeds up PG a bit
class RandomTable {
 public:
  RandomTable(unsigned long size = 0x8000);
  virtual ~RandomTable();

  void resetRandomIndex();
  float getUnitRandom();
  float getRangeRandom(float start, float end);

 protected:
  unsigned long tableSize;
  float *table;
  unsigned long customRandomIndex;

  void generateRandomNumbers();
};

// implementation below
inline RandomTable::RandomTable(unsigned long size) : tableSize(size), table(0), customRandomIndex(0) {
  table = (float *)malloc(sizeof(float) * tableSize);
  generateRandomNumbers();
}

inline RandomTable::~RandomTable() {
  if (table) {
    free(table);
    table = 0;
  }
}

inline void RandomTable::resetRandomIndex() { customRandomIndex = 0; }

inline float RandomTable::getUnitRandom() {
  // prevent against overflow
  if (customRandomIndex > tableSize - 1) customRandomIndex = 0;
  return table[customRandomIndex++];
}

inline float RandomTable::getRangeRandom(float start, float end) { return (start + ((end - start) * getUnitRandom())); }

inline void RandomTable::generateRandomNumbers() {
  // using our Mersenne Twister (preferred way)
  using namespace std::chrono;
  auto time = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

  std::mt19937 rng(time);
  auto duration_before_frame = std::chrono::system_clock::now().time_since_epoch();
  long millis = std::chrono::duration_cast<std::chrono::microseconds>(duration_before_frame).count();
  rng.seed(millis);
  for (unsigned long i = 0; i < tableSize; i++)
    table[i] = static_cast<float>(rng()) / static_cast<float>(std::mt19937::max());
}
