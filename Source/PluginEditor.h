#pragma once
/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "Gui/Knob.h"
#include "Gui/TooglableButton.h"
#include "Gui/ButtonList.h"
#include "Components/SetPresetNameWindow.h"
#include "Components/PresetBar.h"
#include "Components/ScaleComponent.h"
enum KeyPressCommandIDs
{
    buttonNextProgram = 1,
    buttonPrevProgram,
    buttonPadNextProgram,
    buttonPadPrevProgram,
   
};

enum MenuAction
{
    Cancel = 0,
    ToggleMidiKeyboard,
    ImportPreset,
    ImportBank,
    ExportBank,
    ExportPreset,
    SavePreset,
    NewPreset,
    RenamePreset,
    DeletePreset,
    DeleteBank,
    ShowBanks,
    LoadBank // LoadBank must be the last enum value
};
//==============================================================================
/**
*/
class ObxdAudioProcessorEditor  : public AudioProcessorEditor
//                                  , public AudioProcessorListener
                                , public AsyncUpdater
                                , public ChangeListener
//                                  , public Slider::Listener
                                , public Button::Listener
//                                  , public ComboBox::Listener
                                , public ActionListener
                                , public ApplicationCommandTarget
                                , public Timer
                                , public FileDragAndDropTarget
                                , public ScalableComponent
                                
