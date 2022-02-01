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
#include <shape/triangle.h>

using namespace platinum;
using namespace glm;
using namespace std;

const static string root_path(ROOT_PATH);
const static string assets_path = root_path + "/assets/models/cornellbox/";
static const string file_name = "cornell_box.png";

void AddMesh(vector<std::shared_ptr<Primitive>> &primitives, vector<shared_ptr<Light>> &lights, Transform *obj2world, Transform *world2obj, Material *material, TriangleMesh *mesh, bool isLight = false)
{
    const auto &meshIndices = mesh->GetIndices();
    for (size_t i = 0; i < meshIndices.size(); i += 3)
    {
        std::array<int, 3> indices;
        indices[0] = meshIndices[i + 0];
        indices[1] = meshIndices[i + 1];
        indices[2] = meshIndices[i + 2];
        auto triangle = std::make_shared<Triangle>(obj2world, world2obj, indices, mesh);
        shared_ptr<Light> area_light = nullptr;
        if (isLight)
        {
            area_light = make_shared<DiffuseAreaLight>(*obj2world, Spectrum(5.f, 4.f, 5.f), 8, triangle.get());
            lights.push_back(area_light);
        }

        primitives.push_back(std::make_shared<GeometricPrimitive>(triangle, material, static_pointer_cast<AreaLight>(area_light)));
    }
}
int main()
{
    vector<shared_ptr<Primitive>> primitives;
    vector<shared_ptr<Light>> lights;

    Transform obj2world;
    Transform world2obj{obj2world.GetInverseMatrix()};
    auto floor = make_unique<TriangleMesh>(&obj2world, assets_path + "cbox_floor.obj");
    auto ceiling = make_unique<TriangleMesh>(&obj2world, assets_path + "cbox_ceiling.obj");
    auto back = make_unique<TriangleMesh>(&obj2world, assets_path + "cbox_back.obj");
    auto greenwall= make_unique<TriangleMesh>(&obj2world, assets_path + "cbox_greenwall.obj");
    auto redwall = make_unique<TriangleMesh>(&obj2world, assets_path + "cbox_redwall.obj");
    auto smallbox = make_unique<TriangleMesh>(&obj2world, assets_path + "cbox_smallbox.obj");
    auto largebox = make_unique<TriangleMesh>(&obj2world, assets_path + "cbox_largebox.obj");

    auto light = make_unique<TriangleMesh>(&obj2world, assets_path + "cbox_luminaire.obj");

    auto red = make_unique<Matte>((0.63f, 0.05f, 0.05f));
    auto green = make_unique<Matte>((0.12f, 0.45f, 0.15f));
    auto gray = make_unique<Matte>((0.73f, 0.73f, 0.73f));
    auto blue = make_unique<Matte>((0.1f, 0.1f, 0.73f));
    auto cube = make_unique<Matte>((1.0f, 1.0f, 1.0f));

    AddMesh(primitives, lights, &obj2world, &world2obj, gray.get(), floor.get());
    AddMesh(primitives, lights, &obj2world, &world2obj, gray.get(), ceiling.get());
    AddMesh(primitives, lights, &obj2world, &world2obj, gray.get(), back.get());
    AddMesh(primitives, lights, &obj2world, &world2obj, green.get(), greenwall.get());
    AddMesh(primitives, lights, &obj2world, &world2obj, red.get(), redwall.get());

    AddMesh(primitives, lights, &obj2world, &world2obj, gray.get(), smallbox.get());
    AddMesh(primitives, lights, &obj2world, &world2obj, gray.get(), largebox.get());

    AddMesh(primitives, lights, &obj2world, &world2obj, gray.get(), light.get(), true);

    glm::vec3 eye{278,273,-799};
    glm::vec3 focus{278, 273, -800};
    glm::vec3 up{0, 1, 0};

    Transform camera2world = Inverse(LookAt(eye, focus, up));

    unique_ptr<Filter> filter = make_unique<BoxFilter>(glm::vec2{0.5f, 0.5f});
    auto film = make_shared<Film>(glm::ivec2{666, 500}, Bounds2f{{0, 0}, {1, 1}}, std::move(filter), file_name);

    std::shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(camera2world,39,film);

    std::shared_ptr<Sampler> sampler = make_shared<RandomSampler>(4);

    unique_ptr<Integrator> integrator = make_unique<WhittedIntegrator>(camera, sampler, 10);

    std::shared_ptr<Aggregate> aggre = make_shared<LinearAggregate>(primitives);

    auto scene = make_shared<Scene>(aggre, lights);

    integrator->Render(*scene);

}
