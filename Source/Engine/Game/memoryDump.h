#ifndef  __MEMORYDUMP_H__
# define __MEMORYDUMP_H__


typedef unsigned int CRC32;


namespace Memory{
    class SafeMemory{
    private:
        static const int FILENAMESIZE = 64;
        

        typedef enum{
            STATE_OP_NONE,
            STATE_OP_SAVE,
            STATE_OP_LOAD,
        } StateOp;


        char        stateFilename[FILENAMESIZE];
        StateOp     currentStateOp;
        CRC32       moduleCRC;
        

        void calcCRC();
        bool loadState(const char *filename);
        bool saveState(const char *filename);
    public:
        typedef enum{
            STATE_NONE,
            STATE_SUCCESS,
            STATE_FAILED,
        } StateOpResult;


        static const int BUFSIZE = 4096;
        union{
            unsigned char buffer[BUFSIZE];
            // ...
            // fill the rest of this with proper storage names for the safe memory data
            struct{
                
            };
        };


        //SafeMemory(): constructor: initializes the safe memory area.  Clears out all the memory and
        //  sets the current state operation to do nothing.
        SafeMemory();

        //~SafeMemory(): destructor: does nothing
        ~SafeMemory();


        //registerStateSave(): registers a state save request.  This request will only be queued for
        //  execution.  Nothing will occur until the next time the checkForStateRequest() function is
        //  called.  The filename that is passed in will be copied into a safe memory area.  The 
        //  function returns false if the filename is too long to store, or was not specified.  A 
        //  maximum filename length of 64 characters (including path components) is allowed.  Returns 
        //  true if the request is successfully queued.  Calling this function will overwrite any 
        //  previous state operation that may be queued.
        bool registerStateSave(const char *path);

        //registerStateLoad(): registers a state load request.  This request will only be queued for
        //  execution.  Nothing will occur until the next time the checkForStateRequest() function is
        //  called.  The filename that is passed in will be copied into a safe memory area.  The 
        //  function returns false if the filename is too long to store, or was not specified.  A 
        //  maximum filename length of 64 characters (including path components) is allowed.  Returns
        //  true if the request is successfully queued.  Calling this function will overwrite any 
        //  previous state operation that may be queued.
        bool registerStateLoad(const char *path);


        //checkForStateRequest(): checks if a state operation is currently queued.  If it is, it will
        //  be executed immediately and its result will be returned.  Returns true if no state request
        //  is queued or if no filename has been stored for the operation.
        StateOpResult checkForStateRequest();

#ifdef DEBUG
        void dumpModule() const;
#endif
    };


    extern SafeMemory safe;
}


#endif
