
#pragma once

#include <PluginBase.h>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

namespace po = boost::program_options;

namespace fs = boost::filesystem;

enum
{
	NUM_PARAMS
};

class Plugin : public PluginBase
{
public:
	string			mCfgPluginId;
	string			mCfgPluginName;
	string			mCfgPluginDescription;
	string			mCfgPluginAbout;
	int				mCfgPluginMajorVersion;
	int				mCfgPluginMinorVersion;
	int				mCfgPluginMinFrames;
	int				mCfgPluginMaxFrames;
	vector<char>	mDescriptionBuffer;
	vector<char>	mAboutBuffer;
	vector<string>	mCfgGeometryShaderFileNames;
	vector<string>	mCfgVertexShaderFileNames;
	vector<string>	mCfgFragmentShaderFileNames;

	static void checkError( void );

public:
	Plugin( void );

	virtual ~Plugin( void );

	virtual void initialise( plugMainUnion &pResult );
	virtual void deinitialise( plugMainUnion &pResult );

	virtual void instantiate( plugMainUnion &pResult );
	virtual void deInstantiate( plugMainUnion &pResult );
};
