// Copyright 2026 Hannah Blythe Morrison
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "print.h"
#include "dynamic_mods.h"

// Declare internal functions.

void process_single_dynamic_mod(uint16_t keycode, keyrecord_t *record, dynamic_mod *dm);
void process_dynamic_mod_press_action(uint16_t keycode, keyrecord_t *record, dynamic_mod *dm);
void process_dynamic_mod_release_action(uint16_t keycode, keyrecord_t *record, dynamic_mod *dm);
void process_other_press_action(uint16_t keycode, keyrecord_t *record, dynamic_mod *dm);

// Static records that hold the state of each dynamic mod.

static dynamic_mod dm_lsft = DYNAMIC_MOD(DM_LSFT, MOD_BIT(KC_LSFT));
static dynamic_mod dm_lctl = DYNAMIC_MOD(DM_LCTL, MOD_BIT(KC_LCTL));
static dynamic_mod dm_lalt = DYNAMIC_MOD(DM_LALT, MOD_BIT(KC_LALT));
static dynamic_mod dm_lgui = DYNAMIC_MOD(DM_LGUI, MOD_BIT(KC_LGUI));
static dynamic_mod dm_rsft = DYNAMIC_MOD(DM_RSFT, MOD_BIT(KC_RSFT));
static dynamic_mod dm_rctl = DYNAMIC_MOD(DM_RCTL, MOD_BIT(KC_RCTL));
static dynamic_mod dm_ralt = DYNAMIC_MOD(DM_RALT, MOD_BIT(KC_RALT));
static dynamic_mod dm_rgui = DYNAMIC_MOD(DM_RGUI, MOD_BIT(KC_RGUI));

// Process each dynamic mod in turn.

void process_record_dynamic_mods(uint16_t keycode, keyrecord_t *record) {
  process_single_dynamic_mod(keycode, record, &dm_lctl);
  process_single_dynamic_mod(keycode, record, &dm_lalt);
  process_single_dynamic_mod(keycode, record, &dm_lgui);
  process_single_dynamic_mod(keycode, record, &dm_lsft);
  process_single_dynamic_mod(keycode, record, &dm_rctl);
  process_single_dynamic_mod(keycode, record, &dm_ralt);
  process_single_dynamic_mod(keycode, record, &dm_rgui);
  process_single_dynamic_mod(keycode, record, &dm_rsft);
}

void process_single_dynamic_mod(uint16_t keycode, keyrecord_t *record, dynamic_mod *dm) {

  // Check whether the current key action is associated with any dynamic mod.

  if (is_dynamic_mod(keycode)) {

    // Process the key action on this dynamic mod key.

    if (keycode == dm->keycode) {
      if (record->event.pressed)
        process_dynamic_mod_press_action(keycode, record, dm);
      else
        process_dynamic_mod_release_action(keycode, record, dm);
    }
  } else {

    // Process how the key action will impact this dynamic mod.

    if (record->event.pressed)
      process_other_press_action(keycode, record, dm);
  }
}

// Act on the dynamic mod key being pressed.

void process_dynamic_mod_press_action(uint16_t keycode, keyrecord_t *record, dynamic_mod *dm) {
  add_mods(dm->modbit);
  dm->state = DM_PRESSED;
}

// Act on the dynamic mod key being released based on the state that the
// dynamic mod is in.

void process_dynamic_mod_release_action(uint16_t keycode, keyrecord_t *record, dynamic_mod *dm) {
  switch (dm->state) {
    case DM_INTERRUPTED:

      // If the dynamic mod has been interrupted by another key press before it
      // has been released, remove the mod bit and mark the dynamic mod as
      // inactive.

      del_mods(dm->modbit);
      dm->state = DM_INACTIVE;
      break;
    case DM_PRESSED:

      // If the dynamic mod is still in the pressed state, then the modifier
      // that was set has not been used by another key press. In this case, set
      // a oneshot modifier.

      del_mods(dm->modbit);
      add_oneshot_mods(dm->modbit);
      dm->state = DM_ONESHOT;
      break;
    default:
  }
}

// Process other non dynamic mod keys being pressed while this dynamic mod is
// active.

void process_other_press_action(uint16_t keycode, keyrecord_t *record, dynamic_mod *dm) {
  switch (dm->state) {
    case DM_PRESSED:

      // If the pressed key is a cancel key, remove the modifier and revert the
      // state of the dynamic mod.

      if (is_dynamic_mod_cancel_key(keycode)) {
         del_mods(dm->modbit);
         dm->state = DM_INACTIVE;
         return;
      }

      // Only mark the dynamic mod as interrupted if the pressed key is not
      // being ignored.

      if (! is_dynamic_mod_ignored_key(keycode))
        dm->state = DM_INTERRUPTED;
      break;
    case DM_ONESHOT:
      if (get_oneshot_mods() & dm->modbit) {

        // If the dynamic mod has set a oneshot modifier and the pressed key is
        // a cancel key, remove the oneshot modifier before the cancel key is
        // processed itself.

        if (is_dynamic_mod_cancel_key(keycode))
          del_oneshot_mods(dm->modbit);
      } else {

        // If the oneshot modifier is not set then it has been used and the
        // dynamic mod state can be reset.

        dm->state = DM_INACTIVE;
      }
      break;
    default:
  }
}

// Return true if the given keycode is a dynamic mod.

bool is_dynamic_mod(uint16_t keycode) {
  switch (keycode) {
    case DM_LCTL:
    case DM_LALT:
    case DM_LGUI:
    case DM_LSFT:
    case DM_RCTL:
    case DM_RALT:
    case DM_RGUI:
    case DM_RSFT:
      return true;
    default:
      return is_dynamic_mod_key(keycode);
  }
}

// User defined function that will return true if the given keycode is an
// additional dynamic mod key.

__attribute__((weak)) bool is_dynamic_mod_key(uint16_t keycode) {
  return false;
}

// User defined function that will return true if the given keycode should
// cancel all current dynamic mod behaviour.

__attribute__((weak)) bool is_dynamic_mod_cancel_key(uint16_t keycode) {
  return keycode == KC_ESC;
}

// User defined function that will return true if the given keycode should be
// ignored by the dynamic mod processing.

__attribute__((weak)) bool is_dynamic_mod_ignored_key(uint16_t keycode) {
  return IS_QK_LAYER_TAP(keycode);
}
