#include "SettingsStore.h"

namespace {
    const int THRESHOLD_LOW = 30000;
    const int THRESHOLD_MID = 15000;
    const int THRESHOLD_HIGH = 6000;
}

SettingsStore::SettingsStore() : level(Sensitivity::SENS_MID) {}

Sensitivity SettingsStore::current() const {
    return level;
}

void SettingsStore::next() {
    if (level == Sensitivity::SENS_LOW) level = Sensitivity::SENS_MID;
    else if (level == Sensitivity::SENS_MID) level = Sensitivity::SENS_HIGH;
}

void SettingsStore::previous() {
    if (level == Sensitivity::SENS_HIGH) level = Sensitivity::SENS_MID;
    else if (level == Sensitivity::SENS_MID) level = Sensitivity::SENS_LOW;
}

int SettingsStore::thresholdDelta() const {
    switch (level) {
        case Sensitivity::SENS_LOW: return THRESHOLD_LOW;
        case Sensitivity::SENS_HIGH: return THRESHOLD_HIGH;
        case Sensitivity::SENS_MID:
        default: return THRESHOLD_MID;
    }
}
