// ApexPackageShow.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "ApexPackageShow.h"
#define MAX_LOADSTRING 100

#define WM_MSG_SEND     WM_USER + 0x1000
#define WM_MSG_RECV     WM_USER + 0x1001

template<typename T>
T ReadData(DWORD pid, UINT64 addr)
{
    T val = 0;
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);
    if (hProcess)
    {
        SIZE_T readSize = 0;
        if (!ReadProcessMemory(hProcess, (void*)addr, &val, sizeof(T), &readSize))
        {
            val = 0;
        }

        CloseHandle(hProcess);
    }
    return val;
}


bool ReadBytes(DWORD pid, UINT64 addr, UINT32 len, std::vector<BYTE>& data)
{
    bool succ = false;
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);
    if (hProcess)
    {
        data.resize(len);

        SIZE_T readSize = 0;
        succ = (TRUE == ReadProcessMemory(hProcess, (void*)addr, &data[0], len, &readSize));

        CloseHandle(hProcess);
    }
    return succ;
}
std::string DumpHex(IN const void* buf, int len)
{
    if (buf == nullptr || len <= 0)
        return "{null}";

    std::string str;// = "{";

    char tmp[0x10];

    for (int i = 0; i < len - 1; i++)
    {
        sprintf_s(tmp, "%02X ", (((const char*)buf)[i]) & 0xFF);
        str += tmp;
    }
    if (len > 0)
    {
        sprintf_s(tmp, "%02X", (((const char*)buf)[len - 1]) & 0xFF);
        str += tmp;
    }

    //str += "}";

    return str;
}

#include <sstream>
#include <format>
#include <mutex>
#include <fstream>

std::ofstream fs("d:\\pkg.txt", std::ios::binary);
std::mutex lock;
void Show(const char* title, DWORD pid, UINT64 p)
{
    if (!fs.is_open())
        return;
    UINT32 tid = ReadData<UINT32>(pid, p + 0);
    UINT32 sock = ReadData<UINT32>(pid, p + 8);
    UINT64 buf = ReadData<UINT64>(pid, p + 0x10);
    UINT64 len = ReadData<UINT64>(pid, p + 0x18);

    if (len > 0)
    {
        std::vector<BYTE> v;
        if (ReadBytes(pid, buf, len, v))
        {
            std::string str = std::format("{:s} sock: {:-4x}, tid: {:-4x}, len: {:-4x}, data: {:s}\n",
				title,
                sock, tid, len,
                DumpHex(v.data(), v.size()).c_str());
            lock.lock();
            fs.write(str.c_str(), str.length());
            lock.unlock();
        }
    }

}

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, HWND*);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    AllocConsole();
    //freopen_s(&stream, "CONIN$", "r", stdin);
    FILE* stream;
    freopen_s(&stream, "conout$", "w", stdout);
    // TODO: 在此处放置代码。
    HWND hWnd;

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_APEXPACKAGESHOW, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
	if (!InitInstance(hInstance, SWP_HIDEWINDOW, &hWnd))
    {
        return FALSE;
    }
    printf("窗口句柄: %x(%d)\n", (DWORD)hWnd, (DWORD)hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_APEXPACKAGESHOW));

    MSG msg;


    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    FreeConsole();
    fclose(stream);
    fs.flush();
    fs.close();
    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APEXPACKAGESHOW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_APEXPACKAGESHOW);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND* out_hWnd)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   *out_hWnd = hWnd;

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_MSG_SEND:
    {
        DWORD pid = (UINT32)((UINT64)wParam & 0xFFFFFFFF);
        UINT64 p = (UINT64)lParam;
		Show("send", pid, p);
        break;
    }
    case WM_MSG_RECV:
    {
        UINT32 pid = (UINT32)((UINT64)wParam & 0xFFFFFFFF);
        UINT64 p = (UINT64)lParam;
        
        Show("recv", pid, p);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


