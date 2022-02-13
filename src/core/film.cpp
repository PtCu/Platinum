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

    Film::Film(const PropertyNode &root)
    {
        auto iter = root.get_child("Resolution").begin();
        _resolution[0] = (iter++)->second.get_value<float>();
        _resolution[1] = iter->second.get_value<float>();
        iter = root.get_child("CropMin").begin();
        Bounds2f cropWindow;
        cropWindow._p_min[0] = (iter++)->second.get_value<float>();
        cropWindow._p_min[1] = iter->second.get_value<float>();
        iter = root.get_child("CropMax").begin();
        cropWindow._p_max[0] = (iter++)->second.get_value<float>();
        cropWindow._p_max[1] = (iter++)->second.get_value<float>();

        _cropped_pixel_bounds =
            Bounds2i(
                Vector2i(glm::ceil(_resolution.x * cropWindow._p_min.x),
                         glm::ceil(_resolution.y * cropWindow._p_min.y)),
                Vector2i(glm::ceil(_resolution.x * cropWindow._p_max.x),
                         glm::ceil(_resolution.y * cropWindow._p_max.y)));
        LOG(INFO) << "Created film with full resolution " << _resolution << ". Crop window of " << cropWindow << " -> croppedPixelBounds " << _cropped_pixel_bounds;

        _filename = root.get<std::string>("Filename");

        _filter = UPtr<Filter>(static_cast<Filter *>(ObjectFactory::CreateInstance(root.get<std::string>("Filter.Type"), root.get_child("Filter"))));

        Initialize();
    }

    Film::Film(const Vector2i &resolution, const Bounds2f &cropWindow, std::unique_ptr<Filter> filter,
               const std::string &filename, float Diagonal, float scale, float maxSampleLuminance)
        : _resolution(resolution), _filter(std::move(filter)), _diagonal(Diagonal),
          _filename(filename), _scale(scale), _max_sample_luminance(maxSampleLuminance)
    {
        //Compute film image bounds
        //Note: cropWindow range [0,1]x[0,1]
        _cropped_pixel_bounds =
            Bounds2i(
                Vector2i(glm::ceil(_resolution.x * cropWindow._p_min.x),
                         glm::ceil(_resolution.y * cropWindow._p_min.y)),
                Vector2i(glm::ceil(_resolution.x * cropWindow._p_max.x),
                         glm::ceil(_resolution.y * cropWindow._p_max.y)));
        LOG(INFO) << "Created film with full resolution " << resolution << ". Crop window of " << cropWindow << " -> croppedPixelBounds " << _cropped_pixel_bounds;

        Initialize();
    }

    void Film::Initialize()
    {
        _pixels = std::unique_ptr<Pixel[]>(new Pixel[_cropped_pixel_bounds.Area()]);

        //Precompute filter weight table
        //Note: we assume that filtering function f(x,y)=f(|x|,|y|)
        //      hence only store values for the positive quadrant of filter offsets.
        int offset = 0;
        for (int y = 0; y < filter_table_width; ++y)
        {
            for (int x = 0; x < filter_table_width; ++x, ++offset)
            {
                Vector2f p;
                p.x = (x + 0.5f) * _filter->_radius.x / filter_table_width;
                p.y = (y + 0.5f) * _filter->_radius.y / filter_table_width;
                _filter_table[offset] = _filter->Evaluate(p);
            }
        }
    }

    Bounds2i Film::GetSampleBounds() const
    {
        Bounds2f floatBounds(
            floor(Vector2f(_cropped_pixel_bounds._p_min) + Vector2f(0.5f, 0.5f) - _filter->_radius),
            ceil(Vector2f(_cropped_pixel_bounds._p_max) - Vector2f(0.5f, 0.5f) + _filter->_radius));
        return (Bounds2i)floatBounds;
    }

    std::unique_ptr<FilmTile> Film::GetFilmTile(const Bounds2i &sampleBounds)
    {
        // Bound image pixels that samples in _sampleBounds_ contribute to
        Vector2f halfPixel = Vector2f(0.5f, 0.5f);
        Bounds2f floatBounds = (Bounds2f)sampleBounds;
        Vector2i p0 = (Vector2i)ceil(floatBounds._p_min - halfPixel - _filter->_radius);
        Vector2i p1 = (Vector2i)floor(floatBounds._p_max - halfPixel + _filter->_radius) + Vector2i(1, 1);
        Bounds2i tilePixelBounds = Intersect(Bounds2i(p0, p1), _cropped_pixel_bounds);
        return std::unique_ptr<FilmTile>(new FilmTile(tilePixelBounds, _filter->_radius,
                                                      _filter_table, filter_table_width, _max_sample_luminance));
    }

    void Film::MergeFilmTile(std::unique_ptr<FilmTile> tile)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        for (Vector2i pixel : tile->getPixelBounds())
        {
            // Merge _pixel_ into _Film::pixels_
            const FilmTilePixel &tilePixel = tile->getPixel(pixel);
            Pixel &mergePixel = GetPixel(pixel);
            float xyz[3];
            tilePixel.m_contribSum.toXYZ(xyz);
            for (int i = 0; i < 3; ++i)
            {
                mergePixel._xyz[i] += xyz[i];
            }
            mergePixel._filter_weight_sum += tilePixel.m_filterWeightSum;
        }
    }

    void Film::WriteImageToFile(float splatScale)
    {
        LOG(INFO) << "Converting image to RGB and computing final weighted pixel values";
        std::unique_ptr<float[]> rgb(new float[3 * _cropped_pixel_bounds.Area()]);
        std::unique_ptr<Byte[]> dst(new Byte[3 * _cropped_pixel_bounds.Area()]);
        int offset = 0;
        for (Vector2i p : _cropped_pixel_bounds)
        {
            // Convert pixel XYZ color to RGB
            Pixel &pixel = GetPixel(p);
            XYZToRGB(pixel._xyz, &rgb[3 * offset]);

            // Normalize pixel with weight sum
            float filterWeightSum = pixel._filter_weight_sum;
            if (filterWeightSum != 0)
            {
                float invWt = (float)1 / filterWeightSum;
                rgb[3 * offset + 0] = glm::max((float)0, rgb[3 * offset + 0] * invWt);
                rgb[3 * offset + 1] = glm::max((float)0, rgb[3 * offset + 1] * invWt);
                rgb[3 * offset + 2] = glm::max((float)0, rgb[3 * offset + 2] * invWt);
            }

            // Add splat value at pixel
            float splatRGB[3];
            float splatXYZ[3] = {pixel._splatXYZ[0], pixel._splatXYZ[1], pixel._splatXYZ[2]};
            XYZToRGB(splatXYZ, splatRGB);
            rgb[3 * offset + 0] += splatScale * splatRGB[0];
            rgb[3 * offset + 1] += splatScale * splatRGB[1];
            rgb[3 * offset + 2] += splatScale * splatRGB[2];

            // Scale pixel value by _scale_
            rgb[3 * offset + 0] *= _scale;
            rgb[3 * offset + 1] *= _scale;
            rgb[3 * offset + 2] *= _scale;

#define TO_BYTE(v) (uint8_t) clamp(255.f * gammaCorrect(v) + 0.5f, 0.f, 255.f)
            dst[3 * offset + 0] = TO_BYTE(rgb[3 * offset + 0]);
            dst[3 * offset + 1] = TO_BYTE(rgb[3 * offset + 1]);
            dst[3 * offset + 2] = TO_BYTE(rgb[3 * offset + 2]);

            ++offset;
        }

        LOG(INFO) << "Writing image " << _filename << " with bounds " << _cropped_pixel_bounds;
        auto extent = _cropped_pixel_bounds.Diagonal();
        stbi_write_png(_filename.c_str(),
                       extent.x,
                       extent.y,
                       3,
                       static_cast<void *>(dst.get()),
                       extent.x * 3);
    }

    void Film::SetImage(const Spectrum *img) const
    {
        int nPixels = _cropped_pixel_bounds.Area();
        for (int i = 0; i < nPixels; ++i)
        {
            Pixel &p = _pixels[i];
            img[i].toXYZ(p._xyz);
            p._filter_weight_sum = 1;
            p._splatXYZ[0] = p._splatXYZ[1] = p._splatXYZ[2] = 0;
        }
    }

    void Film::AddSplat(const Vector2f &p, Spectrum v)
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

        Vector2i pi = Vector2i(floor(p));
        if (!InsideExclusive(pi, _cropped_pixel_bounds))
            return;

        if (v.y() > _max_sample_luminance)
        {
            v *= _max_sample_luminance / v.y();
        }

        float xyz[3];
        v.toXYZ(xyz);

        Pixel &pixel = GetPixel(pi);
        for (int i = 0; i < 3; ++i)
        {
            pixel._splatXYZ[i].add(xyz[i]);
        }
    }

    void Film::Clear()
    {
        for (Vector2i p : _cropped_pixel_bounds)
        {
            Pixel &pixel = GetPixel(p);
            for (int c = 0; c < 3; ++c)
            {
                pixel._splatXYZ[c] = pixel._xyz[c] = 0;
            }
            pixel._filter_weight_sum = 0;
        }
    }
}