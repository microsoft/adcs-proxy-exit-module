; // PMIExitModule.mc 
; // Copyright (C) Microsoft Corp. All rights reserved.


SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
               Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
               Warning=0x2:STATUS_SEVERITY_WARNING
               Error=0x3:STATUS_SEVERITY_ERROR
              )


FacilityNames=(System=0x0:FACILITY_SYSTEM
               Runtime=0x2:FACILITY_RUNTIME
               Stubs=0x3:FACILITY_STUBS
               Io=0x4:FACILITY_IO_ERROR_CODE
              )

LanguageNames=(English=0x409:MSG00409)

; // The following are the categories of events.

MessageIdTypedef=WORD

MessageId=0x1
SymbolicName=GENERAL_CATEGORY
Language=English
General
.

; // The following are the message definitions.

MessageIdTypedef=DWORD

MessageId=0x100
Severity=Informational
Facility=System
SymbolicName=MSG_PROCESS_SUCCEEDED
Language=English
The process [%1] with main thread id [%2] succeeded with exit code [%3].
.

MessageId=0x101
Severity=Error
Facility=System
SymbolicName=MSG_PROCESS_FAILED
Language=English
The process [%1] with main thread id [%2] failed with exit code [%3]. The temp file [%4] will be preserved for debugging.
.

MessageId=0x102
Severity=Warning
Facility=System
SymbolicName=MSG_PROCESS_TIMEDOUT
Language=English
Timed out waiting %1 seconds for process [%2] with main thread id [%3]. The process was not killed and could still be running. The temp file [%4] will be preserved for debugging.
.

MessageId=0x103
Severity=Informational
Facility=System
SymbolicName=MSG_PROCESS_START_SUCCEEDED
Language=English
Started process [%1] with the following command line [%2]. Process ID=[%3]. Thread ID=[%4].
.

MessageId=0x104
Severity=Error
Facility=System
SymbolicName=MSG_PROCESS_START_FAILED
Language=English
Failed to start process [%1] with the following command line [%2]. HRESULT=%3. %4
.

MessageId=0x105
Severity=Error
Facility=System
SymbolicName=MSG_NOTIFY_FAILED
Language=English
Internal error. Use internal tracing to capture more info and open a bug. ICertExit::Notify(). ExitEvent=%1, Context=%2. HRESULT=%3. %4
.
