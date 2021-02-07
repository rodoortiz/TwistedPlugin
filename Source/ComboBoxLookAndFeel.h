/*
  ==============================================================================

    ComboBoxLookAndFeel.h
    Created: 13 Nov 2020 12:50:10pm
    Author:  Samuel Martínez

  ==============================================================================
*/

#pragma once
class ComboBoxLookAndFeel : public LookAndFeel_V4
{
public:
    Font getComboBoxFont (ComboBox&) override {return getCommonMenuFont();} 
    Font getPopupMenuFont() override {return getCommonMenuFont();}
    
private:
    Font getCommonMenuFont()
    {
        return Font ("Arial", "Regular", 15.0f);
    }
};
