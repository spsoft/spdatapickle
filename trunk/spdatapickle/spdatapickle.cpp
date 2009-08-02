/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include "spdatapickle.hpp"
#include "spdpmetainfo.hpp"

#include "spxml/spxmlutils.hpp"


SP_DataPickle :: SP_DataPickle( SP_DPMetaInfo_t * metaInfo )
{
	mMetaInfo = metaInfo;
}

SP_DataPickle :: ~SP_DataPickle()
{
}

