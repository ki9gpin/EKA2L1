/*
 * main.cpp
 *
 *  Created on: Dec 19, 2018
 *      Author: fewds
 */

#include <e32base.h>
#include <e32debug.h>
#include <e32std.h>

// Add test file here
#include <intests/cmd/cmd.h>
#include <intests/ecom.h>
#include <intests/fate.h>
#include <intests/fbs/bitmap.h>
#include <intests/fbs/font.h>
#include <intests/io/file.h>
#include <intests/ipc/ipc.h>
#include <intests/kern/chunk.h>
#include <intests/kern/codeseg.h>
#include <intests/testmanager.h>
#include <intests/ws/ws.h>

#include <w32std.h>

void MainWrapperL() {
#if GEN_TESTS
    TAbsorberMode mode = EAbsorbWrite;
#else
    TAbsorberMode mode = EAbsorbVerify;
#endif

    // Connect!
    RFbsSession::Connect();

    instance = CTestManager::NewLC(mode);

    // Add all tests back
    AddWsTestCasesL();
    AddIpcTestCasesL();
    AddKernChunkTestCasesL();
    AddCodeSegTestCasesL();
    AddCmdTestCaseL();
    AddFileTestCasesL();
    AddEComTestCasesL();
    AddFbsFontTestCasesL();
    AddFbsBitmapTestCasesL();

    TInt totalPass = instance->Run();
    RDebug::Printf("%d/%d tests passed", totalPass, instance->TotalTests());

    CleanupStack::PopAndDestroy();
}

TInt E32Main() {
    // Setup the stack
    CTrapCleanup *cleanup = CTrapCleanup::New();
    TRAPD(err, MainWrapperL());

    // Returns to the dust
    delete cleanup;

    return err;
}
