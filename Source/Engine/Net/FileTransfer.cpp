#error This file is obselete

#include "Engine.h"

FileTransferModule::FileTransferModule( void )
{
	mUploading = false;
	mDownloading = false;
	mCannotUpload = false;

	mChannel = 0;
	mFileName = "";
	mLastDlPercent = 0.0f;
	mLastDlPos = 0;
	mFileUpName = "";
	mFileUpSize = 0;
}

// This returns false if the message is unrecognized and cannot be subtracted from the buffer
bool FileTransferModule::HandleMessage( UdpMessage* message )
{
	unsigned char command = ( message->ReadUChar() & 0x0F );
	switch( command )
	{
	case CAME_NEXT_CHUNK:
	case CAME_LAST_CHUNK:
		HandleNextChunk( message, command );
		break;
	case FILE_NOT_FOUND:
		HandleFileNotFound( message );
		break;
	case SEND_FIRST_CHUNK:
		HandleFileStartDownload( message );
		break;
	case SEND_NEXT_CHUNK:
		HandleFileSendNextChunk();
		break;
	default:
		GLog.Print( "Unknown file transfer command received: %d\n", command );
		return false;
	}
	return true;
}

void FileTransferModule::HandleNextChunk( UdpMessage* message, unsigned char command )
{
	if( !mDownloading ) return;	// we're not downloading...

	mLastDlPercent = message->ReadFloat();			// percent
	mLastDlPos = message->ReadULong();				// position of chunk
	unsigned short size = message->ReadUShort();	// size of chunk

	FileObject* file = GFileMan.CreateFileWriter( mFileName.AsChar(), true );	// create to write & append

	file->Seek(mLastDlPos);
	Assert( (unsigned long)file->Tell() == mLastDlPos );

#ifdef TRANSGAMING
    if (size){
        message->ReadData(chunkBuf, size);	// read chunk from message
        file->Write(chunkBuf, size);			// write chunk to file
    }
#else
	static char chunk[FILE_CHUNK_SIZE];
	if( size )
	{
		message->ReadData( chunk, size );	// read chunk from message
		file->Write( chunk, size );			// write chunk to file
	}
#endif

	delete file;						// close file

	if( command == CAME_LAST_CHUNK )	// End Transmission
	{
//		if( mNetworkClient )
//			mNetworkClient->HandleFileCompleted( mFileName );

		mDownloading = false;
		mLastDlPercent = 0.0f;
		mLastDlPos = 0;
		mFileName = "";
	}
	else
	{
        m_setTag("Net");
		UdpMessage* mesg = new UdpMessage();
		mesg->WriteUChar( IPChannel::FILETRANSFER|SEND_NEXT_CHUNK );	// command
		mesg->EndWriting();
		mChannel->SendReliable( mesg );						// send message
	}
}

void FileTransferModule::HandleFileNotFound( UdpMessage* message )
{
//	if( mNetworkClient )
//		mNetworkClient->HandleFileNotFound( mFileName );
	mDownloading = false;
	mLastDlPercent = 0.0f;
	mLastDlPos = 0;
	mFileName = "";
}

