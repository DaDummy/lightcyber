/* Lightcyber - 64k Intro by Team210 at Evoke 2k19
 * Copyright (C) 2019 DaDummy <>
 * Copyright (C) 2019 Alexander Kraus <nr4@z10.info>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PAL_WIN32_H
#define PAL_WIN32_H

#include "common.h"

void *malloc(size_t size)
{
	return GlobalAlloc(GMEM_ZEROINIT, size);
}

HWAVEOUT hWaveOut;
WAVEHDR header = { 0, 0, 0, 0, 0, 0, 0, 0 };

HDC hdc;
HGLRC glrc;

int flip_buffers()
{
	SwapBuffers(hdc);

	MSG msg = { 0 };
	while ( PeekMessageA( &msg, NULL, 0, 0, PM_REMOVE ) )
	{
		if ( msg.message == WM_QUIT ) {
			return FALSE;
		}
		TranslateMessage( &msg );
		DispatchMessageA( &msg );
	}

	return TRUE;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_ESCAPE:
					ExitProcess(0);
					break;
				case VK_SPACE:
					// pause/unpaused render timer
					paused = !paused;
					if(paused)
						waveOutPause(hWaveOut);
					else
						waveOutRestart(hWaveOut);
					break;
			}
			break;
		case WM_RBUTTONDOWN:
			ExitProcess(0);
			break;
//         case WM_MOUSEMOVE:
//             mx = GET_X_LPARAM(lParam);
//             my = GET_Y_LPARAM(lParam);
//             break;

		default:
			break;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_COMMAND:
			UINT id =  LOWORD(wParam);
			HWND hSender = (HWND)lParam;

			switch(id)
			{
				case 5:
				{
					int index = SendMessage(hSender, CB_GETCURSEL, 0, 0);
					if(index == 0)
					{
						w = 1920;
						h = 1080;
					}
					else if(index == 1)
					{
						w = 1600;
						h = 900;
					}
					else if(index == 2)
					{
						w = 1280;
						h = 720;
					}
					else if(index == 3)
					{
						w = 960;
						h = 540;
					}
					else if(index == 4)
					{
						w = 1024;
						h = 768;
					}
				}
					break;
				case 6:
					muted = !muted;
					if(muted)
						SendMessage(hSender, BM_SETCHECK, BST_CHECKED, 0);
					else
						SendMessage(hSender, BM_SETCHECK, BST_UNCHECKED, 0);
					break;
				case 7:
					DestroyWindow(hwnd);
					PostQuitMessage(0);
					break;
				case 8: // Full screen Antialiasing
				{
					int index = SendMessage(hSender, CB_GETCURSEL, 0, 0);
					fsaa = (index + 1)*(index + 1);
				}
					break;
				case 9: // Texture buffer size
				{
					int index = SendMessage(hSender, CB_GETCURSEL, 0, 0);
					texs = 128;
					for(int i=0; i<index; ++i)
						texs *= 2;
					block_size = texs*texs;
				}
					break;
				case 10:
				{
					override_index = SendMessage(hSender, CB_GETCURSEL, 0, 0);
					scene_override = override_index > 0;
				}
				break;
			}
			break;

		case WM_CLOSE:
			ExitProcess(0);
			break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI demo(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
#ifdef DEBUG
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
#endif

	// Display settings selector
	WNDCLASS wca = { 0 };
	wca.lpfnWndProc   = DialogProc;
	wca.hInstance     = hInstance;
	wca.lpszClassName = L"Settings";
	RegisterClass(&wca);
	HWND lwnd = CreateWindowEx(
		0,                              // Optional window styles.
		L"Settings",                     // Window class
		demoname,    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		200, 200, 300, 300,

		NULL,       // Parent window
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
		);

	// Add "Resolution: " text
	HWND hResolutionText = CreateWindow(WC_STATIC, "Resolution: ", WS_VISIBLE | WS_CHILD | SS_LEFT, 10,15,100,100, lwnd, NULL, hInstance, NULL);

	// Add resolution Combo box
	HWND hResolutionComboBox = CreateWindow(WC_COMBOBOX, TEXT(""),
	 CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
	 100, 10, 175, 400, lwnd, (HMENU)5, hInstance,
	 NULL);

	// Add items to resolution combo box and select full HD
	const char *fullhd = "1920*1080",
        *resolution_1600x900 = "1600*900",
        *resolution_1280x720 = "1280*720",
		*halfhd = "960*540",
		*normal = "1024*768";
	SendMessage(hResolutionComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (fullhd));
    SendMessage(hResolutionComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (resolution_1600x900));
    SendMessage(hResolutionComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (resolution_1280x720));
	SendMessage(hResolutionComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (halfhd));
	SendMessage(hResolutionComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (normal));
	SendMessage(hResolutionComboBox, CB_SETCURSEL, 2, 0);

	// Add mute checkbox
	HWND hMuteCheckbox = CreateWindow(WC_BUTTON, TEXT("Mute"),
					 WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
					 10, 40, 100, 20,
					 lwnd, (HMENU) 6, hInstance, NULL);

	// Add "Antialiasing: " text
	HWND hAntialiasingText = CreateWindow(WC_STATIC, "FSAA: ", WS_VISIBLE | WS_CHILD | SS_LEFT, 10,65,100,100, lwnd, NULL, hInstance, NULL);

	// Add Fullscreen Antialiasing combo box
	HWND hFSAAComboBox= CreateWindow(WC_COMBOBOX, TEXT(""),
	 CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
	 100, 60, 175, 280, lwnd, (HMENU)8, hInstance,
	 NULL);

	// Populate with entries
	const char *fsaa1= "None",
		*fsaa4 = "4*FSAA",
		*fsaa9 = "9*FSAA",
		*fsaa16 = "16*FSAA",
		*fsaa25 = "25*FSAA",
        *fsaa36 = "36*FSAA";//,
//         *fsaa49 = "49*FSAA",
//         *fsaa64 = "64*FSAA",
//         *fsaa81 = "81*FSAA",
//         *fsaa100 = "100*FSAA";
	SendMessage(hFSAAComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (fsaa1));
	SendMessage(hFSAAComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (fsaa4));
	SendMessage(hFSAAComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (fsaa9));
	SendMessage(hFSAAComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (fsaa16));
	SendMessage(hFSAAComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (fsaa25));
    SendMessage(hFSAAComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (fsaa36));
//     SendMessage(hFSAAComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (fsaa49));
//     SendMessage(hFSAAComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (fsaa64));
//     SendMessage(hFSAAComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (fsaa81));
//     SendMessage(hFSAAComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (fsaa100));
	SendMessage(hFSAAComboBox, CB_SETCURSEL, 5, 0);

	// Add "SFX Buffer: " text
	HWND hTXAAText = CreateWindow(WC_STATIC, "SFX Buffer: ", WS_VISIBLE | WS_CHILD | SS_LEFT, 10,95,100,100, lwnd, NULL, hInstance, NULL);

	// Add SFX buffer size combo box
	HWND hTXAAComboBox= CreateWindow(WC_COMBOBOX, TEXT(""),
	 CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
	 100, 90, 175, 280, lwnd, (HMENU)9, hInstance,
	 NULL);

	// Populate with entries
	const char *buf128= "128^2 px",
		*buf256 = "256^2 px",
		*buf512 = "512^2 px",
		*buf1024 = "1024^2 px";
	SendMessage(hTXAAComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (buf128));
	SendMessage(hTXAAComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (buf256));
	SendMessage(hTXAAComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (buf512));
	SendMessage(hTXAAComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (buf1024));
	//SendMessage(hTXAAComboBox, CB_SETCURSEL, 3, 0);
	SendMessage(hTXAAComboBox, CB_SETCURSEL, 2, 0);

	// Add "Antialiasing: " text
	HWND hSceneText = CreateWindow(WC_STATIC, "Scene: ", WS_VISIBLE | WS_CHILD | SS_LEFT, 10,125,100,100, lwnd, NULL, hInstance, NULL);

	// Add scene selector
	HWND hSceneComboBox = CreateWindow(WC_COMBOBOX, TEXT(""),
	 CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
	 100, 120, 175, 680, lwnd, (HMENU)10, hInstance,
	 NULL);

	// Populate with entries
	const char *all_scenes = "All scenes",
		*logo210_scene = "Logo 210",
        *evoke_scene = "Evoke Logo",
        *graffiti_scene = "Red Graffiti",
        *groundboxes_scene = "Cube",
        *canal_scene = "Honey Hell",
        *graffiti_2_scene = "Blue Graffiti",
        *voronoidesign_scene = "Tentacles",
        *transbubbles_scene = "Bubbles",
        *volclouds_scene = "Smoke",
        *chart_scene = "Chart",
        *greet_scene = "Greetings",
        *volclouds_2_scene = "Smoke II",
        *graffiti_3_scene = "Graffiti III",
        *transbubbles_2_scene = "Bubbles II",
        *voronoidesign_2_scene = "Tentacles II",
        *groundboxes_2_scene = "Cube II",
        *volclouds_3_scene = "Smoke III",
        *transbubbles_3_scene = "Bubbles III",
        *canal_2_scene = "Honey Hell II",
        *groundboxes_3_scene = "Cube III",
        *voronoidesign_3_scene = "Tentacles III",
        *canal_3_scene = "Honey Hell III",
        *graffiti_4_scene = "Graffiti IV",
        *transbubbles_4_scene = "Bubbles IV",
        *volclouds_4_scene = "Smoke IV",
        *groundboxes_4_scene = "Cube IV",
        *voronoidesign_4_scene = "Tentacles V (End)";
	SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (all_scenes));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (logo210_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (evoke_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (graffiti_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (groundboxes_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (canal_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (graffiti_2_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (voronoidesign_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (transbubbles_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (volclouds_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (chart_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (greet_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (volclouds_2_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (graffiti_3_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (transbubbles_2_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (voronoidesign_2_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (groundboxes_2_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (volclouds_3_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (transbubbles_3_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (canal_2_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (groundboxes_3_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (voronoidesign_3_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (canal_3_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (graffiti_4_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (transbubbles_4_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (volclouds_4_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (groundboxes_4_scene));
    SendMessage(hSceneComboBox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) (voronoidesign_4_scene));
	SendMessage(hSceneComboBox, CB_SETCURSEL, 0, 0);

	// Add start button
	HWND hwndButton = CreateWindow(WC_BUTTON,"Offend!",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,185,165,90,90,lwnd,(HMENU)7,hInstance,NULL);

	// Show the selector
	ShowWindow(lwnd, TRUE);
	UpdateWindow(lwnd);

	MSG msg = { 0 };
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

#ifdef DEBUG
	printf("Rendering Demo with:\nSound ");
//     if(muted)printf("muted");
//     else printf("playing");
	printf("\nResolution: %d * %d\n", w, h);
	printf("FSAA: %d*\n", fsaa);
#endif

	// Display demo window
	CHAR WindowClass[]  = "Team210 Demo Window";

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = &WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WindowClass;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

// 	// Get full screen information
// 	HMONITOR hmon = MonitorFromWindow(0, MONITOR_DEFAULTTONEAREST);
// 	MONITORINFO mi = { sizeof(mi) };
// 	GetMonitorInfo(hmon, &mi);

	// Create the window.
	HWND hwnd = CreateWindowEx(
		0,                                                          // Optional window styles.
		WindowClass,                                                // Window class
		":: NR4^QM/Team210 :: GO - MAKE A DEMO ::",                                 // Window text
		WS_POPUP | WS_VISIBLE,                                      // Window style
		0,
		0,
		w,
		h,                     // Size and position

		NULL,                                                       // Parent window
		NULL,                                                       // Menu
		hInstance,                                                  // Instance handle
		0                                                           // Additional application data
	);

    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(dm);
    dm.dmPelsWidth = w;
    dm.dmPelsHeight = h;
    dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
    
    ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
    
    
	// Show it
	ShowWindow(hwnd, TRUE);
	UpdateWindow(hwnd);

	// Create OpenGL context
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
		PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
		32,                   // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                   // Number of bits for the depthbuffer
		8,                    // Number of bits for the stencilbuffer
		0,                    // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	hdc = GetDC(hwnd);

	int  pf = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pf, &pfd);

	glrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, glrc);

    rInitializeRenderer();

    ShowCursor(FALSE);
    
	load_demo();

	// Main loop
	t_start = 0.;
	while(flip_buffers())
	{
		static MMTIME MMTime = { TIME_SAMPLES, 0};
		waveOutGetPosition(hWaveOut, &MMTime, sizeof(MMTIME));
		t_now = ((double)MMTime.u.sample)/( 44100.0);

		draw();
	}

	return 0;
}

void initialize_sound()
{
	hWaveOut = 0;
	int n_bits_per_sample = 16;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, channels, sample_rate, sample_rate*channels*n_bits_per_sample / 8, channels*n_bits_per_sample / 8, n_bits_per_sample, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

	header.lpData = smusic1;
	header.dwBufferLength = 4 * music1_size;
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
}

#endif
