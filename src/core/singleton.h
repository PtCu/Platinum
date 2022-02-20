// Copyright 2022 ptcup
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CORE_SINGLETON_H_
#define CORE_SINGLETON_H_

namespace platinum
{
    template <typename T>
    class Singleton
    {
    public:
        static T &GetInstance()
        {
            static T instance;
            return instance;
        }
        static T& GetInstance(size_t size){
            static T instance(size);
            return instance;
        }
        Singleton(const Singleton &) = delete;
        Singleton &operator=(const Singleton &) = delete;
        virtual ~Singleton() = default;

    protected:
        //构造函数需要是protected的，以能够继承
        Singleton() = default;
    };
}

#endif