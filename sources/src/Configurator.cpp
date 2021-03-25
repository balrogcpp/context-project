//MIT License
//
//Copyright (c) 2021 Andrey Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "pcheader.h"

using namespace std;

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
Configurator::Configurator(const string &file_name) {
  Load(file_name);
}

//----------------------------------------------------------------------------------------------------------------------
Configurator::~Configurator() = default;

//----------------------------------------------------------------------------------------------------------------------
void Configurator::Load(const string &file_name) {
  ifstream ifs(file_name);
  rapidjson::IStreamWrapper isw(ifs);
  document_.ParseStream(isw);

  if (ifs.is_open())
	ifs.close();
  else
	throw Exception("Error during parsing of " + file_name + " : can't open file");

  if (!document_.IsObject())
	throw Exception("Error during parsing of " + file_name + " : file is empty or incorrect");
}

} //namespace
