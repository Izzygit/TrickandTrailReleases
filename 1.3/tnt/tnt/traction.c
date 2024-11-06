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

#include "traction.h"
#include <math.h>
#include "utils_tnt.h"

void check_traction(MotorData *m, TractionData *traction, State *state, RuntimeData *rt, tnt_config *config, TractionDebug *traction_dbg){
	float erpmfactor = fmaxf(1, lerp(0, config->wheelslip_scaleerpm, config->wheelslip_scaleaccel, 1, m->abs_erpm));
	bool start_condition1 = false;
	
	// Conditons to end traction control
	if (state->wheelslip) {
		if (rt->current_time - traction->timeron > .3) {		// Time out at 300ms
			traction_dbg->debug4 = 300;
			deactivate_traction(traction, state, rt, traction_dbg);
		} else {
			//This section determines if the wheel is acted on by outside forces by detecting acceleration direction change
			if (traction->highaccelon) { 
				if (sign(traction->accelstartval) != sign(m->accel_history[m->accel_idx])) { 
				// First we identify that the wheel has deccelerated due to traciton control, switching the sign
					traction->highaccelon = false;				
					traction_dbg->debug1 = rt->current_time - traction->timeron;
				} else if (rt->current_time - traction->timeron > .21) {	// Time out at 210ms if wheel does not deccelerate
					traction_dbg->debug4 = 210;
					deactivate_traction(traction, state, rt, traction_dbg);
				}
			} else if (sign(m->accel_history[m->accel_idx])!= sign(m->accel_history[m->last_accel_idx])) { 
			// Next we check to see if accel direction changes again from outside forces 
				traction_dbg->debug4 = 1; //m->accel_history[m->last_accel_idx];
				deactivate_traction(traction, state, rt, traction_dbg);
			}
			
			//This section determines if the wheel is acted on by outside forces by detecting acceleration magnitude
			if (state->wheelslip) {		
				if (sign(traction->accelstartval) * m->acceleration < traction->slowed_accel) {	
				// First we identify that the wheel has deccelerated due to traciton control
					traction_dbg->debug4 = 2;
					deactivate_traction(traction, state, rt, traction_dbg);
				} else if ((rt->current_time - traction->timeron > .22) && 
				   traction->highaccelon) {					// Time out at 220ms if wheel does not deccelerate
					traction_dbg->debug4 = 220;
					deactivate_traction(traction, state, rt, traction_dbg);
				}
			}

			//If we wheelslipped backwards we just need to know the wheel is travelling forwards again
			if (traction->reverse_wheelslip && 
			    m->erpm_sign == m->erpm_sign_soft && 
			    state->wheelslip) {
				traction_dbg->debug4 = 3;
				deactivate_traction(traction, state, rt, traction_dbg);
			}
		}
	}	
	
	if (m->erpm_sign == sign(m->erpm_history[m->last_erpm_idx])) { 	// We check sign to make sure erpm is increasing or has changed direction. 
		if (m->abs_erpm > fabsf(m->erpm_history[m->last_erpm_idx])) {
			start_condition1 = (sign(m->current) * m->acceleration > traction->start_accel * erpmfactor) &&	// The wheel has broken free indicated by abnormally high acceleration in the direction of motor current
				(sign(m->current) == sign(m->accel_history[m->accel_idx])) &&				// a more precise condition than the first for current direction and erpm - last erpm
		   		(!state->braking_pos);									// Do not apply for braking 
		} 				
	} else if (sign(m->erpm_sign_soft) != sign(m->accel_history[m->accel_idx])) {	// The wheel has changed direction and if these are the same sign we do not want traciton conrol because we likely just landed with high wheel spin
		traction->reverse_wheelslip = true;
		start_condition1 = (sign(m->current) * m->acceleration > traction->start_accel * erpmfactor) &&	// The wheel has broken free indicated by abnormally high acceleration in the direction of motor current
			(sign(m->current) == sign(m->accel_history[m->accel_idx])) &&				// a more precise condition than the first for current direction and erpm - last erpm
	   		(!state->braking_pos);									// Do not apply for braking 
	}
	
	// Initiate traction control
	if ((start_condition1) && 					// Acceleration condition matches erpm
	   (!state->wheelslip) &&					// Not in traction control
	   (rt->current_time - traction->timeroff > .02)) {		// Did not recently wheel slip.
		state->wheelslip = true;
		traction->accelstartval = m->acceleration;
		traction->highaccelon = true; 	
		traction->timeron = rt->current_time;
		
		//Debug Section
		traction_dbg->debug2 = erpmfactor;
		traction_dbg->debug6 = m->acceleration / traction_dbg->freq_factor;
		traction_dbg->debug9 = m->erpm;
		traction_dbg->debug3 = m->erpm_history[m->last_erpm_idx];
		traction_dbg->debug1 = 0;
		traction_dbg->debug4 = 0;
		traction_dbg->debug8 = 0;
		if (rt->current_time - traction_dbg->aggregate_timer > 5) { // Aggregate the number of drop activations in 5 seconds
			traction_dbg->aggregate_timer = rt->current_time;
			traction_dbg->debug5 = 0;
		}
		traction_dbg->debug5 += 1;
	}
}

void reset_traction(TractionData *traction, State *state) {
	state->wheelslip = false;
	traction->reverse_wheelslip = false;
}

void deactivate_traction(TractionData *traction, State *state, RuntimeData *rt, TractionDebug *traction_dbg) {
	state->wheelslip = false;
	traction->timeroff = rt->current_time;
	traction->reverse_wheelslip = false;
	traction_dbg->debug8 = traction->timeroff - traction->timeron;
}

void configure_traction(TractionData *traction, tnt_config *config, TractionDebug *traction_dbg){
	traction->start_accel = 1000.0 * config->wheelslip_accelstart / config->hertz; //convert from erpm/ms to erpm/cycle
	traction->slowed_accel = 1000.0 * config->wheelslip_accelend / config->hertz;
	traction_dbg->freq_factor = 1000.0 / config->hertz;
}
