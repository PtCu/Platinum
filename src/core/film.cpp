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
#include <core/film.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb/stb_image_write.h>
namespace platinum
{

    Film::Film(const glm::ivec2 &resolution, const Bounds2f &cropWindow, std::unique_ptr<Filter> filter,
               const std::string &filename, float Diagonal, float scale, float maxSampleLuminance)
        : m_resolution(resolution), m_filter(std::move(filter)), m_diagonal(Diagonal),
          m_filename(filename), m_scale(scale), m_maxSampleLuminance(maxSampleLuminance)
    {
        //Compute film image bounds
        //Note: cropWindow range [0,1]x[0,1]
        m_croppedPixelBounds =
            Bounds2i(
                glm::ivec2(glm::ceil(m_resolution.x * cropWindow._p_min.x),
                           glm::ceil(m_resolution.y * cropWindow._p_min.y)),
                glm::ivec2(glm::ceil(m_resolution.x * cropWindow._p_max.x),
                           glm::ceil(m_resolution.y * cropWindow._p_max.y)));
        LOG(INFO) << "Created film with full resolution " << resolution << ". Crop window of " << cropWindow << " -> croppedPixelBounds " << m_croppedPixelBounds;

        initialize();
    }

    void Film::initialize()
    {
        m_pixels = std::unique_ptr<Pixel[]>(new Pixel[m_croppedPixelBounds.Area()]);

        //Precompute filter weight table
        //Note: we assume that filtering function f(x,y)=f(|x|,|y|)
        //      hence only store values for the positive quadrant of filter offsets.
        int offset = 0;
        for (int y = 0; y < filterTableWidth; ++y)
        {
            for (int x = 0; x < filterTableWidth; ++x, ++offset)
            {
                glm::vec2 p;
                p.x = (x + 0.5f) * m_filter->m_radius.x / filterTableWidth;
                p.y = (y + 0.5f) * m_filter->m_radius.y / filterTableWidth;
                m_filterTable[offset] = m_filter->evaluate(p);
            }
        }
    }

    Bounds2i Film::getSampleBounds() const
    {
        Bounds2f floatBounds(
            floor(glm::vec2(m_croppedPixelBounds._p_min) + glm::vec2(0.5f, 0.5f) - m_filter->m_radius),
            ceil(glm::vec2(m_croppedPixelBounds._p_max) - glm::vec2(0.5f, 0.5f) + m_filter->m_radius));
        return (Bounds2i)floatBounds;
    }

    std::unique_ptr<FilmTile> Film::getFilmTile(const Bounds2i &sampleBounds)
    {
        // Bound image pixels that samples in _sampleBounds_ contribute to
        glm::vec2 halfPixel = glm::vec2(0.5f, 0.5f);
        Bounds2f floatBounds = (Bounds2f)sampleBounds;
        glm::ivec2 p0 = (glm::ivec2)ceil(floatBounds._p_min - halfPixel - m_filter->m_radius);
        glm::ivec2 p1 = (glm::ivec2)floor(floatBounds._p_max - halfPixel + m_filter->m_radius) + glm::ivec2(1, 1);
        Bounds2i tilePixelBounds = Intersect(Bounds2i(p0, p1), m_croppedPixelBounds);
        return std::unique_ptr<FilmTile>(new FilmTile(tilePixelBounds, m_filter->m_radius,
                                                      m_filterTable, filterTableWidth, m_maxSampleLuminance));
    }

