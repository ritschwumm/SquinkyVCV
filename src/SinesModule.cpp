
#include <sstream>
#include "Squinky.hpp"
#include "WidgetComposite.h"

#ifdef _SINES
#include "Sines.h"
#include "ctrl/SqHelper.h"
#include "ctrl/SqMenuItem.h"
#include "ctrl/SqWidgets.h"

using Comp = Sines<WidgetComposite>;

#define _DRAWBAR

#ifdef _DRAWBAR
class Drawbar : public app::SvgSlider {
public:
    Drawbar(const std::string& handleName) {
        WARN("loading drawbar svg");
        math::Vec margin = math::Vec(3.5, 3.5);

        maxHandlePos = math::Vec(-7, 10).plus(margin);
		minHandlePos = math::Vec(-7, 90).plus(margin);

        setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/scaletx.svg")));
		this->setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance, handleName.c_str())));
        background->box.pos = margin;
        this->box.size.x = 29;
        this->box.size.y = 120;
      
	}
};
#endif

/**
 */
struct SinesModule : Module
{
public:
    SinesModule();
    /**
     *
     * Overrides of Module functions
     */
    void process(const ProcessArgs& args) override;
    void onSampleRateChange() override;

    std::shared_ptr<Comp> blank;
private:

};

void SinesModule::onSampleRateChange()
{
}

SinesModule::SinesModule()
{
    config(Comp::NUM_PARAMS, Comp::NUM_INPUTS, Comp::NUM_OUTPUTS, Comp::NUM_LIGHTS);
    blank = std::make_shared<Comp>(this);
    std::shared_ptr<IComposite> icomp = Comp::getDescription();
    SqHelper::setupParams(icomp, this); 

    onSampleRateChange();
    printf("CALLING INIT\n"); fflush(stdout);
    blank->init();
}

void SinesModule::process(const ProcessArgs& args)
{
    blank->process(args);
}

////////////////////
// module widget
////////////////////

struct SinesWidget : ModuleWidget
{
    SinesWidget(SinesModule *);
    DECLARE_MANUAL("Organ Manual", "https://github.com/squinkylabs/SquinkyVCV/blob/og/docs/og.md");

    Label* addLabel(const Vec& v, const char* str, const NVGcolor& color = SqHelper::COLOR_BLACK)
    {
        Label* label = new Label();
        label->box.pos = v;
        label->text = str;
        label->color = color;
        addChild(label);
        return label;
    }

    void addJacks(SinesModule *module, std::shared_ptr<IComposite> icomp);
    void addDrawbars(SinesModule *module, std::shared_ptr<IComposite> icomp);
    void addOtherControls(SinesModule *module, std::shared_ptr<IComposite> icomp);
};

template <typename TLightBase = RedLight>
struct LEDLightSliderFixed : LEDLightSlider<TLightBase> {
	LEDLightSliderFixed() {
		//this->setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LEDSliderHandle.svg")));
        this->setHandleSvg(APP->window->loadSvg(asset::system("res/ComponentLibrary/LEDSliderHandle.svg")));
	}
};

void SinesWidget::addOtherControls(SinesModule *module, std::shared_ptr<IComposite> icomp)
{
    const float col = 50;
    const float row = 150;
    addParam(SqHelper::createParamCentered<CKSS>(
        icomp,
        Vec(col, row),
        module,
       Comp::DECAY_PARAM));
    auto l = addLabel(Vec(col - 18, row - 30), "fast");
    l->fontSize = 11;
    l = addLabel(Vec(col - 17, row + 10), "slow");
    l->fontSize = 11;

    float keyclickX = col + 40;
    addParam(SqHelper::createParamCentered<CKSSThree>(
        icomp,
        Vec(keyclickX, row),
        module,
        Comp::KEYCLICK_PARAM));
    addLabel(Vec(keyclickX - 18, row - 30), "click");
}


#ifdef _DRAWBAR

