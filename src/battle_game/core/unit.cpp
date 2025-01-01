#include "battle_game/core/unit.h"

#include "battle_game/core/game_core.h"

namespace battle_game {

namespace {
uint32_t life_bar_model_index = 0xffffffffu;
}  // namespace

Unit::Unit(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Object(game_core, id) {
  player_id_ = player_id;
  lifebar_offset_ = {0.0f, 1.0f};
  background_lifebar_color_ = {1.0f, 0.0f, 0.0f, 0.9f};
  front_lifebar_color_ = {0.0f, 1.0f, 0.0f, 0.9f};
  fadeout_lifebar_color_ = {1.0f, 1.0f, 1.0f, 0.5f};
  fadeout_health_ = 1;
  if (!~life_bar_model_index) {
    auto mgr = AssetsManager::GetInstance();
    life_bar_model_index = mgr->RegisterModel(
        {{{-0.5f, 0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
         {{-0.5f, -0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
         {{0.5f, 0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
         {{0.5f, -0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}},
        {0, 1, 2, 1, 2, 3});
  }
  barrel_status_offset_ = {0.0f, -1.0f};
  background_barrel_status_color_ = {0.0f, 1.0f, 0.0f, 0.9f};
  front_barrel_status_color_ = {1.0f, 0.0f, 0.0f, 0.9f};
  heat_barrel_status_color_ = {1.0f, 1.0f, 0.0f, 0.9f};
}

void Unit::SetPosition(glm::vec2 position) {
  position_ = position;
}

void Unit::SetRotation(float rotation) {
  rotation_ = rotation;
}

float Unit::GetSpeedScale() const {
  return 1.0f;
}

float Unit::GetDamageScale() const {
  return 1.0f;
}

float Unit::BasicMaxHealth() const {
  return 100.0f;
}

float Unit::GetHealthScale() const {
  return 1.0f;
}

void Unit::SetLifeBarLength(float new_length) {
  lifebar_length_ = std::max(new_length, 0.0f); //the original code is wrong
}
void Unit::SetLifeBarOffset(glm::vec2 new_offset) {
  lifebar_offset_ = new_offset;
}
void Unit::SetLifeBarFrontColor(glm::vec4 new_color) {
  front_lifebar_color_ = new_color;
}
void Unit::SetLifeBarBackgroundColor(glm::vec4 new_color) {
  background_lifebar_color_ = new_color;
}
void Unit::SetLifeBarFadeoutColor(glm::vec4 new_color) {
  fadeout_lifebar_color_ = new_color;
}
float Unit::GetLifeBarLength() {
  return lifebar_length_;
}
glm::vec2 Unit::GetLifeBarOffset() {
  return lifebar_offset_;
}
glm::vec4 Unit::GetLifeBarFrontColor() {
  return front_lifebar_color_;
}
glm::vec4 Unit::GetLifeBarBackgroundColor() {
  return background_lifebar_color_;
}
glm::vec4 Unit::GetLifeBarFadeoutColor() {
  return fadeout_lifebar_color_;
}

void Unit::ShowLifeBar() {
  lifebar_display_ = true;
}
void Unit::HideLifeBar() {
  lifebar_display_ = false;
}

void Unit::SetBarrelStatusLength(float new_length) {
  barrel_status_length_ = std::max(new_length, 0.0f);
}

void Unit::SetBarrelStatusOffset(glm::vec2 new_offset) {
  barrel_status_offset_ = new_offset;
}

void Unit::SetBarrelStatusFrontColor(glm::vec4 new_color) {
  front_barrel_status_color_ = new_color;
}

void Unit::SetBarrelStatusBackgroundColor(glm::vec4 new_color) {
  background_barrel_status_color_ = new_color;
}

void Unit::SetBarrelStatusOverheatColor(glm::vec4 new_color) {
  heat_barrel_status_color_ = new_color;
}

float Unit::GetBarrelStatusLength() {
  return barrel_status_length_;
}

glm::vec2 Unit::GetBarrelStatusOffset() {
  return barrel_status_offset_;
}

glm::vec4 Unit::GetBarrelStatusFrontColor() {
  return front_barrel_status_color_;
}

glm::vec4 Unit::GetBarrelStatusBackgroundColor() {
  return background_barrel_status_color_;
}

glm::vec4 Unit::GetBarrelStatusOverheatColor() {
  return heat_barrel_status_color_;
}

void Unit::ShowBarrelStatus() {
  barrel_status_display_ = true;
}

void Unit::HideBarrelStatus() {
  barrel_status_display_ = false;
}

void Unit::RenderLifeBar() {
  if (lifebar_display_) {
    auto parent_unit = game_core_->GetUnit(id_);
    auto pos = parent_unit->GetPosition() + lifebar_offset_;
    auto health = parent_unit->GetHealth();
    SetTransformation(pos, 0.0f, {lifebar_length_, 1.0f});
    SetColor(background_lifebar_color_);
    SetTexture(0);
    DrawModel(life_bar_model_index);
    glm::vec2 shift = {(float)lifebar_length_ * (1 - health) / 2, 0.0f};
    SetTransformation(pos - shift, 0.0f, {lifebar_length_ * health, 1.0f});
    SetColor(front_lifebar_color_);
    DrawModel(life_bar_model_index);
    if (std::fabs(health - fadeout_health_) >= 0.01f) {
      fadeout_health_ = health + (fadeout_health_ - health) * 0.93;
      shift = {lifebar_length_ * (health + fadeout_health_ - 1) / 2, 0.0f};
      SetTransformation(pos + shift, 0.0f,
                        {lifebar_length_ * (health - fadeout_health_), 1.0f});
      SetColor(fadeout_lifebar_color_);
      DrawModel(life_bar_model_index);
    } else {
      fadeout_health_ = health;
    }
    if(barrel_status_display_)
    {
      pos = parent_unit->GetPosition() + barrel_status_offset_;
      float temperature = 1.0 * parent_unit->GetBarrelTemperature() / (max_barrel_temperature_ * kTickPerSecond);
      float overheat = 1.0 * parent_unit->GetBarrelOverheatCountDown() / (time_to_cool_per_overheat_ * kTickPerSecond);
      if(temperature > 0)
      {
        SetTransformation(pos, 0.0f, {barrel_status_length_, 1.0f});
        SetColor(background_barrel_status_color_);
        SetTexture(0);
        DrawModel(life_bar_model_index);
        glm::vec2 shift = {(float)barrel_status_length_ * (1 - temperature) / 2, 0.0f};
        SetTransformation(pos - shift, 0.0f, {barrel_status_length_ * temperature, 1.0f});
        SetColor(front_barrel_status_color_);
        DrawModel(life_bar_model_index);
      }
      if(barrel_overheat_count_down_ > 0)
      {
        SetTransformation(pos, 0.0f, {barrel_status_length_, 1.0f});
        SetColor(background_barrel_status_color_);
        SetTexture(0);
        DrawModel(life_bar_model_index);
        glm::vec2 shift = {(float)barrel_status_length_ * (1 - overheat) / 2, 0.0f};
        SetTransformation(pos - shift, 0.0f, {barrel_status_length_ * overheat, 1.0f});
        SetColor(heat_barrel_status_color_);
        DrawModel(life_bar_model_index);
      }
    }
  }
  
}

void Unit::RenderHelper() {
}

const char *Unit::UnitName() const {
  return "Unknown Unit";
}
const char *Unit::Author() const {
  return "Unknown Author";
}
}  // namespace battle_game