    void Film::mergeFilmTile(std::unique_ptr<FilmTile> tile)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (glm::ivec2 pixel : tile->getPixelBounds())
        {
            // Merge _pixel_ into _Film::pixels_
            const FilmTilePixel &tilePixel = tile->getPixel(pixel);
            Pixel &mergePixel = getPixel(pixel);
            float xyz[3];
            tilePixel.m_contribSum.toXYZ(xyz);
            for (int i = 0; i < 3; ++i)
            {
                mergePixel.m_xyz[i] += xyz[i];
            }
            mergePixel.m_filterWeightSum += tilePixel.m_filterWeightSum;
        }
    }

    void Film::writeImageToFile(float splatScale)
    {
        LOG(INFO) << "Converting image to RGB and computing final weighted pixel values";
        std::unique_ptr<float[]> rgb(new float[3 * m_croppedPixelBounds.Area()]);
        std::unique_ptr<Byte[]> dst(new Byte[3 * m_croppedPixelBounds.Area()]);
        int offset = 0;
        for (glm::ivec2 p : m_croppedPixelBounds)
        {
            // Convert pixel XYZ color to RGB
            Pixel &pixel = getPixel(p);
            XYZToRGB(pixel.m_xyz, &rgb[3 * offset]);

            // Normalize pixel with weight sum
            float filterWeightSum = pixel.m_filterWeightSum;
            if (filterWeightSum != 0)
            {
                float invWt = (float)1 / filterWeightSum;
                rgb[3 * offset + 0] = glm::max((float)0, rgb[3 * offset + 0] * invWt);
                rgb[3 * offset + 1] = glm::max((float)0, rgb[3 * offset + 1] * invWt);
                rgb[3 * offset + 2] = glm::max((float)0, rgb[3 * offset + 2] * invWt);
            }

            // Add splat value at pixel
            float splatRGB[3];
            float splatXYZ[3] = {pixel.m_splatXYZ[0], pixel.m_splatXYZ[1], pixel.m_splatXYZ[2]};
            XYZToRGB(splatXYZ, splatRGB);
            rgb[3 * offset + 0] += splatScale * splatRGB[0];
            rgb[3 * offset + 1] += splatScale * splatRGB[1];
            rgb[3 * offset + 2] += splatScale * splatRGB[2];

            // Scale pixel value by _scale_
            rgb[3 * offset + 0] *= m_scale;
            rgb[3 * offset + 1] *= m_scale;
            rgb[3 * offset + 2] *= m_scale;

#define TO_BYTE(v) (uint8_t) clamp(255.f * gammaCorrect(v) + 0.5f, 0.f, 255.f)
            dst[3 * offset + 0] = TO_BYTE(rgb[3 * offset + 0]);
            dst[3 * offset + 1] = TO_BYTE(rgb[3 * offset + 1]);
            dst[3 * offset + 2] = TO_BYTE(rgb[3 * offset + 2]);

            ++offset;
        }

        LOG(INFO) << "Writing image " << m_filename << " with bounds " << m_croppedPixelBounds;
        auto extent = m_croppedPixelBounds.Diagonal();
        stbi_write_png(m_filename.c_str(),
                       extent.x,
                       extent.y,
                       3,
                       static_cast<void *>(dst.get()),
                       extent.x * 3);
    }

    void Film::setImage(const Spectrum *img) const
    {
        int nPixels = m_croppedPixelBounds.Area();
        for (int i = 0; i < nPixels; ++i)
        {
            Pixel &p = m_pixels[i];
            img[i].toXYZ(p.m_xyz);
            p.m_filterWeightSum = 1;
            p.m_splatXYZ[0] = p.m_splatXYZ[1] = p.m_splatXYZ[2] = 0;
        }
    }

    void Film::addSplat(const glm::vec2 &p, Spectrum v)
    {
        //Note:Rather than computing the final pixel value as a weighted
        //     average of contributing splats, splats are simply summed.

        if (v.hasNaNs())
        {
            LOG(ERROR) << stringPrintf("Ignoring splatted spectrum with NaN values "
                                       "at (%f, %f)",
                                       p.x, p.y);
            return;
        }
        else if (v.y() < 0.)
        {
            LOG(ERROR) << stringPrintf("Ignoring splatted spectrum with negative "
                                       "luminance %f at (%f, %f)",
                                       v.y(), p.x, p.y);
            return;
        }
        else if (glm::isinf(v.y()))
        {
            LOG(ERROR) << stringPrintf("Ignoring splatted spectrum with infinite "
                                       "luminance at (%f, %f)",
                                       p.x, p.y);
            return;
        }

        glm::ivec2 pi = glm::ivec2(floor(p));
        if (!InsideExclusive(pi, m_croppedPixelBounds))
            return;

        if (v.y() > m_maxSampleLuminance)
        {
            v *= m_maxSampleLuminance / v.y();
        }

        float xyz[3];
        v.toXYZ(xyz);

        Pixel &pixel = getPixel(pi);
        for (int i = 0; i < 3; ++i)
        {
            pixel.m_splatXYZ[i].add(xyz[i]);
        }
    }

    void Film::clear()
    {
        for (glm::ivec2 p : m_croppedPixelBounds)
        {
            Pixel &pixel = getPixel(p);
            for (int c = 0; c < 3; ++c)
            {
                pixel.m_splatXYZ[c] = pixel.m_xyz[c] = 0;
            }
            pixel.m_filterWeightSum = 0;
        }
    }
}