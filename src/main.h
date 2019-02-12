#include <Settings.h>
#include <ESPHardware.h>

Settings settings;
ESPHardware espHardware;

//callbacks
void setupModeCallback(bool enterSetupMode);
void factoryResetModeCallback(bool enterFactoryReset);
