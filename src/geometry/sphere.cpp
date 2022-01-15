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

#include <geometry/sphere.h>
namespace platinum
{
    void Sphere_::Sample(HitRst &rst, float &pdf) const
    {
        float theta = 2.0f * PI * Random::UniformFloat(), phi = PI * Random::UniformFloat();
        glm::vec3 dir(std::cos(phi), std::sin(phi) * std::cos(theta), std::sin(phi) * std::sin(theta));
        rst.record.vert.position_ = center_ + radius_ * dir;
        rst.record.vert.normal_ = dir;
        pdf = 1.0f / area_;
        rst.emit = _material->Emit();
    }
    float Sphere_::GetArea() const
    {
        return area_;
    }
    AABB Sphere_::GetBoundingBox() const
    {
        return bounding_box_;
    }
    glm::vec3 Sphere_::getCenter(const Ray &r) const
    {
        return center_;
    }
    Sphere_::Sphere_(glm::vec3 cen, float r, std::shared_ptr<Material> m)
        : center_(cen), radius_(r), Object(m)
    {
        glm::vec3 minP = center_ - glm::vec3(radius_);
        glm::vec3 maxP = center_ + glm::vec3(radius_);
        bounding_box_ = AABB(minP, maxP);
        area_ = PI * 4.0f * r * r;
    };
    void Sphere_::setIntersection(float t, HitRst &rst, const Ray &r) const
    {
        rst.record.ray = r;
        rst.record.ray.SetTMax(t);
        rst.record.vert.position_ = r.PointAt(rst.record.ray.GetMaxTime());
        rst.record.vert.normal_ = glm::vec3((rst.record.vert.position_ - getCenter(r)) / radius_);
        getSphereUV(rst.record.vert.normal_, rst.record.vert.u_, rst.record.vert.v_);
        rst.material = GetMaterial();
        rst.is_hit = true;
    }

    HitRst Sphere_::Intersect(const Ray &r)
    {
        HitRst rst;
        glm::vec3 oc = r.GetOrigin() - getCenter(r);
        float a = glm::dot(r.GetDirection(), r.GetDirection());
        float b = glm::dot(oc, r.GetDirection());
        float c = glm::dot(oc, oc) - radius_ * radius_;
        float discriminant = b * b - a * c;
        if (discriminant > 0)
        {
            float temp = (-b - sqrt(discriminant)) / a;
            if (temp < r.GetMaxTime() && temp > r.GetMinTime())
            {
                setIntersection(temp, rst, r);
            }
            temp = (-b + sqrt(discriminant)) / a;
            if (temp < r.GetMaxTime() && temp > r.GetMinTime())
            {
                setIntersection(temp, rst, r);
            }
        }
        return rst;
    }
    //Using the polar coordinates of sphere (phi, theta) to get fractions as u, v.
    void Sphere_::getSphereUV(const glm::vec3 &p, float &u, float &v) const
    {
        float phi = atan2(p.z, p.x);
        float theta = asin(p.y);
        u = 1 - (phi + PI) / (2 * PI);
        v = (theta + PI / 2) / PI;
    }

    AABB Sphere::ObjectBound() const
    {
        return AABB(-glm::vec3(_radius, _radius, _radius), glm::vec3(_radius, _radius, _radius));
    }

    float Sphere::Area() const
    {
        return 4.f * PI * _radius * _radius;
    }

    Interaction Sphere::Sample(const glm::vec2 &u, float &pdf) const
    {
        return {};
    }
    Interaction Sphere::Sample(const Interaction &ref, const glm::vec2 &u, float &pdf) const
    {
        return {};
    }
    float Sphere::Pdf(const Interaction &ref, const glm::vec3 &wi) const
    {
        return 0.f;
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

    bool Sphere::Hit(const Ray &r, float &tHit, SurfaceInteraction &isect) const
    {
        float phi;
        glm::vec3 p_hit;
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
} // namespace platinum
