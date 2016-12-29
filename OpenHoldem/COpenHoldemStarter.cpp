//******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: starting and terminating bots automatically
//
//******************************************************************************

#include "stdafx.h"
#include "COpenHoldemStarter.h"

#include "CAutoConnector.h"
#include "CFilenames.h"
#include "CSharedMem.h"
#include "CSessionCounter.h"
#include "CSymbolEngineTime.h"

// For connection and popup handling
const int kMinNumberOfUnoccupiedBotsneeded = 1;
const int kSecondsToWaitBeforeTermination = 600;

COpenHoldemStarter::COpenHoldemStarter() {
}

COpenHoldemStarter::~COpenHoldemStarter() {
}

void COpenHoldemStarter::StartNewInstanceIfNeeded() {
  if (p_sharedmem->NUnoccupiedBots() >= kMinNumberOfUnoccupiedBotsneeded) {
    // Enough instance available for new connections / popup handling
    return;
  }
  if (p_sharedmem->LowestConnectedSessionID() != p_sessioncounter->session_id()) {
    // Only one instance should handle auto-starting.
    // This might delay auto-starting until the first connection, which is OK.
    return;
  }
  //!!!!! if preferences
  // No error-checking, as Openholdem exists (at least when we started).
  // http://msdn.microsoft.com/en-us/library/windows/desktop/bb762153%28v=vs.85%29.aspx
  ShellExecute(
    NULL,               // Pointer to parent window; not needed
    "open",             // "open" == "execute" for an executable
    p_filenames->ExecutableFilename(),
    NULL, 		          // Parameters
    p_filenames->ToolsDirectory(), // Working directory, location of ManualMode
    SW_SHOWNORMAL);		  // Active window, default size
}

void COpenHoldemStarter::CloseThisInstanceIfNoLongerNeeded() {
  if (p_autoconnector->IsConnected()) {
    // Instance needed for playing
    return;
  }
  if (p_sharedmem->NUnoccupiedBots() <= kMinNumberOfUnoccupiedBotsneeded) {
    // Instance needed for new connections / popup handling
    return;
  }
  if (p_symbol_engine_time->elapsedauto() < kSecondsToWaitBeforeTermination) {
    // Don't shut down immediately
    // Instance might be needed soon again
    return;
  }
  if (p_sharedmem->LowestUnconnectedSessionID() != p_sessioncounter->session_id()) {
    // Only one instance should tzerminate at a time
    // to keep one instance available
    return;
  }
  //!!!!! if preferences
  PostQuitMessage(0);
}