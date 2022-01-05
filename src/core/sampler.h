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

#ifndef CORE_SAMPLER_H_
#define CORE_SAMPLER_H_

#include <core/defines.h>
#include <glm/glm.hpp>
namespace platinum {
    class Sampler {
        virtual ~Sampler() = default;
        Sampler(int64_t samplesPerPixel);
        // 对某个像素点开始采样
        virtual void StartPixel(const glm::ivec2& p);

        /**
         * 返回一维随机变量，每次调用之后，维度下标都会自增
         * 下次调用时，获取下个维度的随机变量
         * @return   一维随机变量
         */
        virtual float Get1D() = 0;

        /**
         * 返回二维随机变量，每次调用之后，维度下标都会自增
         * 下次调用时，获取下个维度的随机变量
         * @return   二维随机变量
         */
        virtual glm::vec2 Get2D() = 0;

        CameraSample GetCameraSample(const glm::ivec2& pRaster, Filter* flter = nullptr);

        // 申请一个长度为n的一维随机变量数组
        void Request1DArray(int n);

        // 申请一个长度为n的二维随机变量数组
        void Request2DArray(int n);

        virtual int RoundCount(int n) const {
            return n;
        }

        /**
         * 获取包含n个样本的一维数组，需要根据之前request的值做校验
         * 返回一个数组，数组元素都为同一个分布，同一个维度的不同样本
         * @param  n
         * @return   数组首地址
         */
        const float* Get1DArray(int n);

        /**
         * 获取包含n个样本的一维数组，需要根据之前request的值做校验
         * 返回一个数组，数组元素都为同一个分布，同一个维度的不同样本
         * 这点需要跟get2D函数区分开
         * 例如，如果要对一个光源表面进行采样
         * 则最好调用该函数获取一系列同一个维度的样本
         * @param  n
         * @return   数组首地址
         */
        const glm::ivec2* Get2DArray(int n);

        // 开始下一个样本，返回值为该像素是否采样完毕
        virtual bool StartNextSample();

        virtual std::unique_ptr<Sampler> Clone(int seed) = 0;

        virtual bool SetSampleIndex(int64_t sampleNum);


        int64_t CurrentSampleIndex() const {
            return _currentPixelSampleIndex;
        }

        const int64_t samplesPerPixel;
    protected:

        // 当前处理的像素点
        glm::ivec2 _currentPixel;

        // 当前处理的像素样本索引
        int64_t _currentPixelSampleIndex;

        // 用于储存一维样本数量的列表
        std::vector<int> _samples1DArraySizes;

        // 用于储存二维样本数量的列表
        std::vector<int> _samples2DArraySizes;

        // 用于储存一维样本的列表
        std::vector<std::vector<float>> _sampleArray1D;

        // 用于存储二维样本的列表
        std::vector<std::vector<glm::vec2>> _sampleArray2D;

    private:

        size_t _array1DOffset;
        size_t _array2DOffset;

    };
}

#endif