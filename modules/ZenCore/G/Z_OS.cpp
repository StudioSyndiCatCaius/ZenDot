#include "z_os.h"
#include "core/config/project_settings.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/wait.h>
#endif

void Z_OS::_bind_methods() {
    ClassDB::bind_static_method("Z_OS", D_METHOD("do_Python", "python_script_path", "args", "show_console", "is_blocking"),
        &Z_OS::do_Python);
}

int Z_OS::do_Python(const String &python_script_path, const PackedStringArray &args, bool show_console, bool is_blocking) {

    String abs_path    = ProjectSettings::get_singleton()->globalize_path(python_script_path);
    String working_dir = abs_path.get_base_dir();
    String script_name = abs_path.get_file();

    String cmd = "python \"" + script_name + "\"";
    for (int i = 0; i < args.size(); i++) {
        cmd += " \"" + args[i] + "\"";
    }

#ifdef _WIN32

    HANDLE stdout_read = NULL, stdout_write = NULL;
    HANDLE stderr_read = NULL, stderr_write = NULL;

    SECURITY_ATTRIBUTES sa;
    sa.nLength              = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle       = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&stdout_read, &stdout_write, &sa, 0)) {
        ERR_PRINT("Z_OS::do_Python - Failed to create stdout pipe.");
        return -1;
    }
    if (!CreatePipe(&stderr_read, &stderr_write, &sa, 0)) {
        ERR_PRINT("Z_OS::do_Python - Failed to create stderr pipe.");
        CloseHandle(stdout_read); CloseHandle(stdout_write);
        return -1;
    }

    SetHandleInformation(stdout_read, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(stderr_read, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb         = sizeof(si);
    si.dwFlags    = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdOutput = stdout_write;
    si.hStdError  = stderr_write;
    si.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
    si.wShowWindow = show_console ? SW_SHOW : SW_HIDE;

    DWORD creation_flags = show_console ? CREATE_NEW_CONSOLE : CREATE_NO_WINDOW;

    Char16String cmd16 = cmd.utf16();
    Char16String dir16 = working_dir.utf16();

    Vector<WCHAR> cmd_buf;
    cmd_buf.resize(cmd16.length() + 1);
    memcpy(cmd_buf.ptrw(), cmd16.ptr(), cmd16.length() * sizeof(WCHAR));
    cmd_buf.ptrw()[cmd16.length()] = 0;

    BOOL success = CreateProcessW(
        NULL, cmd_buf.ptrw(),
        NULL, NULL, TRUE,
        creation_flags, NULL,
        (LPCWSTR)dir16.ptr(),
        &si, &pi
    );

    CloseHandle(stdout_write);
    CloseHandle(stderr_write);

    if (!success) {
        ERR_PRINT("Z_OS::do_Python - CreateProcessW failed. WinError: " + itos(GetLastError()));
        CloseHandle(stdout_read); CloseHandle(stderr_read);
        return -1;
    }

    int exit_code = 0;

    if (is_blocking) {
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Print stdout
        char buf[4096];
        DWORD bytes_read = 0;
        String out_str;
        while (ReadFile(stdout_read, buf, sizeof(buf) - 1, &bytes_read, NULL) && bytes_read > 0) {
            buf[bytes_read] = '\0';
            out_str += String::utf8(buf);
        }
        if (!out_str.is_empty()) print_line("Z_OS Python stdout:\n" + out_str);

        // Print stderr
        String err_str;
        while (ReadFile(stderr_read, buf, sizeof(buf) - 1, &bytes_read, NULL) && bytes_read > 0) {
            buf[bytes_read] = '\0';
            err_str += String::utf8(buf);
        }
        if (!err_str.is_empty()) ERR_PRINT("Z_OS Python stderr:\n" + err_str);

        DWORD code = 0;
        GetExitCodeProcess(pi.hProcess, &code);
        exit_code = (int)code;
    }

    CloseHandle(stdout_read);
    CloseHandle(stderr_read);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return exit_code;

#else

    int stdout_pipe[2], stderr_pipe[2];
    if (pipe(stdout_pipe) != 0 || pipe(stderr_pipe) != 0) {
        ERR_PRINT("Z_OS::do_Python - pipe() failed.");
        return -1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);
        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        chdir(working_dir.utf8().get_data());

        Vector<CharString> arg_strings;
        Vector<char*> argv;

        CharString python_cmd = String("python3").utf8();
        argv.push_back(python_cmd.ptrw());

        CharString script = script_name.utf8();
        arg_strings.push_back(script);
        argv.push_back(arg_strings.ptrw()[0].ptrw());

        for (int i = 0; i < args.size(); i++) {
            arg_strings.push_back(args[i].utf8());
            argv.push_back(arg_strings.ptrw()[i + 1].ptrw());
        }
        argv.push_back(nullptr);

        execvp("python3", argv.ptrw());
        _exit(1);
    }

    if (pid < 0) {
        ERR_PRINT("Z_OS::do_Python - fork() failed.");
        close(stdout_pipe[0]); close(stdout_pipe[1]);
        close(stderr_pipe[0]); close(stderr_pipe[1]);
        return -1;
    }

    close(stdout_pipe[1]);
    close(stderr_pipe[1]);

    int exit_code = 0;

    if (is_blocking) {
        char buf[4096];
        ssize_t n;

        String out_str;
        while ((n = read(stdout_pipe[0], buf, sizeof(buf) - 1)) > 0) {
            buf[n] = '\0';
            out_str += String::utf8(buf);
        }
        if (!out_str.is_empty()) print_line("Z_OS Python stdout:\n" + out_str);

        String err_str;
        while ((n = read(stderr_pipe[0], buf, sizeof(buf) - 1)) > 0) {
            buf[n] = '\0';
            err_str += String::utf8(buf);
        }
        if (!err_str.is_empty()) ERR_PRINT("Z_OS Python stderr:\n" + err_str);

        int status = 0;
        waitpid(pid, &status, 0);
        exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }

    close(stdout_pipe[0]);
    close(stderr_pipe[0]);

    return exit_code;

#endif
}
