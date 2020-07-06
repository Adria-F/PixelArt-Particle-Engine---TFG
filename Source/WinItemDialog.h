#pragma once

#include <windows.h>
#include <shobjidl.h> 
#include <string>

std::string openFileWID(const char* extensionHint, const char* extension)
{
	std::string ret;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (SUCCEEDED(hr))
    {
        IFileOpenDialog *pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
                IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
			std::string hint = extensionHint;
			std::string ext = extension;
			if (ext.length() > 0)
			{
				wchar_t wtextA[20];
				mbstowcs(wtextA, hint.c_str(), hint.length() + 1);//Plus null
				LPCWSTR a = wtextA;
				wchar_t wtextB[20];
				mbstowcs(wtextB, ext.c_str(), ext.length() + 1);//Plus null
				LPCWSTR b = wtextB;
				static COMDLG_FILTERSPEC rgSpec[] =
				{
					{ a, b }
				};
				pFileOpen->SetFileTypes(1, rgSpec);
			}
            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem *pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
						std::wstring ws(pszFilePath);
						std::string s(ws.begin(), ws.end());
						ret = s;
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    return ret;
}

std::string saveFileWID(const char* extensionHint, const char* extension)
{
	std::string ret;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr))
	{
		IFileSaveDialog *pFileSave;

		// Create the FileSaveDialog object.
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
			IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		if (SUCCEEDED(hr))
		{			
			std::string text = extensionHint;
			wchar_t wtextA[20];
			mbstowcs(wtextA, text.c_str(), text.length() + 1);//Plus null
			LPCWSTR a = wtextA;
			text = extension;
			wchar_t wtextB[20];
			mbstowcs(wtextB, text.c_str(), text.length() + 1);//Plus null
			LPCWSTR b = wtextB;
			COMDLG_FILTERSPEC rgSpec[] =
			{
				{ a, b }
			};
			pFileSave->SetFileTypes(1, rgSpec);
			// Show the Save dialog box.
			hr = pFileSave->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileSave->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						std::wstring ws(pszFilePath);
						std::string s(ws.begin(), ws.end());
						ret = s;
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileSave->Release();
		}
		CoUninitialize();
	}
	return ret;
}