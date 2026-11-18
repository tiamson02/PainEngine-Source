/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdemoframework2/hkDemoFramework.h>
#include <hkdemoframework2/hkGameDatabase.h>
#include <hkbase/stream/hkStringStream.h>
#include <hkdemoframework2/hkTextDisplay.h>

hkGameDatabase::hkGameDatabase()
{
	hkGameEntryRegister* e = s_gameList;
	
	// normalize and collect all game names.

	while(e)
	{
		hkString path = e->m_path;
		path = path.replace("\\","/");
		int lastSlash = path.lastIndexOf('/');
		HK_ASSERT2(lastSlash >= 0, "probably a metrowerks build with no 'pragma fullpath_file on': " << path );
		hkGameEntry entry;
		entry.m_func = e->m_func;
		entry.m_help = e->m_help;
		entry.m_path = path.substr(0, lastSlash);
		m_games.pushBack(entry);
		e = e->m_next;
	}

	HK_ASSERT2(m_games.getSize() != 0, "No games registered!");
	
	// find the common root, be careful if there is only one game

	{
		m_prefix = m_games[0].m_path;
		int lastSlash = m_prefix.lastIndexOf('/');
		if(lastSlash >= 0)
		{
			#ifdef HK_PLATFORM_WIN32
				m_prefix = m_prefix.substr(0, lastSlash + 1).asLowerCase();
			#else
				m_prefix = m_prefix.substr(0, lastSlash + 1);
			#endif
		}
	}
	int i;

	for(i = 0; i < m_games.getSize() && m_prefix.getLength() != 0; ++i)
	{

#ifdef HK_PLATFORM_WIN32
		hkString gp = m_games[i].m_path.asLowerCase();
#else
		hkString gp = m_games[i].m_path;
#endif

		if( !gp.beginsWith(m_prefix) )
		{
			// commonRoot is foo/bar/ we want the second last /
			int lastSlash = m_prefix.lastIndexOf('/', 0, m_prefix.getLength() - 1 );
			if(lastSlash >= 0)
			{
				// want the trailing / in the commonRoot
				m_prefix = m_prefix.substr(0, lastSlash + 1);
				
				--i; // check this one again
			}
			else
			{
				m_prefix = "";
			}
		}
	}

	// strip the common root plus from the names

	int prefixLen = m_prefix.getLength();

	int size = m_games.getSize();

	for(i = 0; i < size; ++i)
	{
		// prune the prefix from each name 
		hkString& n = m_games[i].m_path;
		n = n.substr(prefixLen, n.getLength() - prefixLen);
	}
	// Sort the array.

	// For each element of name.
	for (i = 0; i < size; i++)
	{
		// Find the next
		int least = i;
		for (int j = i+1; j < size; j++)
		{
			if ( m_games[j].m_path < m_games[least].m_path )
			{
				least = j;
			}
		}
		if( least != i )
		{
			hkAlgorithm::swap( m_games[i], m_games[least] );
		}
	}
}


class ErrorGame : public hkGame
{
	public:

		ErrorGame(hkGameEnvironment* env, const char* failed)
			: m_env(env), m_failed(failed)
		{
		}

		Result stepGame()
		{
			hkOstringStream os;
			os << "Creation of game '" << m_failed << "' failed. Was it registered?";
			m_env->m_textDisplay->outputText(os.getString(), 20, 20);
			return GAME_OK;			
		}

		hkGameEnvironment* m_env;
		const char* m_failed;
};


int hkGameDatabase::findGame(const char* cname)
{
	hkString name = hkString(cname);//.asLowerCase();
	for(int i = 0; i < m_games.getSize(); ++i)
	{
		const hkString& n = m_games[i].m_path;
		if( n.endsWith(name) )
		{
			return i;
		}
	}
	return -1;
}

hkGame* hkGameDatabase::createGame(const char* name, hkGameEnvironment* env)
{
	int i = findGame(name);
	if( i != -1 )
	{
		env->m_path = m_games[i].m_path;
		return (*m_games[i].m_func)(env);
	}

	return new ErrorGame(env, name);
}


#if defined(HK_COMPILER_MWERKS)
#	pragma force_active on
#endif

hkGameDatabase::hkGameEntryRegister* hkGameDatabase::s_gameList;
HK_SINGLETON_IMPLEMENTATION(hkGameDatabase);

/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
