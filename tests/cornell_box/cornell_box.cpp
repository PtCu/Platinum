// // Copyright 2022 ptcup
// //
// // Licensed under the Apache License, Version 2.0 (the "License");
// // you may not use this file except in compliance with the License.
// // You may obtain a copy of the License at
// //
// //     http://www.apache.org/licenses/LICENSE-2.0
// //
// // Unless required by applicable law or agreed to in writing, software
// // distributed under the License is distributed on an "AS IS" BASIS,
// // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// // See the License for the specific language governing permissions and
// // limitations under the License.
// #include <ROOT_PATH.h>
// #include <core/scene.h>
// #include <core/integrator.h>
// #include <core/primitive.h>
// #include <core/light.h>
// #include <accelerator/linear.h>
// #include <camera/perspective.h>
// #include <integrator/whitted_integrator.h>
// #include <light/diffuse_light.h>
// #include <material/matte.h>
// #include <shape/triangle.h>

// using namespace platinum;
// using namespace glm;
// using namespace std;

// const static string root_path(ROOT_PATH);
// const static string assets_path = root_path + "/assets/";

// void AddMesh(vector<std::shared_ptr<Primitive>> &primitives, const Transform &obj2world, const Transform &world2obj, Material *material, TriangleMesh *mesh,AreaLight* arealight=nullptr)
// {
//     const auto &meshIndices = mesh->GetIndices();
//     for (size_t i = 0; i < meshIndices.size(); i += 3)
//     {
//         std::array<int, 3> indices;
//         indices[0] = meshIndices[i + 0];
//         indices[1] = meshIndices[i + 1];
//         indices[2] = meshIndices[i + 2];
//         auto triangle = std::make_shared<Triangle>(&obj2world, &world2obj, indices, mesh);

//         primitives.push_back(std::make_shared<Primitive>(triangle, material, nullptr));
//     }
// }

// void CreateScene(vector<std::shared_ptr<Primitive>> &primitives)
// {
//     Transform obj2world;
//     Transform world2obj{obj2world.GetInverseMatrix()};
//     auto floor = make_unique<TriangleMesh>(obj2world, "floor.obj");
//     auto left = make_unique<TriangleMesh>(obj2world, "left.obj");
//     auto right = make_unique<TriangleMesh>(obj2world, "right.obj");
//     auto shortbox = make_unique<TriangleMesh>(obj2world, "shortbox.obj");
//     auto tallbox = make_unique<TriangleMesh>(obj2world, "tallbox.obj");
//     auto light = make_unique<TriangleMesh>(obj2world, "light.obj");

//     auto red = make_unique<Matte>(vec3(0.63f, 0.065f, 0.05f));
//     auto green = make_unique<Matte>(vec3(0.14f, 0.45f, 0.091f));
//     auto gray = make_unique<Matte>(vec3(0.725f, 0.71f, 0.68f));
//     auto blue = make_unique<Matte>(vec3(0.1f, 0.1f, 0.73f));
//     auto cube = make_unique<Matte>(vec3(1.0f, 1.0f, 1.0f));


//     AddMesh(primitives, obj2world, world2obj, gray.get(),floor.get());
//     AddMesh(primitives, obj2world, world2obj, cube.get(), shortbox.get());
//     AddMesh(primitives, obj2world, world2obj, cube.get(), tallbox.get());
//     AddMesh(primitives, obj2world, world2obj, red.get(), left.get());
//     AddMesh(primitives, obj2world, world2obj, green.get(), right.get());
//     AddMesh(primitives, obj2world, world2obj, gray.get(), light.get());
// }
int main()
{
}
