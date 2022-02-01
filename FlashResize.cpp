/**
 * A program which starts a standalone flashplayer instance and resizes it to fill the screen. Relies on flashplayer.exe
 * being in %PATH%. At the very least the screen resolution and the OS need to be specified using arguments. Arguments
 * are also available for custom setups where window borders and display are may vary from the default configurations.
 */

#include <stdio.h>
#include <windows.h>

struct Args {
    /**
     * Struct to represent the arguments.
     */
    char *path = nullptr;
    int wLeft = 0;
    int wRight = 0;
    int wTop = 0;
    int wBottom = 0;
    int dLeft = 0;
    int dRight = 0;
    int dTop = 0;
    int dBottom = 0;
};

void ParseArgs(int argc, char *argv[], Args *args) {
    /**
     * Parse arguments from the command line and store them in provided struct.
     *
     * @param int Number of command line arguments.
     * @param char* Command line arguments.
     * @param Args* Pointer to Args struct for storing parsed arguments.
     */
    bool *checked = new bool[argc];
    for (int i = 0; i < argc; i++) checked[i] = false;

    // Resolution.
    for (int i = 1; i < argc; i++) {
        int len = strlen(argv[i]);
        char *lower = new char[len + 1];  // +1 for null character.
        for (int j = 0; j < len; j++) lower[j] = tolower(argv[i][j]);
        lower[len] = '\0';

        if (strncmp(lower, "/r", 2) == 0) {
            char *res = lower + 2;
            char *w = res;
            char *h = strchr(res, 'x');
            h[0] = '\0';
            h += 1;

            args->dLeft = 0;
            args->dRight = atoi(w);
            args->dTop = 0;
            args->dBottom = atoi(h);

            checked[i] = true;
            break;
        }
    }

    // Windows versions.
    for (int i = 1; i < argc; i++) {
        if (checked[i]) continue;

        int len = strlen(argv[i]);
        char *lower = new char[len + 1];  // +1 for null character.
        for (int j = 0; j < len; j++) lower[j] = tolower(argv[i][j]);
        lower[len] = '\0';

        if (strcmp(lower, "/win10") == 0) {
            args->wLeft = 8;
            args->wRight = 8;
            args->wTop = 51;
            args->wBottom = 8;
            args->dBottom -= 40;

            checked[i] = true;
            break;
        } else if (strcmp(lower, "/win11") == 0) {
            args->wLeft = 8;
            args->wRight = 8;
            args->wTop = 51;
            args->wBottom = 8;
            args->dBottom -= 48;

            checked[i] = true;
            break;
        }
    }

    // Window borders and display area overrides.
    for (int i = 1; i < argc; i++) {
        if (checked[i]) continue;

        int len = strlen(argv[i]);
        char *lower = new char[len + 1];  // +1 for null character.
        for (int j = 0; j < len; j++) lower[j] = tolower(argv[i][j]);
        lower[len] = '\0';

        // Window borders.
        if (strncmp(lower, "/wl", 3) == 0) {
            args->wLeft = atoi(lower + 3);
            checked[i] = true;
        } else if (strncmp(lower, "/wr", 3) == 0) {
            args->wRight = atoi(lower + 3);
            checked[i] = true;
        } else if (strncmp(lower, "/wt", 3) == 0) {
            args->wTop = atoi(lower + 3);
            checked[i] = true;
        } else if (strncmp(lower, "/wb", 3) == 0) {
            args->wBottom = atoi(lower + 3);
            checked[i] = true;
        }

        // Display area.
        if (strncmp(lower, "/dl", 3) == 0) {
            args->dLeft = atoi(lower + 3);
            checked[i] = true;
        } else if (strncmp(lower, "/dr", 3) == 0) {
            args->dRight = atoi(lower + 3);
            checked[i] = true;
        } else if (strncmp(lower, "/dt", 3) == 0) {
            args->dTop = atoi(lower + 3);
            checked[i] = true;
        } else if (strncmp(lower, "/db", 3) == 0) {
            args->dBottom = atoi(lower + 3);
            checked[i] = true;
        }
    }

    // File path
    for (int i = 1; i < argc; i++) {
        if (checked[i]) continue;

        int len = strlen(argv[i]);
        char *lower = new char[len + 1];  // +1 for null character.
        for (int j = 0; j < len; j++) lower[j] = tolower(argv[i][j]);
        lower[len] = '\0';

        if (strncmp(lower, "/r", 2) != 0 && strcmp(lower, "/win10") != 0 && strcmp(lower, "/win11") != 0) {
            args->path = argv[i];
            break;
        }
    }
}

