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

#include "cornell_box.h"
#include "filter/box_filter.h"
#include "sampler/random_sampler.h"
static const string file_name = "cornell_box";
int main()
{
    vector<shared_ptr<Primitive>> primitives;
    vector<shared_ptr<Light>> lights;

    Transform obj2world;
    Transform world2obj{obj2world.GetInverseMatrix()};
    auto floor = make_unique<TriangleMesh>(&obj2world, assets_path + "floor.obj");
    auto left = make_unique<TriangleMesh>(&obj2world, assets_path + "left.obj");
    auto right = make_unique<TriangleMesh>(&obj2world, assets_path + "right.obj");
    auto shortbox = make_unique<TriangleMesh>(&obj2world, assets_path + "shortbox.obj");
    auto tallbox = make_unique<TriangleMesh>(&obj2world, assets_path + "tallbox.obj");
    auto light = make_unique<TriangleMesh>(&obj2world, assets_path + "light.obj");

    auto red = make_unique<Matte>((0.63f, 0.065f, 0.05f));
    auto green = make_unique<Matte>((0.14f, 0.45f, 0.091f));
    auto gray = make_unique<Matte>((0.725f, 0.71f, 0.68f));
    auto blue = make_unique<Matte>((0.1f, 0.1f, 0.73f));
    auto cube = make_unique<Matte>((1.0f, 1.0f, 1.0f));

    AddMesh(primitives, lights, &obj2world, &world2obj, gray.get(), floor.get());
    AddMesh(primitives, lights, &obj2world, &world2obj, cube.get(), shortbox.get());
    AddMesh(primitives, lights, &obj2world, &world2obj, cube.get(), tallbox.get());
    AddMesh(primitives, lights, &obj2world, &world2obj, red.get(), left.get());
    AddMesh(primitives, lights, &obj2world, &world2obj, green.get(), right.get());
    AddMesh(primitives, lights, &obj2world, &world2obj, gray.get(), light.get(), true);

    glm::vec3 eye{278,273,-800};
    glm::vec3 focus{278, 273, -799};
    glm::vec3 up{0, 0, 1};
    Transform camera2world = Inverse(LookAt(eye, focus, up));

    unique_ptr<Filter> filter = make_unique<BoxFilter>(glm::vec2{0.5f, 0.5f});
    auto film = make_shared<Film>(glm::ivec2{600, 550}, Bounds2f{{0, 0}, {1, 1}}, std::move(filter), file_name);

    std::shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(camera2world,39,film);

    std::shared_ptr<Sampler> sampler = make_shared<RandomSampler>(4);

    unique_ptr<Integrator> integrator = make_unique<WhittedIntegrator>(camera, sampler, 10);

    std::shared_ptr<Aggregate> aggre = make_shared<LinearAggregate>(primitives);

    auto scene = make_shared<Scene>(aggre, lights);
    
    integrator->Render(*scene);

}
