// Copyright 2024 Michael Silberstein
//
// This file is part of the VESC package.
//
// This VESC package is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This VESC package is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "conf/datatypes.h"

typedef struct {
	float angle_kp[7][2];
	int count;
	float kp_rate;
} KpArray;

void pitch_kp_configure(const tnt_config *config, KpArray *k, int mode);
void roll_kp_configure(const tnt_config *config, KpArray *k, int mode);
void yaw_kp_configure(const tnt_config *config, KpArray *k, int mode);
float angle_kp_select(float angle, const KpArray *k);
void angle_kp_reset(KpArray *k);