struct LParam {
    /**
     * Struct to represent lParam used in EnumWindows.
     */
    DWORD pid = 0;
    HWND hwnd = NULL;

    LParam(DWORD pid, HWND hwnd) : pid(pid), hwnd(hwnd) {}
};

BOOL CALLBACK GetWindowHandle(HWND hwnd, LPARAM lParam) {
    /**
     * Callback for EnumWindows. Checks windows against provided PID and sets hwnd in case of a match.
     *
     * @param HWND Window handle.
     * @param LPARAM LParam with PID to check for and hwnd to set.
     *
     * @return Continue enumeration.
     */
    LParam *lParamU = reinterpret_cast<LParam *>(lParam);
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);

    if (pid == lParamU->pid) {
        // Window found. Set hwnd and stop enumeration.
        lParamU->hwnd = hwnd;
        return FALSE;
    }

    return TRUE;
}

int main(int argc, char *argv[]) {
    /**
     * The main function.
     *
     * @param int Number of command line arguments.
     * @param char* Command line arguments.
     *
     * @return Exit status.
     */
    // Parse arguments and make sure path is specified.
    Args *args = new Args();
    ParseArgs(argc, argv, args);
    if (args->path == nullptr) {
        fprintf(stderr, "Must provide a file path.");
        return EXIT_FAILURE;
    }

    // Convert argument from char* to LPWSTR.
    char *exe = "flashplayer.exe";
    size_t lenExe = strlen(exe);
    size_t lenPath = strlen(args->path);
    size_t lenCl = lenExe + lenPath + 4;  // +4 to account for a space, two quotes and null character.
    LPWSTR cl = new WCHAR[lenCl];
    size_t outLen;
    mbstowcs_s(&outLen, cl, lenCl, exe, lenExe);
    mbstowcs_s(&outLen, cl + lenExe + 2, lenCl - lenExe - 2, args->path, lenPath);

    // Wrap in double quotes and set null character for ending string.
    cl[lenExe] = ' ';
    cl[lenExe + 1] = '"';
    cl[lenExe + lenPath + 2] = '"';
    cl[lenExe + lenPath + 3] = '\0';

    // Initialize handlers.
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Create process.
    if (!CreateProcessW(NULL, cl, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        fprintf(stderr, "CreateProcessW failed (%ld).\n", GetLastError());
        return EXIT_FAILURE;
    }

    // Sleep for 1s so that window is properly initialized.
    Sleep(1000);

    // Get window handle.
    LParam *lParam = new LParam(pi.dwProcessId, NULL);
    EnumWindows(&GetWindowHandle, reinterpret_cast<LPARAM>(&lParam));
    HWND hwnd = FindWindowExW(lParam->hwnd, NULL, NULL, L"Adobe Flash Player 32");

    if (hwnd == NULL) {
        fprintf(stderr, "Could not find window.\n");
        return EXIT_FAILURE;
    }

    // Resize window.
    RECT rect;
    GetWindowRect(hwnd, &rect);
    LONG w = rect.right - rect.left - args->wLeft - args->wRight;
    LONG h = rect.bottom - rect.top - args->wTop - args->wBottom;
    LONG max_w = args->dRight - args->dLeft - args->wLeft - args->wRight;
    LONG max_h = args->dBottom - args->dTop - args->wTop - args->wBottom;

    double ar = static_cast<double>(w) / h;
    double max_ar = static_cast<double>(max_w) / max_h;

    LONG nw = 0;
    LONG nh = 0;
    LONG w_offset = 0;
    LONG h_offset = 0;
    if (ar > max_ar) {
        nw = max_w;
        nh = static_cast<LONG>(nw / ar);
        h_offset = (max_h - nh) / 2;
    } else {
        nh = max_h;
        nw = static_cast<LONG>(nh * ar);
        w_offset = (max_w - nw) / 2;
    }

    SetWindowPos(hwnd, HWND_TOP, args->dLeft + w_offset, args->dTop + h_offset, nw + args->wLeft + args->wRight,
                 nh + args->wTop + args->wBottom, 0);

    // Clean up.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return EXIT_SUCCESS;
}