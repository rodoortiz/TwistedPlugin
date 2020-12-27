
#pragma once
class ButtonLookAndFeelStart : public LookAndFeel_V4
{
public:
    Font getTextButtonFont (TextButton&, int) override
    {
        return Font ("Wingdings 3", "Regular", 15.0f);
    }
    
private:
};

class ButtonLookAndFeelStop : public LookAndFeel_V4
{
public:
    Font getTextButtonFont (TextButton&, int) override
    {
        return Font ("Webdings", "Regular", 10.0f);
    }
    
private:
};
