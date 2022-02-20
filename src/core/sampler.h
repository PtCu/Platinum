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
#include <core/object.h>

namespace platinum
{
    class Sampler : public Object
    {
    public:
        Sampler(const PropertyTree &);

        Sampler(int64_t samplesPerPixel) : _samplesPerPixel(samplesPerPixel) {}

        virtual ~Sampler() = default;

        /**
         * @brief 对某个像素点开始采样
         * @param  p                像素点
         */
        virtual void StartPixel(const Vector2i &p);

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
        virtual Vector2f Get2D() = 0;

        CameraSample GetCameraSample(const Vector2i &p_raster, Filter *flter = nullptr);

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
        const Vector2f *Get2DArray(int n);

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

        /**
         * @brief Set the Sample Number object
         * @param  sampleNum        My Param doc
         * @return true 
         * @return false 
         */
        virtual bool SetSampleNumber(int64_t sampleNum);

        int64_t CurrentSampleIndex() const
        {
            return _currentPixelSampleIndex;
        }

        const int64_t _samplesPerPixel;

    protected:
        // 当前处理的像素点
        Vector2i _currentPixel;

        // 当前处理的像素样本索引
        int64_t _currentPixelSampleIndex;

        // 用于储存一维样本数量的列表
        std::vector<int> _samples1DArraySizes;

        // 用于储存二维样本数量的列表
        std::vector<int> _samples2DArraySizes;

        // 用于储存一维样本的列表
        std::vector<std::vector<float>> _sampleArray1D;

        // 用于存储二维样本的列表
        std::vector<std::vector<Vector2f>> _sampleArray2D;

    private:
        size_t _array1DOffset;
        size_t _array2DOffset;
    };
    inline Vector3f UniformSampleHemisphere(const Vector2f &u)
    {
        float z = u[0];
        float r = glm::sqrt(glm::max((float)0, (float)1. - z * z));
        float phi = 2 * Pi * u[1];
        return Vector3f(r * glm::cos(phi), r * glm::sin(phi), z);
    }

    inline float UniformHemispherePdf() { return Inv2Pi; }

    inline float CosineHemispherePdf(float cosTheta) { return cosTheta * InvPi; }

    inline Vector3f UniformSampleSphere(const Vector2f &u)
    {
        float z = 1 - 2 * u[0];
        float r = glm::sqrt(glm::max((float)0, (float)1 - z * z));
        float phi = 2 * Pi * u[1];
        return Vector3f(r * glm::cos(phi), r * glm::sin(phi), z);
    }

    inline float UniformSpherePdf() { return Inv4Pi; }

    inline Vector2f ConcentricSampleDisk(const Vector2f &u)
    {
        // Map uniform random numbers to $[-1,1]^2$
        Vector2f uOffset = 2.f * u - Vector2f(1, 1);

        // Handle degeneracy at the origin
        if (uOffset.x == 0 && uOffset.y == 0)
            return Vector2f(0, 0);

        // Apply concentric mapping to point
        float theta, r;
        if (glm::abs(uOffset.x) > glm::abs(uOffset.y))
        {
            r = uOffset.x;
            theta = PiOver4 * (uOffset.y / uOffset.x);
        }
        else
        {
            r = uOffset.y;
            theta = PiOver2 - PiOver4 * (uOffset.x / uOffset.y);
        }
        return r * Vector2f(glm::cos(theta), glm::sin(theta));
    }

    inline Vector3f CosineSampleHemisphere(const Vector2f &u)
    {
        Vector2f d = ConcentricSampleDisk(u);
        float z = std::sqrt(glm::max((float)0, 1 - d.x * d.x - d.y * d.y));
        return Vector3f(d.x, d.y, z);
    }

    inline Vector2f UniformSampleTriangle(const Vector2f &u)
    {
        float su0 = glm::sqrt(u[0]);
        return Vector2f(1 - su0, u[1] * su0);
    }

    inline float UniformConePdf(float cosThetaMax) { return 1 / (2 * Pi * (1 - cosThetaMax)); }

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