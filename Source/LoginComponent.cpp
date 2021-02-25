/*
  ==============================================================================

    LoginComponent.cpp
    Created: 25 Feb 2021 11:29:13am
    Author:  Rodolfo Ortiz

  ==============================================================================
*/

#include "LoginComponent.h"

LoginComponent::LoginComponent() : passwordEditor(String(), getDefaultPasswordChar()) {

    //Set Labels
    addAndMakeVisible(instructionLabel);
    instructionLabel.setText("Enter User ID and Password",dontSendNotification);
    instructionLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(userIDLabel);
    userIDLabel.setText("User ID:", dontSendNotification);
    addAndMakeVisible(passwordLabel);
    passwordLabel.setText("Password:", dontSendNotification);
    addAndMakeVisible(errorConnectionLabel);
    errorConnectionLabel.setJustificationType(Justification::centred);
    errorConnectionLabel.setVisible(false);
    addAndMakeVisible(errorLabel);
    errorLabel.setJustificationType(Justification::centred);
    errorLabel.setVisible(false);

    //Set Text Editors
    addAndMakeVisible(userIDEditor);
    addAndMakeVisible(passwordEditor);

    //Set Button
    addAndMakeVisible(authButton);
    authButton.setAlpha(1.0f);
    authButton.setColour(TextButton::buttonColourId, Colours::green);
    authButton.addListener(this);
    addAndMakeVisible(goBackButton);
    goBackButton.setAlpha(1.0f);
    goBackButton.setColour(TextButton::buttonColourId, Colours::red);
    goBackButton.addListener(this);
    goBackButton.setVisible(false);
    addAndMakeVisible(tryAgainButton);
    tryAgainButton.setAlpha(1.0f);
    tryAgainButton.setColour(TextButton::buttonColourId, Colours::yellow);
    tryAgainButton.addListener(this);
    tryAgainButton.setVisible(false);

}

LoginComponent::~LoginComponent() {

}

void LoginComponent::paint(Graphics& g) {
    g.fillAll(Colours::grey);
}

void LoginComponent::resized() {
    instructionLabel.setBoundsRelative(0.0f, 0.05f, 1.0f, 0.1f);
    userIDLabel.setBoundsRelative(0.0f, 0.2f, 0.9f, 0.1f);
    passwordLabel.setBoundsRelative(0.0f, 0.4f, 0.9f, 0.1f);
    errorConnectionLabel.setSize(getWidth(), 30);
    errorConnectionLabel.setCentreRelative(0.5f, 0.4f);
    errorLabel.setSize(getWidth(), 60);
    errorLabel.setCentreRelative(0.5f, 0.4f);

    userIDEditor.setBoundsRelative(0.1f, 0.3f, 0.8f, 0.1f);
    passwordEditor.setBoundsRelative(0.1f, 0.5f, 0.8f, 0.1f);

    authButton.setSize(100, 80);
    authButton.setCentreRelative(0.5f, 0.8f);
    goBackButton.setSize(100, 80);
    goBackButton.setCentreRelative(0.5f, 0.8f);
    tryAgainButton.setSize(100, 80);
    tryAgainButton.setCentreRelative(0.5f, 0.8f);
}

void LoginComponent::dismissAuthUI() {
    userIDLabel.setVisible(false);
    userIDEditor.setVisible(false);
    passwordLabel.setVisible(false);
    passwordEditor.setVisible(false);
    authButton.setVisible(false);
}

void LoginComponent::recoverAuthUI() {
    userIDLabel.setVisible(true);
    userIDEditor.setVisible(true);
    passwordLabel.setVisible(true);
    passwordEditor.setVisible(true);
    authButton.setVisible(true);
}

void LoginComponent::dismissComponent() {
    setVisible(false);
};

void LoginComponent::writeJSON(String inJSON)
{
    String jsonDirectory =
    (juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)).getChildFile("Application Support").getChildFile("Twisted Plugin").getFullPathName() + "/" + "ServerResponse(Not secured)";

    juce::File myJSONFile = juce::File(jsonDirectory + "/" + "ServerResponse" + ".json");

    if (myJSONFile.exists())
    {
        myJSONFile.deleteFile();
    }

    myJSONFile.create();
    myJSONFile.appendText(inJSON);
}

String LoginComponent::readJSON()
{
    String jsonDirectory =
    (juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)).getChildFile("Application Support").getChildFile("Twisted Plugin").getFullPathName() + "/" + "ServerResponse(Not secured)";

    juce::File myJSONFile = juce::File(jsonDirectory + "/" + "ServerResponse" + ".json");

    if (myJSONFile.exists())
    {
        //Read and return json string
        String infoRead = myJSONFile.loadFileAsString();
        return infoRead;
    }
    else
      return "";
}

void LoginComponent::buttonClicked(Button* buttonClicked)
{
    if(buttonClicked == &authButton) {
        String user = userIDEditor.getText();
        String password = passwordEditor.getText();

        request.header("Content-Type", "application/json");

        response = request.post("https://samplehouse.herokuapp.com/api/user/login").field("email", user).field("password", password).execute();

        if(response.result.failed()) {
            dismissAuthUI();

            errorConnectionLabel.setVisible(true);
            errorConnectionLabel.setText(response.result.getErrorMessage(), dontSendNotification);

            goBackButton.setVisible(true);

        } else if(JSON::parse(response.bodyAsString, parsedJson).wasOk()) {
            jwt = parsedJson["token"];

            if(jwt.isNotEmpty()) {
                //Obtaining token string payload
                int charIndex1 = jwt.indexOfAnyOf(".");
                String tempJwt = jwt.substring(charIndex1+1);
                int charIndex2 = tempJwt.indexOfAnyOf(".");

                String tokenPayload = jwt.substring(charIndex1+1, (charIndex1+1)+charIndex2);

                //Adjusting payload for Base64 class
                tokenPayload += String (std::string (tokenPayload.length() % 4, '='));

                //Decoding payload
                MemoryOutputStream decodedStream;
                base64.convertFromBase64(decodedStream, tokenPayload);
                if(JSON::parse(decodedStream.toString(), parsedJson).wasOk())
                {
                    if(parsedJson["vst_access"])
                    {
                        auto jsonString = JSON::toString (parsedJson);
                        writeJSON(jsonString);
                        dismissComponent();
                    } else {
                        dismissAuthUI();
                        errorLabel.setVisible(true);
                        errorLabel.setText("Access denied, check subscription", dontSendNotification);
                        tryAgainButton.setVisible(true);
                    }
                }
            } else {
                dismissAuthUI();
                if(JSON::parse(response.bodyAsString, parsedJson).wasOk()){
                    auto errorMessage = parsedJson["msg"];
                    errorLabel.setVisible(true);
                    errorLabel.setText(errorMessage, dontSendNotification);
                }
                tryAgainButton.setVisible(true);
            }
        }
    }

    if(buttonClicked == &goBackButton) {
        recoverAuthUI();
        errorConnectionLabel.setVisible(false);
        goBackButton.setVisible(false);
    }

    if(buttonClicked == &tryAgainButton) {
        recoverAuthUI();
        errorLabel.setVisible(false);
        tryAgainButton.setVisible(false);
    }
}
