#pragma once

struct PluginInfo;

//Massively stripped down version of the original, as I didn't care about 90% of its contents.


typedef UInt32	PluginHandle;	// treat this as an opaque type

enum {
	kPluginHandle_Invalid = 0xFFFFFFFF,
};

enum {
	kInterface_Serialization = 0,
	kInterface_Console,

	// Added v0002
	kInterface_Messaging,
	kInterface_CommandTable,

	// Added v0004
	kInterface_StringVar,
	kInterface_ArrayVar,
	kInterface_Script,

	// Added v0005 - version bumped to 3
	kInterface_Data,

	kInterface_Max
};


struct NVSEInterface {
	UInt32	nvseVersion;
	UInt32	runtimeVersion;
	UInt32	editorVersion;
	UInt32	isEditor;
	bool	(*RegisterCommand)(void* info);	// returns true for success, false for failure
	void	(*SetOpcodeBase)(UInt32 opcode);
	void* (*QueryInterface)(UInt32 id);

	// call during your Query or Load functions to get a PluginHandle uniquely identifying your plugin
	// invalid if called at any other time, so call it once and save the result
	PluginHandle(*GetPluginHandle)(void);

	// CommandReturnType enum defined in CommandTable.h
	// does the same as RegisterCommand but includes return type; *required* for commands returning arrays
	bool	(*RegisterTypedCommand)(void* info, UInt32 retnType);
	// returns a full path the the game directory
	const char* (*GetRuntimeDirectory)();

	// Allows checking for nogore edition
	UInt32	isNogore;
};


struct PluginInfo {
	enum {
		kInfoVersion = 1
	};

	UInt32			infoVersion;
	const char* name;
	UInt32			version;
};

typedef bool (*_NVSEPlugin_Query)(const NVSEInterface* nvse, PluginInfo* info);
typedef bool (*_NVSEPlugin_Load)(const NVSEInterface* nvse);

/**** Messaging API docs ********************************************************************
 *
 *	Messaging API allows inter-plugin communication at run-time. A plugin may register
 *	one callback for each plugin from which it wishes to receive messages, specifying
 *	the sender by name in the call to RegisterListener(). RegisterListener returns false
 *	if the specified plugin is not loaded, true otherwise. Any messages dispatched by
 *	the specified plugin will then be forwarded to the listener as they occur. Passing NULL as
 *	the sender registers the calling plugin as a listener to every loaded plugin.
 *
 *	Messages may be dispatched via Dispatch() to either a specific listener (specified
 *	by name) or to all listeners (with NULL passed as the receiver). The contents and format of
 *	messageData are left up to the sender, and the receiver is responsible for casting the message
 *	to the expected type. If no plugins are registered as listeners for the sender,
 *	Dispatch() returns false, otherwise it returns true.
 *
 *	Calling RegisterListener() or Dispatch() during plugin load is not advised as the requested plugin
 *	may not yet be loaded at that point. Instead, if you wish to register as a listener or dispatch a
 *	message immediately after plugin load, use RegisterListener() during load to register to receive
 *	messages from NVSE (sender name: "NVSE"). You will then receive a message from NVSE once
 *	all plugins have been loaded, at which point it is safe to establish communications between
 *	plugins.
 *
 *	Some plugin authors may wish to use strings instead of integers to denote message type. In
 *	that case the receiver can pass the address of the string as an integer and require the receiver
 *	to cast it back to a char* on receipt.
 *
 *********************************************************************************************/

struct NVSEMessagingInterface {
	struct Message {
		const char* sender;
		UInt32		type;
		UInt32		dataLen;
		void* data;
	};

	typedef void (*EventCallback)(Message* msg);

	enum {
		kVersion = 3
	};

	// NVSE messages
	enum {
		kMessage_PostLoad,				// sent to registered plugins once all plugins have been loaded (no data)

		kMessage_ExitGame,				// exit to windows from main menu or in-game menu

		kMessage_ExitToMainMenu,		// exit to main menu from in-game menu

		kMessage_LoadGame,				// Dispatched immediately before plugin serialization callbacks invoked, after savegame has been read by Fallout
										// dataLen: length of file path, data: char* file path of .fos savegame file
										// Receipt of this message does not *guarantee* the serialization callback will be invoked
										// as there may be no .nvse file associated with the savegame

										kMessage_SaveGame,				// as above

										kMessage_Precompile,			// EDITOR: Dispatched when the user attempts to save a script in the script editor.
																		// NVSE first does its pre-compile checks; if these pass the message is dispatched before
																		// the vanilla compiler does its own checks.
																		// data: ScriptBuffer* to the buffer representing the script under compilation

																		kMessage_PreLoadGame,			// dispatched immediately before savegame is read by Fallout
																										// dataLen: length of file path, data: char* file path of .fos savegame file

																										kMessage_ExitGame_Console,		// exit game using 'qqq' console command

																										kMessage_PostLoadGame,			//dispatched after an attempt to load a saved game has finished (the game's LoadGame() routine
																																		//has returned). You will probably want to handle this event if your plugin uses a Preload callback
																																		//as there is a chance that after that callback is invoked the game will encounter an error
																																		//while loading the saved game (eg. corrupted save) which may require you to reset some of your
																																		//plugin state.
																																		//data: bool, true if game successfully loaded, false otherwise */

																																		kMessage_PostPostLoad,			// sent right after kMessage_PostLoad to facilitate the correct dispatching/registering of messages/listeners
																																										// plugins may register as listeners during the first callback while deferring dispatches until the next
																																										kMessage_RuntimeScriptError,	// dispatched when an NVSE script error is encountered during runtime/
																																																		// data: char* errorMessageText
																																								// added for kVersion = 2
																																								kMessage_DeleteGame,			// sent right before deleting the .nvse cosave and the .fos save.
																																																// dataLen: length of file path, data: char* file path of .fos savegame file
																																																kMessage_RenameGame,			// sent right before renaming the .nvse cosave and the .fos save.
																																																								// dataLen: length of old file path, data: char* old file path of .fos savegame file
																																																								// you are expected to save the data and wait for kMessage_RenameNewGame
																																																								kMessage_RenameNewGame,			// sent right after kMessage_RenameGame.
																																																																// dataLen: length of new file path, data: char* new file path of .fos savegame file
																																																																kMessage_NewGame,				// sent right before iterating through plugins newGame.
																																																																								// dataLen: 0, data: NULL
																																																														// added for kVersion == 3
																																																														kMessage_DeleteGameName,		// version of the messages sent with a save file name instead of a save file path.
																																																														kMessage_RenameGameName,
																																																														kMessage_RenameNewGameName,
																																																														// added for kVersion == 4 (xNVSE)
																																																														kMessage_DeferredInit,
																																																														kMessage_ClearScriptDataCache,
																																																														kMessage_MainGameLoop,			// called each game loop
																																																														kMessage_ScriptCompile   // EDITOR: called after successful script compilation in GECK. data: pointer to Script
	};

	UInt32	version;
	bool	(*RegisterListener)(PluginHandle listener, const char* sender, EventCallback handler);
	bool	(*Dispatch)(PluginHandle sender, UInt32 messageType, void* data, UInt32 dataLen, const char* receiver);
};