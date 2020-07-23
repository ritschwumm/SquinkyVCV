
#pragma once

#include "Divider.h"
#include "IComposite.h"
#include "PitchUtils.h"
#include "SinesVCO.h"

#include <assert.h>
#include <memory>

namespace rack {
    namespace engine {
        struct Module;
    }
}
using Module = ::rack::engine::Module;


template <class TBase>
class SinesDescription : public IComposite
{
public:
    Config getParam(int i) override;
    int getNumParams() override;
};

template <class TBase>
class Sines : public TBase
{
public:
    using T = float_4;

    Sines(Module * module) : TBase(module)
    {
    }
    Sines() : TBase()
    {
    }

    /**
    * re-calc everything that changes with sample
    * rate. Also everything that depends on baseFrequency.
    *
    * Only needs to be called once.
    */
    void init();

    enum ParamIds
    {
        TEST_PARAM,
        NUM_PARAMS
    };

    enum InputIds
    {
        V_OCT_INPUT,
        GATE_INPUT,
        NUM_INPUTS
    };

    enum OutputIds
    {
        MAIN_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds
    {
        NUM_LIGHTS
    };

    /** Implement IComposite
     */
    static std::shared_ptr<IComposite> getDescription()
    {
        return std::make_shared<SinesDescription<TBase>>();
    }

    /**
     * Main processing entry point. Called every sample
     */
    //void step() override;
    void process(const typename TBase::ProcessArgs& args) override;

private:

    SinesVCO<T> sines[3];
    
    Divider divn;

    void stepn();

};


template <class TBase>
inline void Sines<TBase>::init()
{
    divn.setup(4, [this]() {
        this->stepn();
    });
}

template <class TBase>
inline void Sines<TBase>::stepn()
{
    const float semi = PitchUtils::semitone;
    float pitches[12] = {
        -1, 0, 7 * semi, 1,
        1 + 7 * semi, 2, 2 + 4 * semi, 2 + 7 * semi,
        3, 0, 0, 0};
    
    float_4 pitch;
    pitch.load(pitches);
    sines[0].setPitch(pitch);
    pitch.load(pitches + 4);
    sines[1].setPitch(pitch);
    pitch.load(pitches + 8);
    sines[2].setPitch(pitch);
}

template <class TBase>
inline void Sines<TBase>::process(const typename TBase::ProcessArgs& args)
{
    T sum;
    T deltaT(args.sampleTime);
    sum = sines[0].process(deltaT);
    sum += sines[1].process(args.sampleTime);
    sum += sines[2].process(args.sampleTime);
}

template <class TBase>
int SinesDescription<TBase>::getNumParams()
{
    return Sines<TBase>::NUM_PARAMS;
}

template <class TBase>
inline IComposite::Config SinesDescription<TBase>::getParam(int i)
{
    Config ret(0, 1, 0, "");
    switch (i) {
        case Sines<TBase>::TEST_PARAM:
            ret = {-1.0f, 1.0f, 0, "Test"};
            break;
        default:
            assert(false);
    }
    return ret;
}

