#include "analyzer.h"
#include "MapTools/MapTools.h"

///< \todo 高版本的DE录像好像不能得到正确的初始视角，需要修复
void DefaultAnalyzer::generateMap(const string path, uint32_t width, uint32_t height, bool hd) {
    if (7 == maptile_type_) {
        return getMap<DefaultAnalyzer, DETile1>(path, this, width, height, hd);
    } else if (9 == maptile_type_) {
        return getMap<DefaultAnalyzer, DETile2>(path, this, width, height, hd);
    } else if (4 == maptile_type_) {
        return getMap<DefaultAnalyzer, Tile1>(path, this, width, height, hd);
    } else if (2 == maptile_type_) {
        return getMap<DefaultAnalyzer, TileLegacy>(path, this, width, height, hd);
    } else {
        logger_->warn(
                "{}(): Unknown _mapTileType.",
                __FUNCTION__);
        _sendExceptionSignal();
        return;
    }
}
