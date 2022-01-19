// Copyright 2021 ptcup
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

#ifndef CORE_FILM_H_
#define CORE_FILM_H_

#include <glm/glm.hpp>
#include <core/defines.h>
#include <core/image.h>
#include <core/parallel.h>
#include <core/spectrum.h>
#include <math/bounds.h>

namespace platinum
{
    // class Film
    // {
    // public:
    //     Film(const std::string& filename, int width, int height, int channel = 3) : _width(width),
    //         _height(height),
    //         _filename(filename)
    //     {
    //         _img_size = _width * _height;
    //         _framebuffer.resize(_img_size, glm::vec3(0));
    //         _img.GenBuffer(_width, _height, channel);
    //     }
    //     void AddPixelValue(int idx, const glm::vec3& color) { _framebuffer[idx] += color; }
    //     void SetPixel(int idx, const glm::vec3& color) { _framebuffer[idx] = color; }
    //     glm::vec3 GetPixel(int idx) { return _framebuffer[idx]; }
    //     int GetWidth() { return _width; }
    //     int GetHeight() { return _height; }
    //     int GetImageSize() { return _img_size; }
    //     void SaveImage();

    // private:
    //     int _width;
    //     int _height;
    //     int _img_size;
    //     std::vector<glm::vec3> _framebuffer;
    //     Image _img;
    //     std::string _filename;
    // };
    class Film
    {
    public:


        Film(const glm::ivec2& resolution, const Bounds2f& cropWindow,
            std::unique_ptr<Filter> filter, const std::string& filename, float diagonal = 35.f,
            float scale = 1.f, float maxSampleLuminance = Infinity);

        Bounds2i getSampleBounds() const;
        const glm::ivec2 getResolution() const { return m_resolution; }

        std::unique_ptr<FilmTile> getFilmTile(const Bounds2i& sampleBounds);
        void mergeFilmTile(std::unique_ptr<FilmTile> tile);

        void writeImageToFile(float splatScale = 1);

        void setImage(const Spectrum* img) const;
        void addSplat(const glm::vec2& p, Spectrum v);

        void clear();
        
        void initialize();


    private:

        //Note: XYZ is a display independent representation of color,
        //      and this is why we choose to use XYZ color herein.
        struct Pixel
        {
            Pixel()
            {
                m_xyz[0] = m_xyz[1] = m_xyz[2] = m_filterWeightSum = 0;
            }

            float m_xyz[3];				//xyz color of the pixel
            float m_filterWeightSum;	//the sum of filter weight values
            AtomicFloat m_splatXYZ[3]; //unweighted sum of samples splats
            float m_pad;				//unused, ensure sizeof(Pixel) -> 32 bytes
        };

        glm::ivec2 m_resolution; //(width, height)
        std::string m_filename;
        std::unique_ptr<Pixel[]> m_pixels;

        float m_diagonal;
        Bounds2i m_croppedPixelBounds;	//actual rendering window

        std::unique_ptr<Filter> m_filter;
        std::mutex m_mutex;

        //Note: precomputed filter weights table
        static constexpr int filterTableWidth = 16;
        float m_filterTable[filterTableWidth * filterTableWidth];

        float m_scale;
        float m_maxSampleLuminance;

        Pixel& getPixel(const glm::ivec2& p)
        {
            CHECK(InsideExclusive(p, m_croppedPixelBounds));
            int width = m_croppedPixelBounds._p_max.x - m_croppedPixelBounds._p_min.x;
            int index = (p.x - m_croppedPixelBounds._p_min.x) + (p.y - m_croppedPixelBounds._p_min.y) * width;
            return m_pixels[index];
        }

    };

    struct FilmTilePixel
    {
        Spectrum m_contribSum = 0.f;		//sum of the weighted spectrum contributions
        float m_filterWeightSum = 0.f;		//sum of the filter weights
    };

