/************************** BEGIN DaisyControlUIBase.h **********************
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

#ifndef FAUST_DAISYGENERICCONTROL_H
#define FAUST_DAISYGENERICCONTROL_H

#include <string>
#include <vector>
#include <memory>
#include <string.h>

#include "daisy_seed.h"

#include "faust/gui/DecoratorUI.h"
#include "faust/gui/ValueConverter.h"


/*******************************************************************************
 * DaisyControlUIBase : Faust User Interface
 ******************************************************************************/

class DaisyControlUIBase : public GenericUI
{
    protected:
    
        // Base class for updatable items
        struct UpdatableZone {
            FAUSTFLOAT* fZone;
            
            UpdatableZone(FAUSTFLOAT* zone) : fZone(zone) {}
            virtual ~UpdatableZone() {}
            
            virtual void update() = 0;
        };
    
        struct SwitchButton : daisy::Switch, UpdatableZone {
            
            SwitchButton(FAUSTFLOAT* zone):UpdatableZone(zone)
            {}
            
            void update()
            {
                *fZone = RawState();
            }
        };
    
        // Implement checkbox using daisy::Switch
        struct CheckButton : daisy::Switch, UpdatableZone {
            
            FAUSTFLOAT fLastButton;
            
            CheckButton(FAUSTFLOAT* zone):UpdatableZone(zone), fLastButton(0)
            {}
            
            void update()
            {
                FAUSTFLOAT button = RawState();
                if (button == 1.0 && (button != fLastButton)) {
                    *fZone = !*fZone;
                }
                fLastButton = button;
            }
        };

        // Implements an analog knob by calling Process on the knob instance
        // that has already been created inside its HW Class
        struct AnalogKnob : UpdatableZone {
           
            std::unique_ptr<ValueConverter> fConverter;
            daisy::AnalogControl* fControl;

            FAUSTFLOAT fStep;
            bool fQuantizeToStep;
            
            AnalogKnob(daisy::AnalogControl* control, FAUSTFLOAT* zone, std::unique_ptr<ValueConverter>& converter, int rate, FAUSTFLOAT step, bool quantize)
            : UpdatableZone(zone), fConverter(std::move(converter)), fControl(control), fStep(step), fQuantizeToStep(quantize)
            {
            }
            
            void update()
            {
                FAUSTFLOAT newValue = fConverter->ui2faust(fControl->Process());
                if (fQuantizeToStep) {
                    *fZone = round(newValue / fStep) * fStep;
                } else {
                    *fZone = newValue;
                }
            }
        };

        std::vector<std::unique_ptr<UpdatableZone>> fItems;
        std::string fKey, fValue, fScale;
        bool fQuantize = false;
        int fRate, fBoxLevel;

        struct KnobContext
        {
            int fKnobId;
            FAUSTFLOAT* fZone;
            FAUSTFLOAT fMin;
            FAUSTFLOAT fMax;
            std::string fScale;
            FAUSTFLOAT fStep;
            bool fQuantizeToStep;
            KnobContext(int kid,
                        FAUSTFLOAT* zone,
                        FAUSTFLOAT min,
                        FAUSTFLOAT max,
                        const std::string& scale,
                        FAUSTFLOAT step,
                        bool quantize)
            :fKnobId(kid), fZone(zone), fMin(min), fMax(max), fScale(scale), fStep(step), fQuantizeToStep(quantize)
            {}
        };
        std::vector<KnobContext> fKnobs;


        void InitKnob(int knob_pin, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max, const std::string& scale, FAUSTFLOAT step, bool quantize)
        {
            // context is kept, to be used in InitKnobs()
            fKnobs.push_back(KnobContext(knob_pin, zone, min, max, scale, step, quantize));
        }

        virtual void InitKnobs() = 0;

    public:

        DaisyControlUIBase(int rate)
        :fScale("lin"),fRate(rate), fBoxLevel(0)
        {}

        virtual ~DaisyControlUIBase() {}

        // -- widget's layouts
        void openTabBox(const char* label) { fBoxLevel++; }
        void openHorizontalBox(const char* label) { fBoxLevel++; }
        void openVerticalBox(const char* label) { fBoxLevel++; }
        void closeBox()
        {
            if (--fBoxLevel == 0) InitKnobs();
        }
    
        void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            addNumEntry(label, zone, init, min, max, step);
        }
        void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            addNumEntry(label, zone, init, min, max, step);
        }

        void update()
        {
            for (const auto& it : fItems) it->update();
        }
};

#endif //__FAUST_DAISYGENERICCONTROL_H
/**************************  END  DaisyPatchSmControlUI.h **************************/



