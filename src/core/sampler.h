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

#include <core/utilities.h>
#include <glm/glm.hpp>

namespace platinum
{
    class Sampler
    {
    public:
        Sampler(int64_t samplesPerPixel) : _samplesPerPixel(samplesPerPixel) {}

        virtual ~Sampler() = default;

        /**
         * @brief 对某个像素点开始采样
         * @param  p                像素点
         */
        virtual void StartPixel(const glm::ivec2 &p);

        /**
         * @brief Set the Sample Number object
         * @param  sampleNum        My Param doc
         * @return true 
         * @return false 
         */
        virtual bool SetSampleNumber(int64_t sampleNum);
        /**
         * @brief 开始下一个样本，返回值为该像素是否采样完毕
         * @return true 没有完毕
         * @return false 完毕，可以采集下一个了
         */
        virtual bool StartNextSample();

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

        CameraSample GetCameraSample(const glm::ivec2 &p_raster, Filter *flter = nullptr);

        // 申请一个长度为n的一维随机变量数组
        void Request1DArray(int n);

        // 申请一个长度为n的二维随机变量数组
        void Request2DArray(int n);

        virtual int RoundCount(int n) const
        {
            return n;
        }

        /**
         * 获取包含n个样本的一维数组，需要根据之前request的值做校验
         * 返回一个数组，数组元素都为同一个分布，同一个维度的不同样本
         * @param  n
         * @return   数组首地址
         */
        const float *Get1DArray(int n);

        /**
         * 获取包含n个样本的一维数组，需要根据之前request的值做校验
         * 返回一个数组，数组元素都为同一个分布，同一个维度的不同样本
         * 这点需要跟get2D函数区分开
         * 例如，如果要对一个光源表面进行采样
         * 则最好调用该函数获取一系列同一个维度的样本
         * @param  n
         * @return   数组首地址
         */
        const glm::ivec2 *Get2DArray(int n);

        /**
         * @brief   由于Sampler的实现类存储一些状态信息，
         *          因此在多个线程中使用同一个采样器是线程不安全的。
         *          Clone()方法用来生成一个该采样器的一个新实例供多线程使用，
         *          并接受一个参数，若采样器包含随机数生成器，可以通过该参数为新的采样器指定一个不一样的随机数种子。
         *          这么做的原因在于相同的随机数序列可能会产生规则重复的噪声（repeating noise patterns），
         *          为了避免出现artifacts我们必须为新的采样器指定不同的随机数种子。

         * @param  seed             种子数
         * @return std::unique_ptr<Sampler> 
         */
        virtual std::unique_ptr<Sampler> Clone(int seed) = 0;

        int64_t CurrentSampleIndex() const
        {
            return _currentPixelSampleIndex;
        }

        const int64_t _samplesPerPixel;

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
        std::vector<std::vector<glm::ivec2>> _sampleArray2D;

    private:
        size_t _array1DOffset;
        size_t _array2DOffset;
    };
    glm::vec3 uniformSampleHemisphere(const glm::vec2 &u);
    float uniformHemispherePdf();
    glm::vec3 uniformSampleSphere(const glm::vec2 &u);
    float uniformSpherePdf();

    glm::vec2 ConcentricSampleDisk(const glm::vec2 &u);

    glm::vec2 UniformSampleTriangle(const glm::vec2 &u);

    inline glm::vec3 CosineSampleHemisphere(const glm::vec2 &u)
    {
        glm::vec2 d = ConcentricSampleDisk(u);
        float z = std::sqrt(glm::max((float)0, 1 - d.x * d.x - d.y * d.y));
        return glm::vec3(d.x, d.y, z);
    }

    inline float CosineHemispherePdf(float cosTheta) { return cosTheta * InvPi; }

    inline float BalanceHeuristic(int nf, float fPdf, int ng, float gPdf)
    {
        return (nf * fPdf) / (nf * fPdf + ng * gPdf);
    }

    inline float PowerHeuristic(int nf, float fPdf, int ng, float gPdf)
    {
        float f = nf * fPdf, g = ng * gPdf;
        return (f * f) / (f * f + g * g);
    }
}

#endif