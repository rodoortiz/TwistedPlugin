

#pragma once
class ButtonLookAndFeel : public LookAndFeel_V4
{
public:
    Font getTextButtonFont (TextButton&, int) override
    {
        return Font ("Wingdings 3", "Regular", 15.0f);
    }
    
private:

};
