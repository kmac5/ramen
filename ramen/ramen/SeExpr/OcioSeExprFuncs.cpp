// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/SeExpr/OcioSeExprFuncs.h>

#include<OpenColorIO/OpenColorIO.h>
namespace OCIO = OCIO_NAMESPACE;

#include<SeExprNode.h>

struct OcioConvertData : public SeExprFuncNode::Data
{
	virtual ~OcioConvertData() {}
	OCIO::ConstProcessorRcPtr processor;
};

class OcioConvertSeExprFuncX : public SeExprFuncX
{
public:

	OcioConvertSeExprFuncX() : SeExprFuncX( true) {}
	
	virtual bool prep( SeExprFuncNode *node, bool wantVec)
	{
		int nargs = node->nargs();
		
		if( nargs != 3)
		{
			node->addError( "3 arguments are required");
			return false;
		}
		
		if( !node->isStrArg( 0))
		{
			node->addError( "first argument must be a string");
			return false;
		}
	
		if( !node->isStrArg( 1))
		{
			node->addError( "second argument must be a string");
			return false;
		}
		
		if( !node->child( 2)->prep( true))
			return false;
		
		OcioConvertData *data = new OcioConvertData();
		std::string from_cs = node->getStrArg( 0);
		std::string to_cs = node->getStrArg( 1);

		try
		{
			OCIO::ConstConfigRcPtr config = OCIO::GetCurrentConfig();
			config->sanityCheck();
			data->processor = config->getProcessor( from_cs.c_str(), to_cs.c_str());
		}
		catch( OCIO::Exception& e)
		{
			node->addError( std::string( "ocio error: ") + e.what());
			return false;
		}
				
		node->setData( ( SeExprFuncNode::Data*) data);
		return true;
	}
	
	virtual void eval( const SeExprFuncNode *node, SeVec3d& result) const
	{
		OcioConvertData *data = ( OcioConvertData*) node->getData();
		
		if( data->processor)
		{
			try
			{
				SeVec3d in_color;
				float pixel[3];
				
				node->child( 2)->eval( in_color);
				
				if( node->child( 2)->isVec())
				{
					for( int i = 0; i < 3; ++i)
						pixel[i] = in_color[i];
				}
				else
				{
					for( int i = 0; i < 3; ++i)
						pixel[i] = in_color[0];
				}
	
				data->processor->applyRGB( pixel);
	
				for( int i = 0; i < 3; ++i)
					result[i] = pixel[i];
				
				return;
			}
			catch( OCIO::Exception& e) {}
		}

		result = SeVec3d( 0, 0, 0);
	}
	
} ocioConvertSeExprFunc;

std::auto_ptr<SeExprFunc> createOcioConvertSeExprFunc()
{
	std::auto_ptr<SeExprFunc> fun( new SeExprFunc( ocioConvertSeExprFunc, 3, 3));
	return fun;
}
