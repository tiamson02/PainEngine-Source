# -------------------- sysmem --------------------

	.text
	.set	noreorder
	.globl 	sysmem_stub

sysmem_stub:
	.word	0x41e00000
	.word	0x0000
	.short	0x0101
	.short	0x0000
	.ascii	"sysmem\0\0"
	.align	2

	.globl	AllocSysMemory
AllocSysMemory:
	j	$31
	addiu	$0, $0, 4

	.globl	FreeSysMemory
FreeSysMemory:
	j	$31
	addiu	$0, $0, 5

	.globl	QueryMaxFreeMemSize
QueryMaxFreeMemSize:
	j	$31
	addiu	$0, $0, 7

	.globl	Kprintf
Kprintf:
	j	$31
	addiu	$0, $0, 14


	.word	0, 0

# -------------------- intrman --------------------

	.text
	.set	noreorder
	.globl 	intrman_stub

intrman_stub:
	.word	0x41e00000
	.word	0x0000
	.short	0x0102
	.short	0x0000
	.ascii	"intrman\0"
	.align	2

	.globl	CpuSuspendIntr
CpuSuspendIntr:
	j	$31
	addiu	$0, $0, 17

	.globl	CpuResumeIntr
CpuResumeIntr:
	j	$31
	addiu	$0, $0, 18


	.word	0, 0

# -------------------- ioman --------------------

	.text
	.set	noreorder
	.globl 	ioman_stub

ioman_stub:
	.word	0x41e00000
	.word	0x0000
	.short	0x0104
	.short	0x0000
	.ascii	"ioman\0\0\0"
	.align	2

	.globl	open
open:
	j	$31
	addiu	$0, $0, 4

	.globl	close
close:
	j	$31
	addiu	$0, $0, 5

	.globl	read
read:
	j	$31
	addiu	$0, $0, 6

	.globl	lseek
lseek:
	j	$31
	addiu	$0, $0, 8


	.word	0, 0

# -------------------- sifcmd --------------------

	.text
	.set	noreorder
	.globl 	sifcmd_stub

sifcmd_stub:
	.word	0x41e00000
	.word	0x0000
	.short	0x0101
	.short	0x0000
	.ascii	"sifcmd\0\0"
	.align	2

	.globl	sceSifInitRpc
sceSifInitRpc:
	j	$31
	addiu	$0, $0, 14

	.globl	sceSifBindRpc
sceSifBindRpc:
	j	$31
	addiu	$0, $0, 15

	.globl	sceSifCallRpc
sceSifCallRpc:
	j	$31
	addiu	$0, $0, 16

	.globl	sceSifRegisterRpc
sceSifRegisterRpc:
	j	$31
	addiu	$0, $0, 17

	.globl	sceSifSetRpcQueue
sceSifSetRpcQueue:
	j	$31
	addiu	$0, $0, 19

	.globl	sceSifRpcLoop
sceSifRpcLoop:
	j	$31
	addiu	$0, $0, 22


	.word	0, 0

# -------------------- sifman --------------------

	.text
	.set	noreorder
	.globl 	sifman_stub

sifman_stub:
	.word	0x41e00000
	.word	0x0000
	.short	0x0101
	.short	0x0000
	.ascii	"sifman\0\0"
	.align	2

	.globl	sceSifInit
sceSifInit:
	j	$31
	addiu	$0, $0, 5

	.globl	sceSifSetDma
sceSifSetDma:
	j	$31
	addiu	$0, $0, 7

	.globl	sceSifDmaStat
sceSifDmaStat:
	j	$31
	addiu	$0, $0, 8

	.globl	sceSifCheckInit
sceSifCheckInit:
	j	$31
	addiu	$0, $0, 29


	.word	0, 0

# -------------------- stdio --------------------

	.text
	.set	noreorder
	.globl 	stdio_stub

