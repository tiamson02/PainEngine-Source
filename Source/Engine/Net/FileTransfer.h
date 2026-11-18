#error This file is obselete

class FileTransferModule
{
	friend class IPChannel;
public:

	enum Commands
	{
		CAME_NEXT_CHUNK = 0,
		CAME_LAST_CHUNK = 1,
		FILE_NOT_FOUND = 2,
		SEND_FIRST_CHUNK = 3,
		SEND_NEXT_CHUNK = 4,
	};

	FileTransferModule();
	~FileTransferModule() {};

	bool StartDownload( const char* filename );
	void StopActivity( void ) { mDownloading = false; mUploading = false; };

private:
	IPChannel* mChannel;

	bool HandleMessage( UdpMessage* message );
	void HandleNextChunk( UdpMessage* message, unsigned char command );
	void HandleFileStartDownload( UdpMessage* message );

	// Text commands handlers
	void HandleFileStartDownload( DynamicArray<String>& tokens );
	void HandleFileSendNextChunk( void );

	// Binary commands handlers
	void HandleFileNotFound( UdpMessage* message );

	// Variables
	bool mUploading;
	bool mDownloading;
	bool mCannotUpload;

	String mFileName;				// file to download
	float mLastDlPercent;			// last percent got in a packet - information wholly from server
	unsigned long mLastDlPos;		// last position got in a packet - information wholly from server

	String mFileUpName;				// file to upload
	unsigned long mFileUpSize;		// how much of the file have we alredy given out

#ifdef TRANSGAMING
    static const int FILE_CHUNK_SIZE = 1024;

    unsigned char chunkBuf[FILE_CHUNK_SIZE];
#endif
};
