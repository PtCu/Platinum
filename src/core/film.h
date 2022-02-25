

#ifndef CORE_FILM_H_
#define CORE_FILM_H_

#include <core/utilities.h>
#include <core/object.h>

#include <core/spectrum.h>
#include <math/bounds.h>
#include <core/filter.h>
#include <atomic>

namespace platinum
{

    class AtomicFloat
    {
    public:
        explicit AtomicFloat(float v = 0)
        {
            bits = floatToBits(v);
        }

        operator float() const { return bitsToFloat(bits); }

        float operator=(float v)
        {
            bits = floatToBits(v);
            return v;
        }

        void add(float v)
        {

            uint32_t oldBits = bits, newBits;

            do
            {
                newBits = floatToBits(bitsToFloat(oldBits) + v);
            } while (!bits.compare_exchange_weak(oldBits, newBits));
        }

    private:
        std::atomic<uint32_t> bits;
    };

    class Film : public Object
    {
    public:
        Film(const PropertyTree &node);

        Film(const Vector2i &resolution, const Bounds2f &cropWindow,
             std::unique_ptr<Filter> filter, const std::string &filename, float diagonal = 35.f,
             float scale = 1.f, float maxSampleLuminance = Infinity);

        void SetFilter(std::unique_ptr<Filter> filter) { _filter = std::move(filter); }

        Bounds2i GetSampleBounds() const;

        const Vector2i GetResolution() const { return _resolution; }

        std::unique_ptr<FilmTile> GetFilmTile(const Bounds2i &sampleBounds);
        void MergeFilmTile(std::unique_ptr<FilmTile> tile);

        void WriteImageToFile(float splatScale = 1);

        void SetImage(const Spectrum *img) const;

        void AddSplat(const Vector2f &p, Spectrum v);

        void Clear();

        void Initialize();

        virtual std::string ToString() const { return "Film"; }

    private:
        //Note: XYZ is a display independent representation of color,
        //      and this is why we choose to use XYZ color herein.
        struct Pixel
        {
            Pixel()
            {
                _xyz[0] = _xyz[1] = _xyz[2] = _filter_weight_sum = 0;
            }

            float _xyz[3];            //xyz color of the pixel
            float _filter_weight_sum; //the sum of filter weight values
            AtomicFloat _splatXYZ[3]; //unweighted sum of samples splats
            float _pad;               //unused, ensure sizeof(Pixel) -> 32 bytes
        };

        Vector2i _resolution; //(width, height)
        std::string _filename;
        std::unique_ptr<Pixel[]> _pixels;

        float _diagonal;
        Bounds2i _cropped_pixel_bounds; //actual rendering window

        std::unique_ptr<Filter> _filter;
        std::mutex _mutex;

        //Note: precomputed filter weights table
        static constexpr int filter_table_width = 16;
        float _filter_table[filter_table_width * filter_table_width];

        float _scale;
        float _max_sample_luminance;

        Pixel &GetPixel(const Vector2i &p)
        {
            CHECK(InsideExclusive(p, _cropped_pixel_bounds));
            int width = _cropped_pixel_bounds._p_max.x - _cropped_pixel_bounds._p_min.x;
            int index = (p.x - _cropped_pixel_bounds._p_min.x) + (p.y - _cropped_pixel_bounds._p_min.y) * width;
            return _pixels[index];
        }
    };

    struct FilmTilePixel
    {
        Spectrum m_contribSum = 0.f;   //sum of the weighted spectrum contributions
        float m_filterWeightSum = 0.f; //sum of the filter weights
    };

    class FilmTile final
    {
    public:
        // FilmTile Public Methods
        FilmTile(const Bounds2i &pixelBounds, const Vector2f &filterRadius, const float *filterTable,
                 int filterTableSize, float maxSampleLuminance)
            : m_pixelBounds(pixelBounds), m_filterRadius(filterRadius),
              m_invFilterRadius(1 / filterRadius.x, 1 / filterRadius.y),
              m_filterTable(filterTable), m_filterTableSize(filterTableSize),
              m_maxSampleLuminance(maxSampleLuminance)
        {
            m_pixels = std::vector<FilmTilePixel>(glm::max(0, pixelBounds.Area()));
        }

        void AddSample(const Vector2f &pFilm, Spectrum L, float sampleWeight = 1.f)
        {
            if (L.y() > m_maxSampleLuminance)
                L *= m_maxSampleLuminance / L.y();

            // Compute sample's raster bounds
            Vector2f pFilmDiscrete = pFilm - Vector2f(0.5f, 0.5f);
            Vector2i p0 = (Vector2i)ceil(pFilmDiscrete - m_filterRadius);
            Vector2i p1 = (Vector2i)floor(pFilmDiscrete + m_filterRadius) + Vector2i(1, 1);
            p0 = glm::max(p0, m_pixelBounds._p_min);
            p1 = glm::min(p1, m_pixelBounds._p_max);

            // Loop over filter support and add sample to pixel arrays

            // Precompute $x$ and $y$ filter table offsets
            int *ifx = ALLOCA(int, p1.x - p0.x);
            for (int x = p0.x; x < p1.x; ++x)
            {
                float fx = glm::abs((x - pFilmDiscrete.x) * m_invFilterRadius.x * m_filterTableSize);
                ifx[x - p0.x] = glm::min((int)glm::floor(fx), m_filterTableSize - 1);
            }

            int *ify = ALLOCA(int, p1.y - p0.y);
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
                    FilmTilePixel &pixel = getPixel(Vector2i(x, y));
                    pixel.m_contribSum += L * sampleWeight * filterWeight;
                    pixel.m_filterWeightSum += filterWeight;
                }
            }
        }

        FilmTilePixel &getPixel(const Vector2i &p)
        {
            CHECK(InsideExclusive(p, m_pixelBounds));
            int width = m_pixelBounds._p_max.x - m_pixelBounds._p_min.x;
            int index = (p.x - m_pixelBounds._p_min.x) + (p.y - m_pixelBounds._p_min.y) * width;
            return m_pixels[index];
        }

        const FilmTilePixel &getPixel(const Vector2i &p) const
        {
            CHECK(InsideExclusive(p, m_pixelBounds));
            int width = m_pixelBounds._p_max.x - m_pixelBounds._p_min.x;
            int index = (p.x - m_pixelBounds._p_min.x) + (p.y - m_pixelBounds._p_min.y) * width;
            return m_pixels[index];
        }

        Bounds2i getPixelBounds() const { return m_pixelBounds; }

    private:
        const Bounds2i m_pixelBounds;
        const Vector2f m_filterRadius, m_invFilterRadius;
        const float *m_filterTable;
        const int m_filterTableSize;
        std::vector<FilmTilePixel> m_pixels;
        const float m_maxSampleLuminance;

        friend class Film;
    };
}

#endif