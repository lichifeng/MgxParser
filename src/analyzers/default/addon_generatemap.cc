#include "analyzer.h"
#include "MapTools/MapTools.h"

///< \todo 高版本的DE录像好像不能得到正确的初始视角，需要修复
void DefaultAnalyzer::DrawMap(const string& path, uint32_t width, uint32_t height, bool hd) {
    if (7 == maptile_type_) {
        return GetMap<DefaultAnalyzer, DETile1>(path, this, width, height, hd);
    } else if (9 == maptile_type_) {
        return GetMap<DefaultAnalyzer, DETile2>(path, this, width, height, hd);
    } else if (4 == maptile_type_) {
        return GetMap<DefaultAnalyzer, Tile1>(path, this, width, height, hd);
    } else if (2 == maptile_type_) {
        return GetMap<DefaultAnalyzer, TileLegacy>(path, this, width, height, hd);
    } else {
        throw std::string("Unknown map tile type.");
    }
}
