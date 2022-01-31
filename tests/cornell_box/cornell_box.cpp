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
    CreateScene(primitives, lights);

    glm::vec3 eye{278,273,-800};
    glm::vec3 focus{278, 273, -799};
    glm::vec3 up{0, 0, 1};
    Transform camera2world = Inverse(LookAt(eye, focus, up));

    unique_ptr<Filter> filter = make_unique<BoxFilter>(glm::vec2{0.5f, 0.5f});
    auto film = make_shared<Film>(glm::ivec2{600, 550}, Bounds2f{{0, 0}, {1, 1}}, std::move(filter), file_name);

    std::shared_ptr<Camera> camera = make_shared<PerspectiveCamera>(camera2world,39,film);

    std::shared_ptr<Sampler> sampler = make_shared<RandomSampler>(16);

    unique_ptr<Integrator> integrator = make_unique<WhittedIntegrator>(camera, sampler, 10);

    std::shared_ptr<Aggregate> aggre = make_shared<LinearAggregate>(primitives);

    auto scene = make_shared<Scene>(aggre, lights);
    integrator->Render(*scene);

}
