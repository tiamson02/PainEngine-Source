/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTSTREAM
#define INC_EXPORTSTREAM


#define BIN_CHECKPOINT 0x12ABCDEF

namespace NS_ExportStream
{

	class Token // A Wrapper for Strings that might be tokenized
	{
		public:
			enum TokenType {NORMAL,CHECKPOINT};

			// Constructors
			Token();
			Token(const char *string, TokenType type=NORMAL); 
			Token(int value, TokenType=NORMAL);
			// Destructor
			~Token(); 
			//
			const char* GetString() const;
			void SetString(const char *string); 

			int GetValue() const;
			void SetValue(int value);
			hkBool GetIsCheckpoint() const;
			void SetIsCheckpoint(hkBool is_check_point);

			hkBool operator==(const Token &another_token) const; 
			hkBool operator==(const char *a_string) const;
			hkBool operator!=(const Token &another_token) const;
			hkBool operator!=(const char *a_string) const;

			hkBool Equal(const Token &another_token) const; 
			hkBool Equal(const char *a_string) const;

			Token &operator=(const Token &another_token);

		private:
			char *priv_string;
			int priv_value;
			hkBool priv_is_checkpoint;

			int PRIV_HashFunction(const char *string) const;
	};

	enum ExportStreamType { EST_ASCII, EST_BIN };

	class ExportStream : public hkReferencedObject
	{
		public:

			HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT);
			ExportStream (ExportStreamType type=EST_ASCII);

			int GetVersion() const {return prot_version;}
			void SetVersion(int version) {prot_version=version;}
			void SetType (ExportStreamType type) {prot_type=type;}
			ExportStreamType GetType() const {return static_cast<ExportStreamType>(prot_type);}
			char *SpacesOut(const char *str);
			void SpacesIn(char *str);

		protected:

			int prot_version;
			ExportStreamType prot_type:8;
			char prot_buffer[100];
	};

} // namespace NS_ExportStream

// This function is called whenever an unexpected situation is detected when importing
// It has different implementations in each platform
void throw_exception(char *message);

#endif // INC_EXPORTSTREAM

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
