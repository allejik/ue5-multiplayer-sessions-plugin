// Copyright (c) 2022, Oleh Pylypchuk
// All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include "MultiplayerSessions.h"

#define LOCTEXT_NAMESPACE "FMultiplayerSessionsModule"

void FMultiplayerSessionsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FMultiplayerSessionsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMultiplayerSessionsModule, MultiplayerSessions)
