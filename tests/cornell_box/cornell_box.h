
#include <ROOT_PATH.h>
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
            area_light = make_shared<DiffuseAreaLight>(*obj2world, Spectrum(2.f, 2.f, 2.f), 8, triangle.get());
            lights.push_back(area_light);
        }

        primitives.push_back(std::make_shared<GeometricPrimitive>(triangle, material, dynamic_pointer_cast<AreaLight>(area_light)));
    }
}

void CreateScene(vector<shared_ptr<Primitive>> &primitives, vector<shared_ptr<Light>> &lights)
{
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


}