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

#include <ROOT_PATH.h>
#include "filter/box_filter.h"
#include "sampler/random_sampler.h"
#include <core/scene.h>
#include <core/integrator.h>
#include <core/primitive.h>
#include <core/light.h>
#include <accelerator/linear.h>
#include <camera/perspective.h>
#include <integrator/whitted_integrator.h>
#include <light/diffuse_light.h>
#include <material/matte.h>
#include <shape/sphere.h>

using namespace platinum;
using namespace glm;
using namespace std;

const static string root_path(ROOT_PATH);
const static string assets_path = root_path + "/assets/models/cornellbox/";
static const string file_name = "sphere.png";

int main()
{
    vector<shared_ptr<Primitive>> primitives;
    vector<shared_ptr<Light>> lights;

    // float sph[16] = {5, 0, 0, 25, 0, 5, 0, 15, 0, 0, 5, 20, 0, 0, 0, 1};
    //Transform obj2world(sph);
    Transform obj2world;
    Transform world2obj{obj2world.GetInverseMatrix()};

    auto sphere = make_shared<Sphere>(&obj2world, &world2obj, 1.f);
    shared_ptr<Light> area_light = make_shared<DiffuseAreaLight>(obj2world, Spectrum(5.f, 4.f, 0.f), 8, sphere.get());

    shared_ptr<Material> material = make_shared<Matte>((0.63f, 0.05f, 0.05f));

    primitives.push_back(std::make_shared<GeometricPrimitive>(sphere, material.get(), static_pointer_cast<AreaLight>(area_light)));

    Vector3f eye{5, 5, 5};

    Vector3f focus{0, 0, 0};

    Vector3f up{0, 1, 0};

    Transform camera2world = Inverse(LookAt(eye, focus, up));

    unique_ptr<Filter> filter = make_unique<BoxFilter>(Vector2f{0.5f, 0.5f});
    auto film = make_shared<Film>(Vector2i{600, 500}, Bounds2f{{0, 0}, {1, 1}}, std::move(filter), file_name);

    Ptr<Camera> camera = make_shared<PerspectiveCamera>(camera2world, 45, film);

    Ptr<Sampler> sampler = make_shared<RandomSampler>(4);

    unique_ptr<Integrator> integrator = make_unique<WhittedIntegrator>(camera, sampler, 10);

    Ptr<Aggregate> aggre = make_shared<LinearAggregate>(primitives);

    auto scene = make_shared<Scene>(aggre, lights);

    integrator->Render(*scene);
}
