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

#pragma once

#include QMK_KEYBOARD_H

typedef enum {
  DM_PRESSED,
  DM_INTERRUPTED,
  DM_ONESHOT,
  DM_INACTIVE
} dynamic_mod_state;

typedef struct {
  dynamic_mod_state state;
  uint16_t          keycode;
  uint8_t           modbit;
} dynamic_mod;

#define DYNAMIC_MOD(kc, mb) { \
  .state   = DM_INACTIVE,     \
  .keycode = (kc),            \
  .modbit  = (mb)             \
}

// Can be used in process_record_user() to process any additional, user defined
// dynamic mods.

void process_single_dynamic_mod(uint16_t keycode, keyrecord_t *record, dynamic_mod *dm);

// Can be overriden in keycode.c to flag any additional, user defined dynamic
// mods.

bool is_dynamic_mod_key(uint16_t keycode);

// These functions should be overriden by the user to return true if any keys
// should either reset all active dynamic mod behaviour, or be ignored by the
// dynamic mod decision making. For example, the Esc key might act as a reset
// key and some layer key presses might be ignored.

bool is_dynamic_mod_cancel_key(uint16_t keycode);
bool is_dynamic_mod_ignored_keycode(uint16_t keycode);
