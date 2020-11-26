

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
