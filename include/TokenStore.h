#pragma once

namespace TokenStore {

void save(const char* slack, const char* gitlab);
void load();
const char* slack();
const char* gitlab();

}