stdio_stub:
	.word	0x41e00000
	.word	0x0000
	.short	0x0103
	.short	0x0000
	.ascii	"stdio\0\0\0"
	.align	2

	.globl	printf
printf:
	j	$31
	addiu	$0, $0, 4


	.word	0, 0

# -------------------- sysclib --------------------

	.text
	.set	noreorder
	.globl 	sysclib_stub

sysclib_stub:
	.word	0x41e00000
	.word	0x0000
	.short	0x0104
	.short	0x0000
	.ascii	"sysclib\0"
	.align	2

	.globl	memcpy
memcpy:
	j	$31
	addiu	$0, $0, 12

	.globl	memset
memset:
	j	$31
	addiu	$0, $0, 14

	.globl	strcat
strcat:
	j	$31
	addiu	$0, $0, 20

	.globl	strcpy
strcpy:
	j	$31
	addiu	$0, $0, 23

	.globl	strlen
strlen:
	j	$31
	addiu	$0, $0, 27

	.globl	strncmp
strncmp:
	j	$31
	addiu	$0, $0, 29

	.globl	strstr
strstr:
	j	$31
	addiu	$0, $0, 34


	.word	0, 0

# -------------------- thbase --------------------

	.text
	.set	noreorder
	.globl 	thbase_stub

thbase_stub:
	.word	0x41e00000
	.word	0x0000
	.short	0x0102
	.short	0x0000
	.ascii	"thbase\0\0"
	.align	2

	.globl	CreateThread
CreateThread:
	j	$31
	addiu	$0, $0, 4

	.globl	DeleteThread
DeleteThread:
	j	$31
	addiu	$0, $0, 5

	.globl	StartThread
StartThread:
	j	$31
	addiu	$0, $0, 6

	.globl	TerminateThread
TerminateThread:
	j	$31
	addiu	$0, $0, 10

	.globl	ChangeThreadPriority
ChangeThreadPriority:
	j	$31
	addiu	$0, $0, 14

	.globl	GetThreadId
GetThreadId:
	j	$31
	addiu	$0, $0, 20

	.globl	SleepThread
SleepThread:
	j	$31
	addiu	$0, $0, 24

	.globl	WakeupThread
WakeupThread:
	j	$31
	addiu	$0, $0, 25

	.globl	iWakeupThread
iWakeupThread:
	j	$31
	addiu	$0, $0, 26

	.globl	DelayThread
DelayThread:
	j	$31
	addiu	$0, $0, 33

	.globl	USec2SysClock
USec2SysClock:
	j	$31
	addiu	$0, $0, 39


	.word	0, 0

# -------------------- thsemap --------------------

	.text
	.set	noreorder
	.globl 	thsemap_stub

thsemap_stub:
	.word	0x41e00000
	.word	0x0000
	.short	0x0101
	.short	0x0000
	.ascii	"thsemap\0"
	.align	2

	.globl	CreateSema
CreateSema:
	j	$31
	addiu	$0, $0, 4

	.globl	DeleteSema
DeleteSema:
	j	$31
	addiu	$0, $0, 5

	.globl	SignalSema
SignalSema:
	j	$31
	addiu	$0, $0, 6

	.globl	iSignalSema
iSignalSema:
	j	$31
	addiu	$0, $0, 7

	.globl	WaitSema
WaitSema:
	j	$31
	addiu	$0, $0, 8


	.word	0, 0

# -------------------- timrman --------------------

	.text
	.set	noreorder
	.globl 	timrman_stub

timrman_stub:
	.word	0x41e00000
	.word	0x0000
	.short	0x0103
	.short	0x0000
	.ascii	"timrman\0"
	.align	2

	.globl	AllocHardTimer
AllocHardTimer:
	j	$31
	addiu	$0, $0, 4

	.globl	FreeHardTimer
FreeHardTimer:
	j	$31
	addiu	$0, $0, 6

	.globl	SetTimerHandler
SetTimerHandler:
	j	$31
	addiu	$0, $0, 20

	.globl	SetupHardTimer
