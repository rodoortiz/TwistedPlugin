/*
  ==============================================================================

    LoginComponent.h
    Created: 25 Feb 2021 11:29:13am
    Author:  Rodolfo Ortiz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RestRequest.h"

class LoginComponent : public Component, public Button::Listener {
public:
    LoginComponent();
    ~LoginComponent();
    
    void paint(Graphics&) override;
    void resized() override;
    
    static juce_wchar getDefaultPasswordChar() noexcept
    {
       #if JUCE_LINUX
        return 0x2022;
       #else
        return 0x25cf;
       #endif
    };
    
    void dismissComponent();
    void dismissAuthUI();
    void recoverAuthUI();
    void buttonClicked(Button*) override;
    
    void writeJSON(String inJSON);
    String readJSON();
    
private:
    Label instructionLabel{"Enter your user ID and Password"}, userIDLabel {"User ID:"}, passwordLabel{"Password:"}, errorConnectionLabel, errorLabel;
    TextEditor userIDEditor, passwordEditor;
    TextButton authButton {"Authenticate"}, goBackButton{"Go Back"}, tryAgainButton{"Try Again"};
    
    RestRequest request;
    RestRequest::Response response;

    var parsedJson;
    String jwt;
    Base64 base64;
    bool access = true;
    
};
