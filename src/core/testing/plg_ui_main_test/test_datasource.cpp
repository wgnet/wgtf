#include "test_datasource.hpp"
#include "pages/test_page.hpp"
#include "core_serialization/serializer/xml_serializer.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "wg_types/binary_block.hpp"
#include "core_command_system/i_command_manager.hpp"
#include <fstream>


namespace wgt
{
namespace {
	static const char * s_objectVersion = "ui_main_ver_1_0_14.";
	static const char * s_objectFile = "generic_app_test_";

	std::string genTestObjFileName( int id )
	{
		std::string s = s_objectFile;
		s += s_objectVersion;
		s += std::to_string(id);
		return s;
	}
}

TestDataSource::TestDataSource( TestDataSourceManager& dataSrcMgr, int id )
	: dataSrcMgr_( dataSrcMgr )
    , testPageId_ ( "" )
	, testPageId2_( "" )
	, description_( std::string("TestDataSource_") + std::to_string(id) )
	, testPage_( nullptr )
	, testPage2_( nullptr )
{
}

TestDataSource::~TestDataSource()
{

}

void TestDataSource::init( IComponentContext & contextManager, int id )
{
	auto defManager = contextManager.queryInterface< IDefinitionManager >();
	if (defManager == NULL)
	{
		return;
	}
	auto objManager = contextManager.queryInterface< IObjectManager >();
	auto fileSystem = contextManager.queryInterface<IFileSystem>();

	std::string objectFile = genTestObjFileName( id );

	if (fileSystem && objManager && defManager)
	{
		if (fileSystem->exists( objectFile.c_str() ))
		{
			IFileSystem::IStreamPtr fileStream = 
				fileSystem->readFile( objectFile.c_str(), std::ios::in | std::ios::binary );
			XMLSerializer serializer( *fileStream, *defManager );

			bool br = serializer.deserialize( testPageId_ );
			assert( br );
			br = serializer.deserialize( testPageId2_ );
			assert( br );
			testPage_ = safeCast<TestPage>(objManager->getObject( testPageId_ ));
			testPage2_ = safeCast<TestPage2>(objManager->getObject( testPageId2_ ));
		}
	}
      objManager->registerListener( &dataSrcMgr_ );
	if (testPage_ == nullptr)
	{
		testPage_ = defManager->create< TestPage >();
		testPage_->init( *defManager );
		RefObjectId id;
		bool ok = testPage_.getId( id );
		assert( ok );
		testPageId_ = id.toString();
	}
	if (testPage2_ == nullptr)
	{
		testPage2_ = defManager->create< TestPage2 >();
		testPage2_->init( *defManager );
		RefObjectId id;
		bool ok = testPage2_.getId( id );
		assert( ok );
		testPageId2_ = id.toString();
	}
    objManager->deregisterListener( &dataSrcMgr_ );
}

void TestDataSource::fini( IComponentContext & contextManager, int id )
{
	auto objManager = contextManager.queryInterface< IObjectManager >();
	auto defManager = contextManager.queryInterface< IDefinitionManager >();
	auto fileSystem = contextManager.queryInterface<IFileSystem>();
	if (objManager && defManager && fileSystem)
	{
		// save objects data
		{
			ResizingMemoryStream stream;
			XMLSerializer serializer( stream, *defManager );
			// save objects' ids which help to restore to the member when loading back
			serializer.serialize( testPageId_ );
			serializer.serialize( testPageId2_ );
			std::string objectFile = genTestObjFileName( id );
			serializer.sync();
			fileSystem->writeFile( 
				objectFile.c_str(), stream.buffer().c_str(), stream.buffer().size(), std::ios::out | std::ios::binary );
		}
	}
	else
	{
		assert( false );
	}
}

const ObjectHandleT< TestPage > & TestDataSource::getTestPage() const
{
	return testPage_;
}

const ObjectHandleT< TestPage2 > & TestDataSource::getTestPage2() const
{
	return testPage2_;
}

const char* TestDataSource::description() const 
{
	return description_.c_str();
}

std::shared_ptr< BinaryBlock > TestDataSourceManager::getThumbnailImage()
{
	std::unique_ptr< char[] > buffer;
	int filesize = 0;	
	if (buffer == nullptr)
	{
#ifndef _WINGDI_ 
#pragma pack(push, 1)
		typedef struct tagBITMAPFILEHEADER 
		{
			unsigned short    bfType;        // must be 'BM' 
			unsigned long    bfSize;        // size of the whole .bmp file
			unsigned short     bfReserved1;   // must be 0
			unsigned short     bfReserved2;   // must be 0
			unsigned long    bfOffBits;     
		} BITMAPFILEHEADER; 

		typedef struct tagBITMAPINFOHEADER
		{
			unsigned long   biSize;            // size of the structure
			long   biWidth;           // image width
			long   biHeight;          // image height
			unsigned short    biPlanes;          // bitplanes
			unsigned short    biBitCount;         // resolution 
			unsigned long   biCompression;     // compression
			unsigned long   biSizeImage;       // size of the image
			long   biXPelsPerMeter;   // pixels per meter X
			long   biYPelsPerMeter;   // pixels per meter Y
			unsigned long  biClrUsed;         // colors used
			unsigned long   biClrImportant;    // important colors
		} BITMAPINFOHEADER;
#pragma pack(pop)
#endif
		char * tmp = nullptr;
		int headersize = sizeof (BITMAPFILEHEADER );
		int infosize = sizeof (BITMAPINFOHEADER );
		filesize = headersize + infosize + 64*64*3;
		buffer.reset( new char[filesize] );
		BITMAPFILEHEADER bmfh;
		BITMAPINFOHEADER info;
		memset ( &bmfh, 0, headersize );
		memset ( &info, 0, infosize);
		bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
		bmfh.bfReserved1 = 0;
		bmfh.bfReserved2 = 0;
		bmfh.bfSize = headersize+ 
			infosize+ 0;
		bmfh.bfOffBits = 0x36;

		info.biSize = infosize;
		info.biWidth = 64;
		info.biHeight = 64;
		info.biPlanes = 1;	
		info.biBitCount = 24;
		info.biCompression = 0L;	
		info.biSizeImage = 0;
		info.biXPelsPerMeter = 0x0ec4;  
		info.biYPelsPerMeter = 0x0ec4;     
		info.biClrUsed = 0;	
		info.biClrImportant = 0; 
		memcpy(buffer.get(), &bmfh, headersize);
		memcpy(buffer.get() +headersize, &info, infosize);
		for (int i = 0; i < 64; i++)
		{
			tmp = (buffer.get() + headersize + infosize) + (i * 64 * 3);
			for (int j = 0; j< 64 * 3; j += 3)
			{
				tmp[j] = i*j % 256;
				tmp[j + 1] = i % 256;
				tmp[j + 2] = j % 256;
			}
		}
	}
	return std::make_shared< BinaryBlock >( buffer.get(), filesize, false );
}

void TestDataSourceManager::onObjectRegistered(const ObjectHandle & pObj)
{
	RefObjectId id;
	bool ok = pObj.getId( id );
	assert( ok );
	loadedObj_.insert( std::make_pair( id, pObj ) );
}
void TestDataSourceManager::onObjectDeregistered(const ObjectHandle & obj )
{
	RefObjectId id;
	bool ok = obj.getId( id );
	assert( ok );
	auto findIt = loadedObj_.find( id );
	if(findIt != loadedObj_.end())
	{
		loadedObj_.erase( findIt );
	}
}

void TestDataSourceManager::init(IComponentContext & contextManager)
{
	contextManager_ = &contextManager;
	auto defManager = contextManager.queryInterface< IDefinitionManager >();
	auto objManager = contextManager.queryInterface< IObjectManager >();
	auto fileSystem = contextManager.queryInterface<IFileSystem>();

	std::string objectFile = s_objectFile;
	objectFile += s_objectVersion;

	if (fileSystem && objManager && defManager)
	{
		if (fileSystem->exists( objectFile.c_str() ))
		{
			IFileSystem::IStreamPtr fileStream = 
				fileSystem->readFile( objectFile.c_str(), std::ios::in | std::ios::binary );
			XMLSerializer serializer( *fileStream, *defManager );

			// read version
			std::string version;
			serializer.deserialize( version );
			if(version == s_objectVersion)
			{
				// load objects
                loadedObj_.clear();
                objManager->registerListener( this );
				defManager->deserializeDefinitions( serializer );
				bool br = objManager->loadObjects( serializer );
				objManager->deregisterListener( this );
				assert( br );
			}
		}
	}
}

void TestDataSourceManager::fini()
{
	assert( contextManager_ );
	for (auto& p : sources_)
	{
		p.second->fini(*contextManager_, p.first);
	}
	auto objManager = contextManager_->queryInterface< IObjectManager >();
	auto defManager = contextManager_->queryInterface< IDefinitionManager >();
	auto fileSystem = contextManager_->queryInterface<IFileSystem>();
	if (objManager && defManager && fileSystem)
	{
		// save objects data
		{
			ResizingMemoryStream stream;
			XMLSerializer serializer( stream, *defManager );
			// write version
			serializer.serialize( s_objectVersion );

			// save objects
			defManager->serializeDefinitions( serializer );
			bool br = objManager->saveObjects( *defManager, serializer );
			assert( br );
			std::string objectFile = s_objectFile;
			objectFile += s_objectVersion;;
			serializer.sync();
			fileSystem->writeFile( 
				objectFile.c_str(), stream.buffer().c_str(), stream.buffer().size(), std::ios::out | std::ios::binary );
		}
	}
	else
	{
		assert( false );
	}
	sources_.resize(0);
	id_ = 0;
}

IDataSource* TestDataSourceManager::openDataSource()
{
	TestDataSource* ds = new TestDataSource( *this, id_ );
	sources_.emplace_back( DataSources::value_type(id_, std::unique_ptr<TestDataSource>(ds)) );
	ds->init(*contextManager_, id_);
	++id_;
	return ds;
}

void TestDataSourceManager::closeDataSource(IDataSource* data)
{
	auto it = std::find_if( sources_.begin(), sources_.end(), 
		[=](const DataSources::value_type& p) { return p.second.get() == data; } );

	assert( it != sources_.end() );
	it->second->fini(*contextManager_, it->first);
	sources_.erase(it);
}
} // end namespace wgt