SetupHardTimer:
	j	$31
	addiu	$0, $0, 22

	.globl	StartHardTimer
StartHardTimer:
	j	$31
	addiu	$0, $0, 23


	.word	0, 0

# -------------------- libsd --------------------

	.text
	.set	noreorder
	.globl 	libsd_stub

libsd_stub:
	.word	0x41e00000
	.word	0x0000
	.short	0x0105
	.short	0x0000
	.ascii	"libsd\0\0\0"
	.align	2

	.globl	sceSdInit
sceSdInit:
	j	$31
	addiu	$0, $0, 4

	.globl	sceSdSetParam
sceSdSetParam:
	j	$31
	addiu	$0, $0, 5

	.globl	sceSdGetParam
sceSdGetParam:
	j	$31
	addiu	$0, $0, 6

	.globl	sceSdSetSwitch
sceSdSetSwitch:
	j	$31
	addiu	$0, $0, 7

	.globl	sceSdGetSwitch
sceSdGetSwitch:
	j	$31
	addiu	$0, $0, 8

	.globl	sceSdSetAddr
sceSdSetAddr:
	j	$31
	addiu	$0, $0, 9

	.globl	sceSdGetAddr
sceSdGetAddr:
	j	$31
	addiu	$0, $0, 10

	.globl	sceSdSetCoreAttr
sceSdSetCoreAttr:
	j	$31
	addiu	$0, $0, 11

	.globl	sceSdProcBatch
sceSdProcBatch:
	j	$31
	addiu	$0, $0, 15

	.globl	sceSdVoiceTrans
sceSdVoiceTrans:
	j	$31
	addiu	$0, $0, 17

	.globl	sceSdBlockTrans
sceSdBlockTrans:
	j	$31
	addiu	$0, $0, 18

	.globl	sceSdVoiceTransStatus
sceSdVoiceTransStatus:
	j	$31
	addiu	$0, $0, 19

	.globl	sceSdBlockTransStatus
sceSdBlockTransStatus:
	j	$31
	addiu	$0, $0, 20

	.globl	sceSdSetEffectAttr
sceSdSetEffectAttr:
	j	$31
	addiu	$0, $0, 23

	.globl	sceSdClearEffectWorkArea
sceSdClearEffectWorkArea:
	j	$31
	addiu	$0, $0, 25

	.globl	sceSdSetTransIntrHandler
sceSdSetTransIntrHandler:
	j	$31
	addiu	$0, $0, 26

	.globl	sceSdGetTransIntrHandlerArgument
sceSdGetTransIntrHandlerArgument:
	j	$31
	addiu	$0, $0, 28


	.word	0, 0

# -------------------- cdvdman --------------------

	.text
	.set	noreorder
	.globl 	cdvdman_stub

cdvdman_stub:
	.word	0x41e00000
	.word	0x0000
	.short	0x0101
	.short	0x0000
	.ascii	"cdvdman\0"
	.align	2

	.globl	sceCdInit
sceCdInit:
	j	$31
	addiu	$0, $0, 4

	.globl	sceCdRead
sceCdRead:
	j	$31
	addiu	$0, $0, 6

	.globl	sceCdGetError
sceCdGetError:
	j	$31
	addiu	$0, $0, 8

	.globl	sceCdSearchFile
sceCdSearchFile:
	j	$31
	addiu	$0, $0, 10

	.globl	sceCdSync
sceCdSync:
	j	$31
	addiu	$0, $0, 11

	.globl	sceCdDiskReady
sceCdDiskReady:
	j	$31
	addiu	$0, $0, 13

	.globl	sceCdCallback
sceCdCallback:
	j	$31
	addiu	$0, $0, 37

	.globl	sceCdGetReadPos
sceCdGetReadPos:
	j	$31
	addiu	$0, $0, 44

	.globl	sceCdMmode
sceCdMmode:
	j	$31
	addiu	$0, $0, 75


	.word	0, 0

