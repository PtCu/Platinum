// The MIT License (MIT)

// Copyright (c) 2021 PtCu

//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

#include <shape/sphere.h>
#include <core/interaction.h>
#include <math/transform.h>
#include <glm/gtx/norm.hpp>
#include <math/bounds.h>
#include <core/sampler.h>

namespace platinum
{

    REGISTER_CLASS(Sphere, "Sphere");

    Sphere::Sphere(const PropertyTree &node)
    :Shape(node),_radius(node.Get<float>("Radius",1.f))
    {
    }
    
    Bounds3f Sphere::ObjectBound() const
    {
        return Bounds3f(-Vector3f(_radius, _radius, _radius), Vector3f(_radius, _radius, _radius));
    }

    float Sphere::Area() const
    {
        return 4.f * Pi * _radius * _radius;
    }

    Interaction Sphere::Sample(const Vector2f &u, float &pdf) const
    {
        Vector3f pObj = Vector3f(0, 0, 0) + _radius * UniformSampleSphere(u);

        Interaction it;
        it.n = glm::normalize((*_object2world).ExecOn(pObj, 0.0f));

        pObj *= _radius / distance(pObj, Vector3f(0, 0, 0));
        it.p = (*_object2world).ExecOn(pObj, 1.0f);

        pdf = 1 / Area();
        return it;
    }
    Interaction Sphere::Sample(const Interaction &ref, const Vector2f &u, float &pdf) const
    {
        {
            Vector3f pCenter = (*_object2world).ExecOn(Vector3f(0, 0, 0), 1.0f);

            // Sample uniformly on sphere if $\pt{}$ is inside it
            Vector3f pOrigin = ref.p;
            if (glm::distance2(pOrigin, pCenter) <= _radius * _radius)
            {
                Interaction intr = Sample(u, pdf);
                Vector3f wi = intr.p - ref.p;
                if (dot(wi, wi) == 0)
                {
                    pdf = 0;
                }
                else
                {
                    // Convert from Area measure returned by Sample() call above to
                    // solid angle measure.
                    wi = normalize(wi);
                    pdf *= distance2(ref.p, intr.p) / glm::abs(glm::dot(intr.n, -wi));
                }
                if (std::isinf(pdf))
                    pdf = 0.f;
                return intr;
            }

            // Sample sphere uniformly inside subtended cone

            // Compute coordinate system for sphere sampling
            float dc = distance(ref.p, pCenter);
            float invDc = 1 / dc;
            Vector3f wc = (pCenter - ref.p) * invDc;
            Vector3f wcX, wcY;
            coordinateSystem(wc, wcX, wcY);

            // Compute $\theta$ and $\phi$ values for Sample in cone
            float sinThetaMax = _radius * invDc;
            float sinThetaMax2 = sinThetaMax * sinThetaMax;
            float invSinThetaMax = 1 / sinThetaMax;
            float cosThetaMax = glm::sqrt(glm::max((float)0.f, 1.0f - sinThetaMax2));

            float cosTheta = (cosThetaMax - 1) * u[0] + 1;
            float sinTheta2 = 1 - cosTheta * cosTheta;

            if (sinThetaMax2 < 0.00068523f /* sin^2(1.5 deg) */)
            {
                /* Fall back to a Taylor series expansion for small angles, where
			   the standard approach suffers from severe cancellation errors */
                sinTheta2 = sinThetaMax2 * u[0];
                cosTheta = glm::sqrt(1 - sinTheta2);
            }

            // Compute angle $\alpha$ from center of sphere to sampled point on surface
            float cosAlpha = sinTheta2 * invSinThetaMax +
                             cosTheta * glm::sqrt(glm::max((float)0.f, 1.f - sinTheta2 * invSinThetaMax * invSinThetaMax));
            float sinAlpha = glm::sqrt(glm::max((float)0.f, 1.f - cosAlpha * cosAlpha));
            float phi = u[1] * 2 * Pi;

            // Compute surface normal and sampled point on sphere
            Vector3f nWorld = sphericalDirection(sinAlpha, cosAlpha, phi, -wcX, -wcY, -wc);
            Vector3f pWorld = pCenter + _radius * Vector3f(nWorld.x, nWorld.y, nWorld.z);

            // Return _Interaction_ for sampled point on sphere
            Interaction it;
            it.p = pWorld;
            it.n = nWorld;

            // Uniform cone PDF.
            pdf = 1 / (2 * Pi * (1 - cosThetaMax));

            return it;
        }
    }
    float Sphere::Pdf(const Interaction &ref, const Vector3f &wi) const
    {
        Vector3f pCenter = (*_object2world).ExecOn(Vector3f(0, 0, 0), 1.0f);
        // Return uniform PDF if point is inside sphere
        Vector3f pOrigin = ref.p;
        if (distance2(pOrigin, pCenter) <= _radius * _radius)
            return Shape::Pdf(ref, wi);

        // Compute general sphere PDF
        float sinThetaMax2 = _radius * _radius / distance2(ref.p, pCenter);
        float cosThetaMax = glm::sqrt(glm::max((float)0, 1 - sinThetaMax2));
        return UniformConePdf(cosThetaMax);
    }

