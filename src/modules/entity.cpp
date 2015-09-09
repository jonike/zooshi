// Copyright 2015 Google Inc. All rights reserved.
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

#include "modules/entity.h"

#include "event/event_system.h"
#include "event/base_node.h"
#include "mathfu/glsl_mappings.h"
#include "entity/entity_manager.h"
#include "components/services.h"
#include "component_library/meta.h"

namespace fpl {
namespace fpl_project {

// Returns the entity representing the player.
class PlayerEntityNode : public event::BaseNode {
 public:
  PlayerEntityNode(ServicesComponent* services_component)
      : services_component_(services_component) {}

  static void OnRegister(event::NodeSignature* node_sig) {
    node_sig->AddOutput<entity::EntityRef>();
  }

  virtual void Initialize(event::Inputs* /*in*/, event::Outputs* out) {
    out->Set(0, services_component_->player_entity());
  }

 private:
  ServicesComponent* services_component_;
};

// Returns the entity representing the raft.
class RaftEntityNode : public event::BaseNode {
 public:
  RaftEntityNode(ServicesComponent* services_component)
      : services_component_(services_component) {}

  static void OnRegister(event::NodeSignature* node_sig) {
    node_sig->AddOutput<entity::EntityRef>();
  }

  virtual void Initialize(event::Inputs* /*in*/, event::Outputs* out) {
    out->Set(0, services_component_->raft_entity());
  }

 private:
  ServicesComponent* services_component_;
};

// Given an input string, return the named entity.
class EntityNode : public event::BaseNode {
 public:
  EntityNode(component_library::MetaComponent* meta_component)
      : meta_component_(meta_component) {}

  static void OnRegister(event::NodeSignature* node_sig) {
    node_sig->AddInput<std::string>();
    node_sig->AddOutput<entity::EntityRef>();
  }

  virtual void Execute(event::Inputs* in, event::Outputs* out) {
    auto entity_id = in->Get<std::string>(0);
    entity::EntityRef entity =
        meta_component_->GetEntityFromDictionary(entity_id->c_str());
    assert(entity.IsValid());
    out->Set(0, entity);
  }

 private:
  component_library::MetaComponent* meta_component_;
};

void InitializeEntityModule(event::EventSystem* event_system,
                            ServicesComponent* services_component,
                            component_library::MetaComponent* meta_component) {
  auto EntityCtor = [meta_component]() {
    return new EntityNode(meta_component);
  };
  auto PlayerEntityCtor = [services_component]() {
    return new PlayerEntityNode(services_component);
  };
  auto RaftEntityCtor = [services_component]() {
    return new RaftEntityNode(services_component);
  };
  event::Module* module = event_system->AddModule("entity");
  module->RegisterNode<EntityNode>("entity", EntityCtor);
  module->RegisterNode<PlayerEntityNode>("player_entity", PlayerEntityCtor);
  module->RegisterNode<RaftEntityNode>("raft_entity", RaftEntityCtor);
}

}  // fpl_project
}  // fpl