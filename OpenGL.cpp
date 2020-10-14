// Headers
#include <windows.h>
#include <stdlib.h>
#include <iostream>
#include <gl/GL.h>
#include <gl/glu.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#define APP_WIDTH 800
#define APP_HEIGHT 600

#define X_DELTA 16 
#define Y_DELTA 39

// global variables
DWORD dwStyleAbhijeet;
WINDOWPLACEMENT wpPrevAbhijeet = { sizeof(WINDOWPLACEMENT) };
bool gbFullScreenAbhijeet = false;
HWND ghWndAbhijeet = NULL;
FILE* gpFile = NULL;
bool gbActiveWindow = false;

HDC gHdc = NULL;
HGLRC gHrc = NULL;

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


// WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE HPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
    // function declaration
    void display();
    void initialise();

    // variable declarations
    WNDCLASSEX wndclass;
    HWND hwnd;
    MSG msg;
    TCHAR szAppName[] = TEXT("MyApp");
    bool bDone = false;

    // code
    if (fopen_s(&gpFile, "AGKLog.txt", "w") != 0) {
        MessageBox(NULL, TEXT("Can not open a file"), TEXT("ERROR"), MB_OK);
        exit(0);
    }

    // initialization of WNDCLASSEX
    wndclass.cbSize = sizeof(WNDCLASSEX);
    // CS_OWNDC : keep DC on fixed location in memory
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.lpszClassName = szAppName;
    wndclass.lpszMenuName = NULL;
    wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    // register above class
    RegisterClassEx(&wndclass);

    int desktop_width = GetSystemMetrics(SM_CXSCREEN);
    int desktop_height = GetSystemMetrics(SM_CYSCREEN);

    int x_center_of_desktop = desktop_width / 2;
    int y_center_of_desktop = desktop_height / 2;

    int x_window = x_center_of_desktop - APP_WIDTH / 2;
    int y_window = y_center_of_desktop - APP_HEIGHT / 2;

    // create window
    hwnd = CreateWindowEx(WS_EX_APPWINDOW,
        szAppName,
        TEXT("Abhijeet Kandalkar"),
        // WS_CLIPSIBLINGS : Dont allow sibling to overlap
        // WS_CLIPCHILDREN : Dont' allow children to overlap
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
        x_window,
        y_window,
        APP_WIDTH,
        APP_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL);

    ghWndAbhijeet = hwnd;

    initialise();
    ShowWindow(hwnd, iCmdShow);
    // UpdateWindow(hwnd);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    // message loop with GetMessage is blocking
    //while (GetMessage(&msg, NULL, 0, 0)) {
    //    TranslateMessage(&msg);
    //    DispatchMessage(&msg);
    //}
    while (bDone == false) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                bDone = true;
            }
            else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else {
            if (gbActiveWindow) {
                // call update function for OpenGL rendering

                // call display function for OpenGL rendering
                display();
            }
        }
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    void ToggleFullScreen(void);
    void uninitialised();
    void resize(int, int);

    // code
    switch (iMsg) {
    case WM_PAINT:
        break;
    case WM_CREATE:
        fprintf(gpFile, "%s\n", __FUNCTION__);
        break;
    case WM_DESTROY:
        fprintf(gpFile, "Bye!\n");
        uninitialised();
        PostQuitMessage(0);
        break;
    case WM_SETFOCUS:
        gbActiveWindow = true;
        break;
    case WM_KILLFOCUS:
        gbActiveWindow = false;
        break;
    case WM_ERASEBKGND:
        return 0;
    case WM_SIZE:
        resize(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_ESCAPE:
            DestroyWindow(hwnd);
            break;
        case 0x46: // Ascii for 'F'
        case 0x66: // Ascii for 'f'
            ToggleFullScreen();
            break;
        default:
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    default:
        break;
    }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void ToggleFullScreen(void) {
    MONITORINFO miAbhijeet{ sizeof(MONITORINFO) };

    if (gbFullScreenAbhijeet == false) {
        dwStyleAbhijeet = GetWindowLong(ghWndAbhijeet, GWL_STYLE);
        if (dwStyleAbhijeet & WS_OVERLAPPEDWINDOW) {
            if (GetWindowPlacement(ghWndAbhijeet, &wpPrevAbhijeet) && GetMonitorInfo(MonitorFromWindow(ghWndAbhijeet, MONITORINFOF_PRIMARY), &miAbhijeet)) {
                SetWindowLong(ghWndAbhijeet, GWL_STYLE, dwStyleAbhijeet & ~WS_OVERLAPPEDWINDOW);
                int width = miAbhijeet.rcMonitor.right - miAbhijeet.rcMonitor.left;
                int height = miAbhijeet.rcMonitor.bottom - miAbhijeet.rcMonitor.top;
                std::cout << "w : " << width << " h : " << height << std::endl;
                SetWindowPos(ghWndAbhijeet, HWND_TOP, miAbhijeet.rcMonitor.left, miAbhijeet.rcMonitor.top, width, height, SWP_NOZORDER
                    | SWP_FRAMECHANGED);
            }
        }
        ShowCursor(FALSE);
        gbFullScreenAbhijeet = true;
    }
    else {
        SetWindowLong(ghWndAbhijeet, GWL_STYLE, dwStyleAbhijeet | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(ghWndAbhijeet, &wpPrevAbhijeet);
        SetWindowPos(ghWndAbhijeet, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOZORDER);
        ShowCursor(TRUE);
        gbFullScreenAbhijeet = false;

    }
}

void initialise() {
    void resize(int, int);

    PIXELFORMATDESCRIPTOR pfd;
    int iPixelFormatIndex;

    gHdc = GetDC(ghWndAbhijeet);
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cRedBits = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits = 8;
    pfd.cAlphaBits = 8;

    iPixelFormatIndex = ChoosePixelFormat(gHdc, &pfd);
    if (iPixelFormatIndex == 0) {
        fprintf(gpFile, "Pixel format error\n");
        DestroyWindow(ghWndAbhijeet);
    }

    if (SetPixelFormat(gHdc, iPixelFormatIndex, &pfd) == FALSE) {
        fprintf(gpFile, "SetPixelFormat failed\n");
        DestroyWindow(ghWndAbhijeet);
    }

    gHrc = wglCreateContext(gHdc);
    if (gHrc == NULL) {
        fprintf(gpFile, "wglCreateContext failed\n");
        DestroyWindow(ghWndAbhijeet);
    }

    if (wglMakeCurrent(gHdc, gHrc) == FALSE) {
        fprintf(gpFile, "wglMakeCurrent failed\n");
        DestroyWindow(ghWndAbhijeet);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // warm resize
    resize(APP_WIDTH - X_DELTA, APP_HEIGHT - Y_DELTA);
}

void resize(int width, int height) {
    if (height == 0)
        height = 1;
    fprintf(gpFile, "%s h: %d w : %d\n", __FUNCTION__, width, height);
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
}

void display() {
    fprintf(gpFile, "%s\n", __FUNCTION__);
    void MultiColorTriangle();

    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, -3.0f);

    MultiColorTriangle();

	SwapBuffers(gHdc);
}

void WhiteColorTriangle() {
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 0.1f, 0.0f);
    glVertex3f(-0.1f, -0.1f, 0.0f);
    glVertex3f(0.1f, -0.1f, 0.0f);
    glEnd();
}

void MultiColorTriangle() {
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); //Red
    glVertex3f(0.0f, 1.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f); // Green
    glVertex3f(-1.0f, -1.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f); // Blue
    glVertex3f(1.0f, -1.0f, 0.0f);
    glEnd();
}

void WhiteColorRectangle() {
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(0.1f, 0.1f, 0.0f);
    glVertex3f(-0.1f, 0.1f, 0.0f);
    glVertex3f(-0.1f, -0.1f, 0.0f);
    glVertex3f(0.1f, -0.1f, 0.0f);
    glEnd();
}

void uninitialised() {
    if (gbFullScreenAbhijeet) {
        dwStyleAbhijeet = GetWindowLong(ghWndAbhijeet, GWL_STYLE);
        SetWindowLong(ghWndAbhijeet, GWL_STYLE, dwStyleAbhijeet | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(ghWndAbhijeet, &wpPrevAbhijeet);
        SetWindowPos(ghWndAbhijeet, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOZORDER);
        ShowCursor(TRUE);
    }

    if (wglGetCurrentContext() == gHrc) {
        wglMakeCurrent(NULL, NULL);
    }

    if (gHrc) {
        wglDeleteContext(gHrc);
        gHrc = NULL;
    }

    if (gHdc) {
        ReleaseDC(ghWndAbhijeet, gHdc);
        gHdc = NULL;
    }

    if (gpFile) {
        fprintf(gpFile, "File closed\n");
        fclose(gpFile);
        gpFile = NULL;
    }
}