    bool Sphere::Hit(const Ray &r) const
    {
        //先变化光线到局部坐标中
        Ray ray = _world2object->ExecOn(r);

        float a = glm::dot(ray._direction, ray._direction);
        float b = glm::dot(ray._direction, ray._origin);
        float c = glm::dot(ray._origin, ray._origin) - _radius * _radius;

        float discriminant = b * b - a * c;
        if (discriminant <= 0)
            return false;
        float disc_sqrt = glm::sqrt(discriminant);
        float t0 = (-b - disc_sqrt) / a;
        float t1 = (-b + disc_sqrt) / a;

        if (t0 > t1)
            std::swap(t0, t1);
        //Since t0 is guaranteed to be less than or equal to t1 (and 0 is less than tMax), then if t0 is greater than tMax or t1 is less than 0,
        //it is certain that both intersections are out of the range of interest.
        if (t0 > ray._t_max || t1 <= 0)
            return false;

        float t_shape_hit = t0;
        if (t_shape_hit <= 0)
        {
            t_shape_hit = t1;
            if (t_shape_hit > ray._t_max)
                return false;
        }
        return true;
    }

    bool Sphere::Hit(const Ray &r, float &t_hit, SurfaceInteraction &inter) const
    {

        //先变化光线到局部坐标中
        Ray ray = _world2object->ExecOn(r);

        float a = glm::dot(ray._direction, ray._direction);
        float b = glm::dot(ray._direction, ray._origin);
        float c = glm::dot(ray._origin, ray._origin) - _radius * _radius;

        float discriminant = b * b - a * c;
        if (discriminant <= 0)
            return false;
        float disc_sqrt = glm::sqrt(discriminant);
        float t0 = (-b - disc_sqrt) / a;
        float t1 = (-b + disc_sqrt) / a;

        if (t0 > t1)
            std::swap(t0, t1);
        //Since t0 is guaranteed to be less than or equal to t1 (and 0 is less than tMax), then if t0 is greater than tMax or t1 is less than 0,
        //it is certain that both intersections are out of the range of interest.
        if (t0 > ray._t_max || t1 <= 0)
            return false;

        float t_shape_hit = t0;
        if (t_shape_hit <= 0)
        {
            t_shape_hit = t1;
            if (t_shape_hit > ray._t_max)
                return false;
        }

        Vector3f p_hit = ray.GetPointAt(t_shape_hit);

        // Refine sphere intersection point
        p_hit *= _radius / glm::l2Norm(p_hit);
        if (p_hit.x == 0 && p_hit.y == 0)
            p_hit.x = 1e-5f * _radius;

        //glm::atan调用的还是std::atan2
        float phi = glm::atan(p_hit.y, p_hit.x);

        if (phi < 0)
            phi += 2 * Pi;

        float theta = glm::acos(glm::clamp(p_hit.z / _radius, -1.f, -1.f));

        //Φ的范围为[0,2pi]
        float u = phi / (Pi * 2);
        //θ的范围为[-pi/2,pi/2]
        float v = (theta + PiOver2) / Pi;

        //计算dpdu和dpdv
        float z_sinTheta = glm::sqrt(p_hit.x * p_hit.x + p_hit.y * p_hit.y);
        float inv_z_radius = 1.f / z_sinTheta;
        float cosPhi = p_hit.x * inv_z_radius;
        float sinPhi = p_hit.y * inv_z_radius;

        Vector3f dpdu(-2 * Pi * p_hit.y, 2 * Pi * p_hit.x, 0);
        Vector3f dpdv(p_hit.z * cosPhi, p_hit.z * sinPhi, -_radius * glm::sin(theta));
        dpdv *= 2 * Pi;

        inter = _object2world->ExecOn(SurfaceInteraction(p_hit, Vector2f(u, v), -ray._direction, dpdu, dpdv, this));

        if (glm::dot(inter.n, inter.wo) < 0)
            inter.n = -inter.n;
        t_hit = t_shape_hit;
        return true;
    }

    float Sphere::SolidAngle(const Vector3f &p, int nSamples) const
    {
        Vector3f pCenter = (*_object2world).ExecOn(Vector3f(0, 0, 0), 1.0f);
        if (distance2(p, pCenter) <= _radius * _radius)
            return 4 * Pi;
        float sinTheta2 = _radius * _radius / distance2(p, pCenter);
        float cosTheta = glm::sqrt(glm::max((float)0, 1 - sinTheta2));
        return (2 * Pi * (1 - cosTheta));
    }
} // namespace platinum