    class FilmTile final
    {
    public:
        // FilmTile Public Methods
        FilmTile(const Bounds2i& pixelBounds, const glm::vec2& filterRadius, const float* filterTable,
            int filterTableSize, float maxSampleLuminance)
            : m_pixelBounds(pixelBounds), m_filterRadius(filterRadius),
            m_invFilterRadius(1 / filterRadius.x, 1 / filterRadius.y),
            m_filterTable(filterTable), m_filterTableSize(filterTableSize),
            m_maxSampleLuminance(maxSampleLuminance)
        {
            m_pixels = std::vector<FilmTilePixel>(glm::max(0, pixelBounds.Area()));
        }

        void addSample(const glm::vec2& pFilm, Spectrum L, float sampleWeight = 1.f)
        {
            if (L.y() > m_maxSampleLuminance)
                L *= m_maxSampleLuminance / L.y();

            // Compute sample's raster bounds
            glm::vec2 pFilmDiscrete = pFilm - glm::vec2(0.5f, 0.5f);
            glm::ivec2 p0 = (glm::ivec2)ceil(pFilmDiscrete - m_filterRadius);
            glm::ivec2 p1 = (glm::ivec2)floor(pFilmDiscrete + m_filterRadius) + glm::ivec2(1, 1);
            p0 = glm::max(p0, m_pixelBounds._p_min);
            p1 = glm::min(p1, m_pixelBounds._p_max);

            // Loop over filter support and add sample to pixel arrays

            // Precompute $x$ and $y$ filter table offsets
            int* ifx = new int[p1.x - p0.x];
            for (int x = p0.x; x < p1.x; ++x)
            {
                float fx = glm::abs((x - pFilmDiscrete.x) * m_invFilterRadius.x * m_filterTableSize);
                ifx[x - p0.x] = glm::min((int)glm::floor(fx), m_filterTableSize - 1);
            }

            int* ify = new int[p1.y - p0.y];
            for (int y = p0.y; y < p1.y; ++y)
            {
                float fy = std::abs((y - pFilmDiscrete.y) * m_invFilterRadius.y * m_filterTableSize);
                ify[y - p0.y] = glm::min((int)std::floor(fy), m_filterTableSize - 1);
            }

            for (int y = p0.y; y < p1.y; ++y)
            {
                for (int x = p0.x; x < p1.x; ++x)
                {
                    // Evaluate filter value at $(x,y)$ pixel
                    int offset = ify[y - p0.y] * m_filterTableSize + ifx[x - p0.x];
                    float filterWeight = m_filterTable[offset];

                    // Update pixel values with filtered sample contribution
                    FilmTilePixel& pixel = getPixel(glm::ivec2(x, y));
                    pixel.m_contribSum += L * sampleWeight * filterWeight;
                    pixel.m_filterWeightSum += filterWeight;
                }
            }
            delete[] ifx;
            delete[]ify;
            ifx=nullptr;
            ify = nullptr;
        }

        FilmTilePixel& getPixel(const glm::ivec2& p)
        {
            CHECK(InsideExclusive(p, m_pixelBounds));
            int width = m_pixelBounds._p_max.x - m_pixelBounds._p_min.x;
            int index = (p.x - m_pixelBounds._p_min.x) + (p.y - m_pixelBounds._p_min.y) * width;
            return m_pixels[index];
        }

        const FilmTilePixel& getPixel(const glm::ivec2& p) const
        {
            CHECK(InsideExclusive(p, m_pixelBounds));
            int width = m_pixelBounds._p_max.x - m_pixelBounds._p_min.x;
            int index = (p.x - m_pixelBounds._p_min.x) + (p.y - m_pixelBounds._p_min.y) * width;
            return m_pixels[index];
        }

        Bounds2i getPixelBounds() const { return m_pixelBounds; }

    private:
        const Bounds2i m_pixelBounds;
        const glm::vec2 m_filterRadius, m_invFilterRadius;
        const float* m_filterTable;
        const int m_filterTableSize;
        std::vector<FilmTilePixel> m_pixels;
        const float m_maxSampleLuminance;

        friend class Film;
    };
}

#endif