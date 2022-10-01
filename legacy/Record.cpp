#define cimg_use_png
#define cimg_display 0

#include <string>
#include <fstream>
#include <iostream>
#include <time.h>

#include "Record.h"
#include "zlib.h"
#include "Analyzer.h"
#include "CImg.h"
#include "MapColors.h"
#include "Constants.h"

using namespace std;

Record::Record() { _init(); }

Record::Record(const string& path) : recPath(path)
{ 
    _init();
    load(path);
}

void Record::_init()
{
    pregameMsg.reserve(PREGAMECHAT_MAX);
    ingameMsg.reserve(PREGAMECHAT_MAX);
    outEncoding = "utf-8";
}

Record& Record::load(const std::string& path)
{
    if (_header.size() > 0)
    {
        _header.clear();
        _body.clear();
        messages.clear();
        _gaiaObjs.clear();
        _playerObjs.clear();

        pregameMsg.clear();
        ingameMsg.clear();
        tribute.clear();
        playerData = PlayerData();
        aoe2header = AOE2HEADER();

        scenarioFilename.clear();
        _mapBitmap = nullptr;
        rawEncoding.clear();
    }

    // Open file
    std::ifstream f;
    _file = &f;

    _file->open(recPath = path,
        ifstream::in | ifstream::binary);
    if (!_file->is_open())
    {
        cout << "Failed to open " << path << endl;
        exit(1);
    }

    // Detect raw header and body info
    _getHeaderRaw();

    // Inflate raw header data
    _inflateRawHeader();

    // Read body stream
    _getBody();

    _file->close();

    difficulty = LEVEL_HARDEST;
    gameSpeed = SPEED_NORMAL;
    revealMap = REVEAL_NORMAL;
    gameType = TYPE_RANDOMMAP;
    mapSize = SIZE_TINY;
    mapID = 0;
    popLimit = 0;
    lockDiplomacy = false;

    return *this;
}

Record& Record::parse()
{
    double start, end;
    start = clock();

    Analyzer analyzer;
    analyzer.load(this).parse();

    end = clock();
    parsingTime = (end - start) / CLOCKS_PER_SEC * 1000;

    return *this;
}

void Record::getMap(string path, int width, int height, bool HD)
{
    int versionOffset, versionSpan, elevation, curPos, rbPos; // rb = right bottom

    // Init map
    cimg_library::CImg<unsigned char> img(mapXY[0], mapXY[1], 1, 4, 0xff);

    // prepare raw data
    if ((uint8_t)0xff == _mapBitmap[0])
    {
        versionOffset = 1;
        versionSpan = 4;
    }
    else
    {
        versionOffset = 0;
        versionSpan = 2;
    }

    // populate bits
    cimg_forXY(img, x, y)
    {
        curPos = (y * mapXY[0] + x) * versionSpan + versionOffset;
        rbPos = ((y + 1) * mapXY[0] + x + 1) * versionSpan + versionOffset;

        elevation = 1;
        if (x < mapXY[0] - 1 && y < mapXY[1] -1)
        {
            if (_mapBitmap[curPos + 1] > _mapBitmap[rbPos + 1])
                elevation = 0;
            else if (_mapBitmap[curPos + 1] < _mapBitmap[rbPos + 1])
                elevation = 2;
        }

        img(x, y, 0) = MAP_COLORS[_mapBitmap[curPos]][elevation][0];
        img(x, y, 1) = MAP_COLORS[_mapBitmap[curPos]][elevation][1];
        img(x, y, 2) = MAP_COLORS[_mapBitmap[curPos]][elevation][2];
        img(x, y, 3) = 0xff;
    }

    // draw objects
    unsigned char RGB[4];
    for (auto& obj : _gaiaObjs)
    {
        // NOTE: See this: https://stackoverflow.com/questions/5134614/c-const-map-element-access
        RGB[0] = gaiaColors.at(obj.id).r;
        RGB[1] = gaiaColors.at(obj.id).g;
        RGB[2] = gaiaColors.at(obj.id).b;
        RGB[3] = 0xff;
        img.draw_rectangle(obj.pos[0] - 1, obj.pos[1] - 1, obj.pos[0] + 1, obj.pos[1] + 1, RGB, 1);
    }

    for (auto& obj : _playerObjs)
    {
        Realm* realm = playerData.getRealm(obj.owner);
        if (realm->valid() && obj.isGate())
        {
            img.draw_rectangle(obj.pos[0] - 1, obj.pos[1] - 1, obj.pos[0] + 1, obj.pos[1] + 1, playerColors[realm->color], 1);
        }
    }

    float factor = mapXY[0] / 220.0;
    for (auto& realm : playerData.realms)
    {
        if (!realm.valid()) continue;
        img.draw_circle((int)realm.initState.initCamera[0], (int)realm.initState.initCamera[1], factor * 8, playerColors[realm.color], 0.3);
        img.draw_circle((int)realm.initState.initCamera[0], (int)realm.initState.initCamera[1], factor * 4, playerColors[realm.color], 1);
    }

    // Zoom the map to 3x original size
    if (HD) img.resize_tripleXY();

    // rotate
    img.rotate(-45);

    // resize
    img.resize(width, height);

    // save png
    img.save_png(path.c_str());

    //img.display();
}


void Record::_getHeaderRaw()
{
    // get file size
    _file->seekg(0, _file->end);
    fileLen = _file->tellg();
    _file->seekg(0, _file->beg);

    /*
    headerMeta[0]: header_len + next_pos + header_data (Data length)
    headerMeta[1]: nextPos
    */
    int32_t headerMeta[2];
    _file->read((char*)headerMeta, 8);

    bodyStart = headerMeta[0];

    int rawHeaderStart = headerMeta[1] < fileLen ? 8 : 4;
    rawHeaderLen = (SPAN)headerMeta[0] - rawHeaderStart;
    isAOK = rawHeaderStart == 4;
}

void Record::_getBody()
{
    SPAN bodyLen = fileLen - bodyStart;
    _file->seekg(bodyStart, _file->beg);
    _body.resize(bodyLen);
    _file->read((char*)&_body[0], bodyLen);
}

int Record::_inflateRawHeader()
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[ZLIB_CHUNK];
    unsigned char out[ZLIB_CHUNK];

    _header.reserve(HEADER_INIT);

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, -MAX_WBITS);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        _file->read((char*)&in, ZLIB_CHUNK);
        strm.avail_in = _file->gcount();
        if (!_file->good()) {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = ZLIB_CHUNK;
            strm.next_out = out;

            ret = inflate(&strm, Z_NO_FLUSH);
            // assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }

            have = ZLIB_CHUNK - strm.avail_out;
            _header.insert(
                _header.end(),
                out,
                out + have
            );
            if (!_file->good()) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

void Record::extract(
    std::string headerPath  = "headerStrm.dat", 
    std::string bodyPath    = "bodyStrm.dat"
) const
{
    ofstream headerOut(headerPath, ofstream::binary);
    ofstream bodyOut(bodyPath, ofstream::binary);

    headerOut.write((char*)&_header[0], _header.size());
    bodyOut.write((char*)&_body[0], _body.size());

    headerOut.close();
    bodyOut.close();
}