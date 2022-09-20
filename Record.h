#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <typeinfo>

#include "PlayerData.h"
#include "DataStructures.h"
#include "Constants.h"

#define SPAN size_t
#define BYTE uint8_t


class Record
{
	friend class Analyzer;
	public:
		Record();
		Record(const std::string& path);
		
		~Record() = default;

		Record&			   load(const std::string& path);
		Record&			   parse();
		void			   extract(std::string headerPath, std::string bodyPath) const;
		void			   getMap(std::string path, int width, int height, bool HD = false);
		inline void		   getHDMap(std::string path, int width, int height) { getMap(path, width, height, true); }
		inline std::string getNvN()
		{
			std::string NvN;
			int teamArrSz = sizeof(playerData.teams) / sizeof(Team);
			for (int i = 0; i < teamArrSz; i++)
			{
				if (playerData.teams[i].valid())
				{
					if (NvN.size()) NvN.append("v");
					NvN.append(std::to_string(playerData.teams[i].players.size()));
				}
			}
			return NvN;
		}

		std::string		recPath;
		SPAN			fileLen;
		SPAN			rawHeaderLen;
		SPAN			bodyStart;
		double			parsingTime;
		std::string		rawEncoding;
		std::string		outEncoding;
						
		bool			isAOK;
		bool			isAOC;
		bool			isTrial;
		bool			isHD;
		bool			isHDPatch4;
		bool			isUP;
		bool			isUP15;
		bool			isMsx;
		bool			isAOE2;
						
		std::string		version;
		char			rawVersion[8];
		float			subVersion;
		int32_t			logVersion;
		bool			includeAI;
		int32_t			gameSpeed;
		int16_t			pov;
		int8_t			playerCnt; // GAIA not counted in
		int16_t			gameMode;
		bool			isMultiplayer;
		bool			isInstantBuilding;
		bool			isCheatEnabled;
		int32_t			mapXY[2]; // [x, y] 8bytes
		int32_t			mapID;
		int32_t			mapSize;
		std::string		mapName;
		bool			allVisible;
		bool			fogOfWar;
		int				gameType;
		VictorySettings victory;
		int32_t			difficulty;
		bool			lockTeams;
		int32_t			revealMap;
		int32_t			popLimit;
		bool			lockDiplomacy;
		std::string		scenarioFilename   ; // TODO: string MAX=255?
		AOE2HEADER		aoe2header;
		int32_t			duration;

		PlayerData							playerData;
		std::vector<std::string>			pregameMsg;
		std::vector<IngameMsg>				ingameMsg;
		std::vector<Tribute>				tribute;
		std::map<std::string, std::string>	messages;

	private:
		void _init();
		void _getHeaderRaw();
		int	 _inflateRawHeader();
		void _getBody();
		std::vector<BYTE>					_header;
		std::vector<BYTE>					_body;
		std::ifstream*						_file;
		SPAN								_startInfoPos;
		SPAN								_triggerInfoPos;
		SPAN								_gameSettingPos;
		SPAN								_scenarioHeaderPos;
		BYTE*								_mapBitmap; // Pointer to start of mapbits section
		std::vector<StartObject>			_gaiaObjs;
		std::vector<StartObject>			_playerObjs;
};