void FileTransferModule::HandleFileStartDownload( UdpMessage* message )
{
	if( mUploading || mCannotUpload ) return;		// no way - we're uploading already

	String filename = message->ReadString();

    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();

	FileObject* file = GFileMan.CreateFileReader( filename.AsChar() );	// filename

	if( !file )	// file not found response
	{
		mesg->WriteUChar( IPChannel::FILETRANSFER|FILE_NOT_FOUND );
		mesg->EndWriting();
		mChannel->SendReliable( mesg );						// send message
		return;
	}

	unsigned long fileSize = file->GetSize();

	unsigned short chunkSize;
	unsigned char command;
	unsigned long oldPos = message->ReadULong();

	if( fileSize - oldPos > FILE_CHUNK_SIZE )
	{
		mUploading = true;
		mFileUpName = filename;
		chunkSize = FILE_CHUNK_SIZE;
		command = CAME_NEXT_CHUNK;
		mFileUpSize = oldPos + FILE_CHUNK_SIZE;
	}
	else
	{
		mUploading = false;
		mFileUpName = "";
		chunkSize = (unsigned short)(fileSize-oldPos);
		command = CAME_LAST_CHUNK;
		mFileUpSize = 0;
	}

	mesg->WriteUChar( IPChannel::FILETRANSFER|command );	// command
	mesg->WriteFloat( 0.0f );					// command part - file percent
	mesg->WriteULong( 0 );						// command part - position of chunk
	mesg->WriteUShort( chunkSize );				// command part - size of chunk

#ifdef TRANSGAMING
    if (chunkSize){
        file->Read(chunkBuf, chunkSize);			// read chunk from file
        mesg->WriteData(chunkBuf, chunkSize);	// write chunk to message
    }
#else
	static char chunk[FILE_CHUNK_SIZE];
	if( chunkSize )
	{
		file->Read( chunk, chunkSize );			// read chunk from file
		mesg->WriteData( chunk, chunkSize );	// write chunk to message
	}
#endif
	mesg->EndWriting();
	mChannel->SendReliable( mesg );						// send message
	delete file;								// close file
}

void FileTransferModule::HandleFileSendNextChunk( void )
{
	if( !mUploading || mCannotUpload ) return;		// no way - we're not uploading yet

    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();

	FileObject* file = GFileMan.CreateFileReader( mFileUpName.AsChar() );

	if( !file )	// file not found response
	{
		mesg->WriteUChar( IPChannel::FILETRANSFER|FILE_NOT_FOUND );
		mesg->EndWriting();
		mChannel->SendReliable( mesg );					// send message
		return;
	}

	unsigned long fileSize = file->GetSize();
	file->Seek( mFileUpSize );

	unsigned short chunkSize;
	unsigned char command;
	unsigned long oldPos = mFileUpSize;

	if( fileSize - oldPos > FILE_CHUNK_SIZE )
	{
		chunkSize = FILE_CHUNK_SIZE;
		command = CAME_NEXT_CHUNK;
		mFileUpSize += FILE_CHUNK_SIZE;
	}
	else
	{
		mUploading = false;
		mFileUpName = "";
		mFileUpSize = 0;
		chunkSize = (unsigned short)(fileSize-oldPos);
		command = CAME_LAST_CHUNK;
	}

	mesg->WriteUChar( IPChannel::FILETRANSFER|command );		// command
	mesg->WriteFloat( (((float)oldPos)/fileSize)*100.0f );		// command part - file percent
	mesg->WriteULong( oldPos );					// command part - position of chunk
	mesg->WriteUShort( chunkSize );				// command part - size of chunk

#ifdef TRANSGAMING
	if (chunkSize){
		file->Read(chunkBuf, chunkSize);				// read chunk from file
		mesg->WriteData(chunkBuf, chunkSize);		// write chunk to message
	}
#else
	static char chunk[FILE_CHUNK_SIZE];
	if( chunkSize )
	{
		file->Read( chunk, chunkSize );				// read chunk from file
		mesg->WriteData( chunk, chunkSize );		// write chunk to message
	}
#endif
	mesg->EndWriting();
	mChannel->SendReliable( mesg );						// send message
	delete file;								// close file
}

bool FileTransferModule::StartDownload( const char* filename )
{
	if( mDownloading ) return false;	// no way - we're downloading already

	FileObject* file = GFileMan.CreateFileReader( filename );
	mFileName = filename;
	int size = file?file->GetSize():0;

    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();
	mesg->WriteUChar( IPChannel::FILETRANSFER|SEND_FIRST_CHUNK );	// command
	mesg->WriteString( filename );				// command part - filename
	mesg->WriteULong( size );					// command part - filesize
	mesg->EndWriting();
	mChannel->SendReliable( mesg );						// send message

	mDownloading = true;
	mLastDlPercent = 0.0f;
	mLastDlPos = 0;
	return true;
}
