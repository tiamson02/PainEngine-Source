#ifndef  __SIMPLETIMING_H__
# define __SIMPLETIMING_H__

extern bool g_gatherTiming;

#if defined(SIMPLE_TIMING) && defined(TRANSGAMING)

#ifdef TRANSGAMING_PSP
# define GRABTIME               PSP_GetCurrentTimeInSeconds
# define CALCTIME(start, end)   (end - start)
#elif defined TRANSGAMING_PS2
# define GRABTIME               PS2_GetTimerSystemTime
# define CALCTIME(start, end)   PS2_GetTimeInSeconds(start, end)
#elif defined TRANSGAMING_WINDOWS
# define GRABTIME               GetCurrentTime
# define CALCTIME(start, end)   (end - start)
#endif

# define DBG_ENABLETIMING()                 { g_gatherTiming = true; }
# define DBG_DISABLETIMING()                { g_gatherTiming = false; }


# ifdef TRANSGAMING_PSP
#  define DBG_TIMER_DEFINE(n)               float           _localTimer[n] = {0};\
                                            const char *    _timerTags[n] = {NULL};\
                                            const int       _timerCount = n;\
                                            int             _timerMax = 0;
# elif defined TRANSGAMING_PS2
#  define DBG_TIMER_DEFINE(n)               ulong32   _localTimer[n] = {0};\
                                            const char *    _timerTags[n] = {NULL};\
                                            const int       _timerCount = n;\
                                            int             _timerMax = 0;
#elif defined TRANSGAMING_WINDOWS
#  define DBG_TIMER_DEFINE(n)               ulong32   _localTimer[n] = {0};\
                                            const char *    _timerTags[n] = {NULL};\
                                            const int       _timerCount = n;\
                                            int             _timerMax = 0;
# endif
            

# define DBG_TIMER_GRABTIME(tag, n) \
                if (g_gatherTiming){\
                    _localTimer[n] = GRABTIME();\
                    _timerTags[n] = tag;\
                    if (n > _timerMax) _timerMax = n;\
                }

# define DBG_TIMER_DUMP(msg)        \
                if (g_gatherTiming){\
                    _localTimer[_timerCount - 1] = GRABTIME();\
                    TRACE("%s\n", msg);\
                    for (int _i = 0; _i < _timerMax; _i++){\
                        if (_localTimer[_i + 1] == 0){\
                            TRACE("  %-8s: %g\n", _timerTags[_i], CALCTIME(_localTimer[_i], _localTimer[_timerCount - 1]));\
                            break;\
                        }\
                        else\
                            TRACE("  %-8s: %g\n", _timerTags[_i], CALCTIME(_localTimer[_i], _localTimer[_i + 1]));\
                    }\
                    TRACE("  %-8s: %g\n", "Total", CALCTIME(_localTimer[0], _localTimer[_timerCount - 1]));\
                }

                    
#else  //!SIMPLE_TIMING
# define DBG_ENABLETIMING()
# define DBG_DISABLETIMING()
# define DBG_TIMER_DEFINE(n)
# define DBG_TIMER_GRABTIME(tag, n)
# define DBG_TIMER_DUMP(msg)
#endif //!SIMPLE_TIMING

#endif
