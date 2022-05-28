#pragma once
#ifdef _WIN32
#include <Windows.h>
#include <Shlobj.h>

bool dialog(CLSID which, std::string& path)
{
    bool success = false;

    IFileDialog *pfd = nullptr;
    HRESULT res = CoCreateInstance(which, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

    if (SUCCEEDED(res))
    {
        DWORD dwFlags;
        res = pfd->GetOptions(&dwFlags);
        if (SUCCEEDED(res))
        {
            COMDLG_FILTERSPEC types;
            types.pszName = L"Level Files";
            types.pszSpec = L"*.lvl";
            res = pfd->SetFileTypes(1, &types);

            if (SUCCEEDED(res))
            {
                res = pfd->SetDefaultExtension(L"lvl");

                if (SUCCEEDED(res))
                {
                    res = pfd->Show(nullptr);

                    if (SUCCEEDED(res))
                    {
                        IShellItem *psiResult;
                        res = pfd->GetResult(&psiResult);

                        if (SUCCEEDED(res))
                        {
                            PWSTR file_path = nullptr;
                            res = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &file_path);

                            if (SUCCEEDED(res))
                            {
                                std::wstring wres = file_path;
                                path.assign(wres.begin(), wres.end());
                                CoTaskMemFree(file_path);
                                success = true;
                            }
                        }
                    }
                }
            }
        }
        pfd->Release();
    }

    return success;
}

bool open_file(std::string& path)
{
    return dialog(CLSID_FileOpenDialog, path);
}

bool save_file(std::string& path)
{
    return dialog(CLSID_FileSaveDialog, path);
}

#else

#endif