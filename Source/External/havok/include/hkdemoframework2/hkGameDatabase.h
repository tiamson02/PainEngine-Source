/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DEMOFRAMEWORK2_GAMEDATABASE_H
#define HK_DEMOFRAMEWORK2_GAMEDATABASE_H

class hkGame;
class hkGameEnvironment;

class hkGameDatabase : public hkSingleton<hkGameDatabase>
{
	public:

		typedef hkGame* (HK_CALL *GameCreationFunction)(hkGameEnvironment* env);

		hkGameDatabase();

		hkGame* createGame(const char* name, hkGameEnvironment* env);

		int findGame(const char* name);

		struct hkGameEntry;
		const hkObjectArray<hkGameEntry>& getGames() const { return m_games; }

		const hkString& getPrefix() const { return m_prefix; }

		/***********
		* INTERNAL *
		***********/

		// Registered games are formatted into an array of Entries.

		struct hkGameEntry
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DEMO_FRAMEWORK, hkGameEntry);
			hkString m_path;
			GameCreationFunction m_func;
			const char* m_help;
		};

		hkObjectArray<hkGameEntry> m_games;
		hkString m_prefix;


		// Game registration

		struct hkGameEntryRegister
		{
			hkGameEntryRegister( GameCreationFunction func, const char* help, const char* path)
				: m_func(func), m_help(help), m_path(path)
			{
				m_next = s_gameList;
				s_gameList = this;
			}
			GameCreationFunction m_func;
			const char* m_name;
			const char* m_help;
			const char* m_path;
			hkGameEntryRegister* m_next;
		};
		static hkGameEntryRegister* s_gameList;
};

#define HK_DECLARE_GAME(NAME, HELP, PATH) \
	static hkGame* HK_CALL hkCreate ## NAME(hkGameEnvironment* env) { return new NAME(env); } \
	static hkGameDatabase::hkGameEntryRegister hkGameEntryRegister##NAME (&hkCreate##NAME, HELP, PATH)

#endif // HK_DEMOFRAMEWORK2_GAMEDATABASE_H

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
