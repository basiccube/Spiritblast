#include <string>
#include <filesystem>
#include <stdio.h>
#include <Windows.h>
#include <nfd.hpp>

using namespace std;

#define AURIECORE_URL "https://github.com/AurieFramework/Aurie/releases/download/v2.0.0b/AurieCore.dll"
#define AURIEPATCHER_URL "https://github.com/AurieFramework/Aurie/releases/download/v2.0.0b/AuriePatcher.exe"
#define YYTOOLKIT_URL "https://github.com/AurieFramework/YYToolkit/releases/download/v5.0.0a/YYToolkit.dll"

// This doesn't work for some reason
//#define SPIRITBLAST_URL "https://github.com/basiccube/Spiritblast/releases/latest/download/AB-Mod.dll"
#define SPIRITBLAST_URL "https://github.com/basiccube/Spiritblast/releases/download/0.3/AB-Mod.dll"

LPWSTR convert_string(string str)
{
	wchar_t wstr[512];
	mbstowcs(wstr, str.c_str(), str.length());

	LPWSTR lpstr = wstr;
	return lpstr;
}

bool exec_and_wait(string cmdLine)
{
	// I fucking hate the Win32 API
	STARTUPINFO startInfo;
	PROCESS_INFORMATION procInfo;

	ZeroMemory(&startInfo, sizeof(startInfo));
	ZeroMemory(&procInfo, sizeof(procInfo));
	startInfo.cb = sizeof(startInfo);

	LPWSTR cmdStr = convert_string(cmdLine);
	if (!CreateProcess(NULL, cmdStr, NULL, NULL, FALSE, 0, NULL, NULL, &startInfo, &procInfo))
	{
		printf("Failed to create process : %s\n", cmdLine.c_str());
		return false;
	}
	WaitForSingleObject(procInfo.hProcess, INFINITE);

	CloseHandle(procInfo.hProcess);
	CloseHandle(procInfo.hThread);

	return true;
}

bool download_file(string url, string outPath)
{
	printf("URL : %s\n", url.c_str());
	printf("Output destination : %s\n", outPath.c_str());

	string curlCmd = "curl --retry 3 --create-dirs -L -o \"" + outPath + "\" " + url;
	if (!exec_and_wait(curlCmd))
		return false;

	// Check if the file at the very least was created
	if (!filesystem::exists(outPath))
	{
		printf("File %s doesn't exist, the download might have failed.\n", outPath.c_str());
		return false;
	}
	
	return true;
}

int quit(int result)
{
	NFD::Quit();
	return result;
}

int main(int argc, char *argv[])
{
	NFD::Init();
	NFD::UniquePathU8 outputPath;

	printf("Spiritblast installer\n");

	printf("Select the folder where ANTONBLAST is installed.\n");
	nfdresult_t folderResult = NFD::PickFolder(outputPath);

	if (folderResult == NFD_ERROR)
	{
		printf("Folder selection dialog failed, quitting...\n");
		return quit(1);
	}
	else if (folderResult == NFD_CANCEL)
	{
		printf("Folder path was not specified, quitting...\n");
		return quit(1);
	}

	string outputStr = outputPath.get();
	printf("Path : %s\n", outputStr.c_str());

	string exePath = outputStr + "\\ANTONBLAST.exe";
	if (!filesystem::exists(exePath))
	{
		printf("ANTONBLAST executable not found!\nMake sure you selected the correct directory.\n");
		return quit(1);
	}
	else
		printf("ANTONBLAST executable found\n");

	printf("Downloading AurieCore.dll...\n");
	string aurieCorePath = outputStr + "\\mods\\Native\\AurieCore.dll";
	if (!filesystem::exists(aurieCorePath))
	{
		if (!download_file(AURIECORE_URL, aurieCorePath))
		{
			printf("Failed to download AurieCore!\n");
			return quit(1);
		}
	}
	else
		printf("AurieCore.dll already exists, moving on\n");

	printf("Downloading AuriePatcher.exe...\n");
	string auriePatcherPath = outputStr + "\\AuriePatcher.exe";
	if (!filesystem::exists(auriePatcherPath))
	{
		if (!download_file(AURIEPATCHER_URL, auriePatcherPath))
		{
			printf("Failed to download AuriePatcher!\n");
			return quit(1);
		}
	}
	else
		printf("AuriePatcher.exe already exists, moving on\n");

	printf("Downloading YYToolkit.dll...\n");
	string yyToolkitPath = outputStr + "\\mods\\Aurie\\YYToolkit.dll";
	if (!filesystem::exists(yyToolkitPath))
	{
		if (!download_file(YYTOOLKIT_URL, yyToolkitPath))
		{
			printf("Failed to download YYToolkit!\n");
			return quit(1);
		}
	}
	else
		printf("YYToolkit.dll already exists, moving on\n");

	printf("Downloading Spiritblast.dll...\n");
	string spiritblastPath = outputStr + "\\mods\\Aurie\\Spiritblast.dll";
	if (!download_file(SPIRITBLAST_URL, spiritblastPath))
	{
		printf("Failed to download Spiritblast!\n");
		return quit(1);
	}

	printf("Patching ANTONBLAST.exe...\n");
	if (!exec_and_wait(auriePatcherPath + " \"" + exePath + "\" \"" + aurieCorePath + "\" install"))
	{
		printf("Failed to execute the patcher!\n");
		return quit(1);
	}

	printf("Installation done.\nIf everything went well then launching the game should now have a command prompt window open.\n");
	printf("If this doesn't happen, then something might've went wrong. Maybe try the manual installation method next.\n");
	
	return quit(0);
}