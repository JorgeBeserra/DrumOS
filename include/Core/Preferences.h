#pragma once

namespace DrumOS {
namespace ConfigStore {

bool begin();
bool loadPads();
bool savePads();
bool savePad(int pad);
bool factoryReset();
bool isReady();

}
}
