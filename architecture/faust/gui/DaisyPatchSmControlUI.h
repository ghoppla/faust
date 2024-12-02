/************************** BEGIN DaisyPatchSmControlUI.h **********************
 FAUST Architecture File
 Copyright (C) 2003-2024 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 *************************************************************************/


#ifndef FAUST_DAISYPATCHSMCONTROL_H
#define FAUST_DAISYPATCHSMCONTROL_H


#include <string>
#include <vector>
#include <memory>
#include <string.h>

#include "daisy_seed.h"

#include "faust/gui/DecoratorUI.h"
#include "faust/gui/ValueConverter.h"
#include "faust/gui/DaisyControlUIBase.h"


class DaisyPatchSmControlUI : public DaisyControlUIBase
{   
    protected:

        daisy::patch_sm::DaisyPatchSM* fHw;

        void InitKnobs() override
        {
            for (size_t i = 0; i < fKnobs.size(); i++) {
                std::unique_ptr<ValueConverter> converter;
                if (fKnobs[i].fScale == "log") {
                    converter = std::make_unique<LogValueConverter>(0., 1., fKnobs[i].fMin, fKnobs[i].fMax);
                } else if (fKnobs[i].fScale == "exp") {
                    converter = std::make_unique<ExpValueConverter>(0., 1., fKnobs[i].fMin, fKnobs[i].fMax);
                } else {
                    converter = std::make_unique<LinearValueConverter>(0., 1., fKnobs[i].fMin, fKnobs[i].fMax);
                }
                std::unique_ptr<AnalogKnob> knob = std::make_unique<AnalogKnob>(&fHw->controls[fKnobs[i].fKnobId],
                                                  fKnobs[i].fZone,
                                                  converter,
                                                  fRate,
                                                  fKnobs[i].fStep,
                                                  fKnobs[i].fQuantizeToStep);
                fItems.push_back(std::move(knob));
            }
        }

    public: 

        DaisyPatchSmControlUI(daisy::patch_sm::DaisyPatchSM* hw, int rate)
        : DaisyControlUIBase(rate), fHw(hw)
        { }
    
        // -- active widgets
        void addButton(const char* label, FAUSTFLOAT* zone)
        {
            if (fKey == "switch") {
                std::unique_ptr<SwitchButton> button = std::make_unique<SwitchButton>(zone);
                //if (fValue == "1") {
                    //button->Init(fSeed->GetPin(SW_1_PIN), fRate);
                //}
                fItems.push_back(std::move(button));
            }
            fValue = fKey = fScale = "";
        }
    
        void addCheckButton(const char* label, FAUSTFLOAT* zone)
        {
            if (fKey == "switch") {
                std::unique_ptr<CheckButton> button = std::make_unique<CheckButton>(zone);
                //if (fValue == "1") {
                    //button->Init(fSeed->GetPin(SW_1_PIN), fRate);
                //}
                fItems.push_back(std::move(button));
            }
            fValue = fKey = fScale = "";
        }

        void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            if (fKey == "knob") {
                if (fValue == "1") {
                    InitKnob(daisy::patch_sm::CV_1, zone, min, max, fScale, step, fQuantize);
                } else if (fValue == "2") {
                    InitKnob(daisy::patch_sm::CV_2, zone, min, max, fScale, step, fQuantize);
                } else if (fValue == "3") {
                    InitKnob(daisy::patch_sm::CV_3, zone, min, max, fScale, step, fQuantize);
                } else if (fValue == "4") {
                    InitKnob(daisy::patch_sm::CV_4, zone, min, max, fScale, step, fQuantize);
                }
            }
            fValue = fKey = fScale = "";
        }

        // -- metadata declarations
        void declare(FAUSTFLOAT* zone, const char* key, const char* val)
        {
            if (strcmp(key, "switch") == 0
                || strcmp(key, "knob") == 0) {
                fKey = key;
                fValue = val;
            } else if (std::string(key) == "scale") {
                fScale = val;
            } else if(std::string(key) == "quantize") {
                fQuantize = true;
            }
        }
};

#endif //__FAUST_DAISYPATCHSMONTROL_H
/**************************  END  DaisyPatchSmControlUI.h **************************/

