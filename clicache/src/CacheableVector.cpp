/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */





#include "CacheableVector.hpp"
#include "DataOutput.hpp"
#include "DataInput.hpp"
#include "ExceptionTypes.hpp"
#include "impl/SafeConvert.hpp"

using namespace System;
using namespace System::Collections::Generic;

namespace Apache
{
  namespace Geode
  {
    namespace Client
    {

      // Region: ISerializable Members

      void CacheableVector::ToData(DataOutput^ output)
      {
        if(m_arrayList != nullptr)
        {
          output->WriteArrayLen(m_arrayList->Count);
          for each (Object^ obj in m_arrayList) {
						//TODO::split
            output->WriteObject(obj);
          }
        }
        else
          output->WriteByte(0xFF);
      }

     void CacheableVector::FromData(DataInput^ input)
      {
        int len = input->ReadArrayLen();
        for( int i = 0; i < len; i++)
        {
          m_arrayList->Add(input->ReadObject());
        }
      }

      System::UInt64 CacheableVector::ObjectSize::get()
      { 
        //TODO::
        /*System::UInt32 size = static_cast<System::UInt32> (sizeof(CacheableVector^));
        for each (ISerializable^ val in this) {
          if (val != nullptr) {
            size += val->ObjectSize;
          }
        }*/
        return m_arrayList->Count;
    }  // namespace Client
  }  // namespace Geode
}  // namespace Apache

 } //namespace 

