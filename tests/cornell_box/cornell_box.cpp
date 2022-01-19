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
#include <core/scene.h>
#include <core/integrator.h>
#include <core/primitive.h>
#include <core/light.h>
#include <accelerator/linear.h>
#include <camera/perspective.h>
#include <integrator/whitted_integrator.h>
#include <light/diffuse_light.h>
#include <material/matte.h>

using namespace platinum;
using namespace glm;
using namespace std;

const static string root_path(ROOT_PATH);
const static string assets_path = root_path + "/assets/";

void CreateScene(vector<Primitive>& prim, vector<Light>& light) {
    
    
}
int main() {

}
