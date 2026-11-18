/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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
			bool GetIsCheckpoint() const;
			void SetIsCheckpoint(bool is_check_point);

			bool operator==(const Token &another_token) const; 
			bool operator==(const char *a_string) const;
			bool operator!=(const Token &another_token) const;
			bool operator!=(const char *a_string) const;

			bool Equal(const Token &another_token) const; 
			bool Equal(const char *a_string) const;

			Token &operator=(const Token &another_token);

		private:
			char *priv_string;
			int priv_value;
			bool priv_is_checkpoint;

			int PRIV_HashFunction(const char *string) const;
	};

	enum ExportStreamType { EST_ASCII, EST_BIN };

	class ExportStream : public hkBaseObject
	{
		public:

			HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
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