{
public:
    ObxdAudioProcessorEditor(ObxdAudioProcessor& ownerFilter);
    ~ObxdAudioProcessorEditor();
        
    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;
    
    void scaleFactorChanged() override;
    
	void mouseUp (const MouseEvent& e) override;
	void paint (Graphics& g) override;
    
    void updateFromHost();
    void handleAsyncUpdate() override;
    String getCurrentProgramName(){
        return processor.getProgramName(processor.getCurrentProgram());
    }
    void updatePresetBar(bool resize=true);
	//==============================================================================
	void changeListenerCallback (ChangeBroadcaster* source) override;
    void buttonClicked (Button *) override;
    //bool keyPressed(const KeyPress & press) override;
    void timerCallback() override {
#if JUCE_WINDOWS || JUCE_LINUX
    // No run timer to grab component on  window
#else
        this->grabKeyboardFocus();
#endif
        countTimer ++;
        if (countTimer == 4 && needNotifytoHost){
            countTimer = 0;
            needNotifytoHost= false;
            processor.updateHostDisplay();
        }

        countTimerForLed++;
        if (midiUnlearnButton && midiUnlearnButton->getToggleState() && countTimerForLed > 3) {
            midiUnlearnButton->setToggleState(false, NotificationType::sendNotification);
            countTimerForLed = 0;
        }
    }
    ApplicationCommandTarget* getNextCommandTarget() override {
        return nullptr;
    };
    void getAllCommands (Array<CommandID>& commands) override {
        Array<CommandID> ids { KeyPressCommandIDs::buttonNextProgram, KeyPressCommandIDs::buttonPrevProgram,
                               KeyPressCommandIDs::buttonPadNextProgram, KeyPressCommandIDs::buttonPadPrevProgram
                               };

        commands.addArray (ids);
    };
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override {
        switch (commandID)
        {
            case KeyPressCommandIDs::buttonNextProgram:
                result.setInfo ("Move up", "Move the button + ", "Button", 0);
                result.addDefaultKeypress ('+', 0);
                result.setActive (true);
                break;
            case KeyPressCommandIDs::buttonPrevProgram:
                result.setInfo ("Move right", "Move the button - ", "Button", 0);
                result.addDefaultKeypress ('-', 0);
                result.setActive (true);
                break;
            case KeyPressCommandIDs::buttonPadNextProgram:
                result.setInfo ("Move down", "Move the button Pad + ", "Button", 0);
                result.addDefaultKeypress (KeyPress::numberPadAdd, 0);
                result.setActive (true);
                break;
            case KeyPressCommandIDs::buttonPadPrevProgram:
                result.setInfo ("Move left", "Move the button Pad -", "Button", 0);
                result.addDefaultKeypress (KeyPress::numberPadSubtract, 0);
                result.setActive (true);
                break;
            default:
                break;
        }
    };
    bool perform (const InvocationInfo& info) override {

       switch (info.commandID)
       {
           case KeyPressCommandIDs::buttonNextProgram:
           case KeyPressCommandIDs::buttonPadNextProgram:
                nextProgram();
				grabKeyboardFocus();
                break;

           case KeyPressCommandIDs::buttonPrevProgram:
           case KeyPressCommandIDs::buttonPadPrevProgram:
                prevProgram();
				grabKeyboardFocus();
                break;
           default:
               return false;
       }
       return true;
    };/*
    bool keyPressed (const KeyPress& key,
                     Component* originatingComponent) override {
        DBG("--- " << key.getKeyCode());
        
    };*/
    
    void nextProgram();
    void prevProgram();
    
    void MenuActionCallback(int action);
    void deleteBank();
    
    void resized() override;
    
    bool isHighResolutionDisplay() const
    {
        return processor.physicalPixelScaleFactor > 1.0;
    }
    void actionListenerCallback(const String& message) override;
private:
    Rectangle<int> transformBounds(int x, int y, int w, int h);
	Knob* addKnob (int x, int y, int d, ObxdAudioProcessor& filter, int parameter, String name, float defval);
	void placeLabel (int x, int y, String text);
	TooglableButton* addButton (int x, int y, int w, int h, ObxdAudioProcessor& filter, int parameter, String name);
	ButtonList* addList(int x, int y, int w, int h, ObxdAudioProcessor& filter, int parameter, String name, String nameImg);
    ImageButton* addMenuButton (int x, int y, int d, String nameImg);
    void createMenu ();
    void createMidi(int, PopupMenu &);
    void resultFromMenu (const Point<int>);
    void clean();
    

	void rebuildComponents (ObxdAudioProcessor&);
    void loadSkin(ObxdAudioProcessor&);
	//==============================================================================
public:
    ObxdAudioProcessor& processor;
private:
    // images
    Image backgroundImage;
    std::map<String, Component*> mappingComps;
	//==============================================================================
	Knob* cutoffKnob=nullptr,
    *resonanceKnob=nullptr,
    *osc1PitchKnob=nullptr,
    *osc2PitchKnob=nullptr,
    *osc2DetuneKnob=nullptr,
    *volumeKnob=nullptr,
    *portamentoKnob=nullptr,
    *voiceDetuneKnob=nullptr,
    *filterEnvelopeAmtKnob=nullptr,
    *pulseWidthKnob=nullptr,
    *xmodKnob=nullptr,
    *multimodeKnob=nullptr,
    *attackKnob=nullptr,
    *decayKnob=nullptr,
    *sustainKnob=nullptr,
    *releaseKnob=nullptr,
    *fattackKnob=nullptr,
    *fdecayKnob=nullptr,
    *fsustainKnob=nullptr,
    *freleaseKnob=nullptr,
    *osc1MixKnob=nullptr,
    *osc2MixKnob=nullptr,
    *noiseMixKnob=nullptr,
    *filterDetuneKnob=nullptr,
    *envelopeDetuneKnob=nullptr,
    *portamentoDetuneKnob=nullptr,
    *tuneKnob=nullptr,
    *lfoFrequencyKnob=nullptr,
    *lfoAmt1Knob=nullptr,
    *lfoAmt2Knob=nullptr,
    *pan1Knob=nullptr,
    *pan2Knob=nullptr,
    *pan3Knob=nullptr,
    *pan4Knob=nullptr,
    *pan5Knob=nullptr,
    *pan6Knob=nullptr,
    *pan7Knob=nullptr,
    *pan8Knob=nullptr,
    *brightnessKnob=nullptr,
    *envPitchModKnob=nullptr,
    *bendLfoRateKnob=nullptr,
    *veloAmpEnvKnob=nullptr,
    *veloFltEnvKnob=nullptr,
    *transposeKnob=nullptr;

    TooglableButton* hardSyncButton=nullptr,
    *osc1SawButton=nullptr,
    *osc2SawButton=nullptr,
    *osc1PulButton=nullptr,
    *osc2PulButton=nullptr,
    *filterKeyFollowButton=nullptr,
    *unisonButton=nullptr,
    *pitchQuantButton=nullptr,
    *filterHQButton=nullptr,
    *filterBPBlendButton=nullptr,
    *lfoSinButton=nullptr,
    *lfoSquareButton=nullptr,
    *lfoSHButton=nullptr,
    *lfoOsc1Button=nullptr,
    *lfoOsc2Button=nullptr,
    *lfoFilterButton=nullptr,
    *lfoPwm1Button=nullptr,
    *lfoPwm2Button=nullptr,
    *bendRangeButton=nullptr,
    *bendOsc2OnlyButton=nullptr,
    *fourPoleButton=nullptr,
    *asPlayedAllocButton=nullptr,
    *midiLearnButton=nullptr,
    *midiUnlearnButton=nullptr;

	ButtonList *voiceSwitch = nullptr,
    *legatoSwitch = nullptr;

	File skinFolder;
    
    //==============================================================================
    OwnedArray<Knob::KnobAttachment>              knobAttachments;
    OwnedArray<AudioProcessorValueTreeState::ButtonAttachment> toggleAttachments;
    OwnedArray<ButtonList::ButtonListAttachment>  buttonListAttachments;
    
    OwnedArray<ImageButton> imageButtons;
    
    OwnedArray<PopupMenu> popupMenus;
    
    bool notLoadSkin = false;
    int progStart;
    int bankStart;
    int skinStart;
    
    Array<File> skins;
    Array<File> banks;
    std::unique_ptr<SetPresetNameWindow> setPresetNameWindow;
    std::unique_ptr<PresetBar> presetBar;
    std::unique_ptr<FileChooser> fileChooser;
    // Command manager
    ApplicationCommandManager commandManager;
    int countTimer =0;
    bool needNotifytoHost = false;

    Array<String> midiFiles;
    int menuMidiNum;
    int menuScaleNum;
    int countTimerForLed = 0;

    struct Action
    {
        static const String panReset;
    };
};

#endif  // PLUGINEDITOR_H_INCLUDED
