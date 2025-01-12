#include "mig15_vs_f86_by_prxor.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

//#include <iostream>
#include <cmath>

namespace battle_game::unit {
namespace {
uint32_t fighter_model_index = 0xffffffffu;
}  // namespace

Fighter::Fighter(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~fighter_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Fighter */
      fighter_model_index = mgr->RegisterModel(
          {
              {{0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.07f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.14f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.14f, 0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{1.0f, -0.1f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{1.0f, -0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.14f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.14f, -0.4f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.05f, -0.75f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.4f, -1.1f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.33f, -1.24f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},

              {{0.0f, -1.1f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              
              {{-0.33f, -1.24f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.4f, -1.1f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.05f, -0.75f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.14f, -0.4f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.14f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-1.0f, -0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-1.0f, -0.1f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.14f, 0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.14f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.07f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
          },//connect the points on by one
          {
              0, 1, 2, 0, 2, 3, 3, 4, 5, 3, 5, 6, 0, 3, 6, 0, 6, 7, 0, 7, 8, 8, 9, 10, 8, 10, 11,
              0, 21, 20, 0, 20, 19, 19, 18, 17, 19, 17, 16, 0, 19, 16, 0, 16, 15, 0, 15, 14, 14, 13, 12, 14, 12, 11,
              0, 11, 8, 0, 11, 14
          });
    }


  }
}

void Fighter::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(fighter_model_index);

}

void Fighter::Update() {
  FighterMove(3.0f, glm::radians(180.0f));
  Fire();
}

void Fighter::FighterMove(float move_speed, float rotate_angular_speed) {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    if (input_data.key_down[GLFW_KEY_W]) {
      current_speed_.y += 0.01f;
      current_speed_.y = std::min(current_speed_.y, max_speed_);
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      current_speed_.y -= 0.01f;
      current_speed_.y = std::max(current_speed_.y, 0.0f);
    }
    float speed = move_speed * GetSpeedScale();
    glm::vec2 offset{0.0f};
    offset = current_speed_ * kSecondPerTick * speed;
    auto new_position =
        position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                          glm::vec3{0.0f, 0.0f, 1.0f}) *
                              glm::vec4{offset, 0.0f, 0.0f}};
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    }

    float rotation_offset = 0.0f;
    auto diff = input_data.mouse_cursor_position - position_;
    if(glm::length(diff) > 0.01f) {
      auto target_rotation = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
      float target_w = target_rotation - rotation_;
      while (target_w > glm::pi<float>()) {
        target_w -= 2 * glm::pi<float>();
      }
      while (target_w < -glm::pi<float>()) {
        target_w += 2 * glm::pi<float>();
      }
      if (abs(target_w) < 0.01f) {
        rotation_offset = 0.0f;
      }
      else if (target_w > 0) {
        rotation_offset = std::min((float)(std::sqrt(target_w) * 0.15), 0.3f);
      }
      else {
        rotation_offset = -std::min((float)(std::sqrt(-target_w) * 0.15), 0.3f);
      }
    }
    rotation_offset *= kSecondPerTick * rotate_angular_speed * GetSpeedScale();
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }

}

void Fighter::Fire() {
  if (fire_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 30.0f}, rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, rotation_),
            rotation_, GetDamageScale(), velocity);
        fire_count_down_ = 0.333 * kTickPerSecond;  // Fire 3 times per second.
      }
    }
  }
  if (fire_count_down_ > 0) {
    fire_count_down_--;
  }
}

bool Fighter::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x * position.x + position.y * position.y < 1.0f;
}

const char *Fighter::UnitName() const {
  return "Fighter";
}

const char *Fighter::Author() const {
  return "PRXOR";
}
} //namespace battle_game::unit