const char* handles[] = {
    "res/blue-handle-16.svg",
    "res/blue-handle-513.svg",
    "res/white-handle-8.svg",
    "res/white-handle-4.svg",
    "res/black-handle-223.svg",
    "res/white-handle-2.svg",
    "res/black-handle-135.svg",
    "res/black-handle-113.svg",
    "res/white-handle-1.svg"
};
void SinesWidget::addDrawbars(SinesModule *module, std::shared_ptr<IComposite> icomp)
{
    float drawbarX = 7;
    float drawbarDX = 29;
    float drawbarY = 190;
    float row1 = 81;

    for (int i = 0; i < 9; ++i) {
        std::string handleName = handles[i];P
        auto drawbar = new Drawbar(handleName);
        drawbar->box.pos = Vec(drawbarX + i * drawbarDX, drawbarY);
        if (module) {
            drawbar->paramQuantity = module->paramQuantities[Comp::DRAWBAR1_PARAM +i];
        }
        addParam(drawbar);
    }
    addParam(SqHelper::createParam<Blue30Knob>(
        icomp,
        Vec(160, row1),
        module,  Comp::PERCUSSION1_PARAM ));
    addParam(SqHelper::createParam<Blue30Knob>(
        icomp,
        Vec(200, row1),
        module,  Comp::PERCUSSION2_PARAM ));

}
#else


void SinesWidget::addDrawbars(SinesModule *module, std::shared_ptr<IComposite> icomp)
{
    float drawbarX = 20;
    float drawbarDX = 20;
    float drawbarY = 240;
#ifndef _DRAWBAR
    for (int i = 0; i < 9; ++i) {
        addParam(createLightParamCentered<LEDLightSliderFixed<GreenLight>>( 
            Vec(drawbarX + i * drawbarDX, drawbarY), 
            module, Comp::DRAWBAR1_PARAM + i, Comp::DRAWBAR1_LIGHT + i));
    }

    addParam(createLightParamCentered<LEDLightSliderFixed<GreenLight>>( 
            Vec(drawbarX + 10 * drawbarDX, drawbarY), 
            module, Comp::PERCUSSION1_PARAM, Comp::PERCUSSION1_LIGHT ));
    addParam(createLightParamCentered<LEDLightSliderFixed<GreenLight>>( 
            Vec(drawbarX + 11 * drawbarDX, drawbarY), 
            module, Comp::PERCUSSION2_PARAM, Comp::PERCUSSION2_LIGHT ));

#elif 1
    WARN("about to add new drawbar");
    auto drawbar = SqHelper::createParam<Drawbar>(
        icomp,
        Vec(drawbarX, drawbarY),
        module,
        Comp::DRAWBAR1_PARAM);
  

    addParam(drawbar);
    WARN("drawbar x = %f y=%f", drawbar->box.pos.x, drawbar->box.pos.y );
 WARN("drawbar w = %f h=%f", drawbar->box.size.x, drawbar->box.size.y );
  
    WARN("did it");
#else
    addParam(SqHelper::createParam<BefacoSlidePot>(
        icomp,
        Vec(drawbarX, drawbarY),
        module,
        Comp::DRAWBAR1_PARAM)
    );
#endif
}
#endif

void SinesWidget::addJacks(SinesModule *module, std::shared_ptr<IComposite> icomp)
{
    addInput(createInput<PJ301MPort>(
        Vec(50, 340),
        module,
        Comp::VOCT_INPUT));
    addLabel( Vec(44, 320), "V/Oct");

    addInput(createInput<PJ301MPort>(
        Vec(90, 340),
        module,
        Comp::GATE_INPUT));
    addLabel( Vec(84, 320), "Gate");

    addOutput(createOutput<PJ301MPort>(
        Vec(130, 340),
        module,
        Comp::MAIN_OUTPUT));
    addLabel( Vec(124, 320), "Out");

    addOutput(createOutput<PJ301MPort>(
        Vec(160, 340),
        module,
        Comp::DEBUG_OUTPUT));
    addLabel( Vec(160, 320), "Debug");
}

/**
 * Widget constructor will describe my implementation structure and
 * provide meta-data.
 * This is not shared by all modules in the DLL, just one
 */

SinesWidget::SinesWidget(SinesModule *module)
{
    setModule(module);
    SqHelper::setPanel(this, "res/blank_panel.svg");

    std::shared_ptr<IComposite> icomp = Comp::getDescription();
    addJacks(module, icomp);
    addDrawbars(module, icomp);
    addOtherControls(module, icomp);

    // screws
    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild( createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
}

Model *modelSinesModule = createModel<SinesModule, SinesWidget>("squinkylabs-sines");
#endif

